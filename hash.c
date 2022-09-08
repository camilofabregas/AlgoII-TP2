#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#define TAM_INICIAL 5
#define FACTOR_CARGA_TECHO 0.7
#define FACTOR_CARGA_PISO 0.3
#define FACTOR_REDIMENSION 2

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef enum {VACIO = 0, BORRADO, OCUPADO} estado_t;


typedef struct campo_hash {
	char* clave;
	void* valor;
	estado_t estado;
} campo_hash_t;


struct hash {
	campo_hash_t* tabla;
	size_t tam;
	size_t cant;
	size_t borrados;
	hash_destruir_dato_t destruir_dato;
};


struct hash_iter {
	const hash_t* hash;
	size_t actual;
};


/* ******************************************************************
 *                      FUNCIONES AUXILIARES
 * *****************************************************************/

// RS Hashing, hecho por Robert Sedgewick.
// Fuente: https://www.programmingalgorithms.com/algorithm/rs-hash/c/
unsigned long hashing(const char* str) {
	unsigned long length = (unsigned long)strlen(str);
	unsigned long b = 378551;
	unsigned long a = 63689;
	unsigned long hash = 0;
	unsigned long i = 0;
	for (i = 0; i < length; str++, i++) {
		hash = hash * a + (*str);
		a = a * b;
	}
	return hash;
}

// Permite encontrar la proxima posicion ocupada.
// Pre: recibe la posicion actual y el hash.
// Post: devuelve la posicion mas cercana al actual donde hay un ocupado.
size_t hallar_nuevo_ocupado(size_t actual, const hash_t* hash) {
	size_t cont = actual;
	while(hash->tam > cont && hash->tabla[cont].estado != OCUPADO) {
		cont += 1;
	}
	return cont;
}


// Funcion auxiliar para setear los campos de un hash.
// Pre: recibe la tabla, la posicion, y los datos a guardar (clave, valor, estado).
void setear_campo_hash(campo_hash_t* tabla, size_t pos, char* clave, void* valor, estado_t estado) {
	tabla[pos].clave = clave;
	tabla[pos].valor = valor;
	tabla[pos].estado = estado;
}


// Nueva funcion para evitar repetir codigo en obtener_pos_clave
// Pre: recibe la tabla, la clave a buscar, el inicio y el fin de la busqueda, y una indicacion 'encontrar_vacio' si se necesita
// Post: devuelve un numero donde debe estar la clave, una posicion para guardar la clave (VACIO O BORRADO) o -1 en caso de que no se obtenga nada
int _obtener_pos_clave(campo_hash_t* tabla, const char* clave, bool encontrar_vacio, int inicio, int fin) {
	int pos_borrado = -1;
	for (int i = inicio; i < fin; i++) {
		if (tabla[i].estado == OCUPADO && strcmp(tabla[i].clave, clave) == 0) {
			return i;
		}
		else if (tabla[i].estado == BORRADO && pos_borrado == -1) pos_borrado = i;
		else if (tabla[i].estado == VACIO) {
			if(encontrar_vacio) {
				if(pos_borrado == -1) return i;
				return pos_borrado;
			}
			return -1;
		}
	}
	return -1;
}

// Obtiene la posicion real de la clave en el hash en ese momento.
// Pre: recibe el hash y la clave a buscar.
// Post: devuelve la posicion de la clave.
// NOTA: Se agrego el valor 'encontrar_vacio' para reutilizarlo en hash_guardar
// NOTA 2: Se agrego la funcionalidad de _obtener_pos_clave(...) para evitar al repeticion de codigo
int obtener_pos_clave(campo_hash_t* tabla, const char* clave, bool encontrar_vacio, size_t tam) {
	int pos = (int)(hashing(clave) % tam);
	int pos_obtenida = _obtener_pos_clave(tabla, clave, encontrar_vacio, pos, (int)tam);
	if(pos_obtenida != -1) return pos_obtenida;
	return _obtener_pos_clave(tabla, clave, encontrar_vacio, 0, pos);
}

// Agranda o achica el hash segun el tam_nuevo pasado por parametro.
// Pre: recibe el hash y el nuevo tamaño del mismo.
// Post: devuelve true si lo pudo redimensionar, o false si no.
// Si el nuevo tam es menor al inicial, se redimensiona el hash a TAM_INICIAL
bool hash_redimensionar(hash_t* hash, size_t tam_nuevo) {
	if (tam_nuevo < TAM_INICIAL) tam_nuevo = TAM_INICIAL;
	campo_hash_t* tabla_nueva = calloc(tam_nuevo, sizeof(campo_hash_t));
	if (!tabla_nueva) return false;
	size_t cant_nuevo = 0;
	for (int i = 0; i < hash->tam; i++) {
		if (hash->tabla[i].estado != OCUPADO) {
			continue;
		}
		int pos_nueva_elem = obtener_pos_clave(tabla_nueva, hash->tabla[i].clave, true, tam_nuevo);
		setear_campo_hash(tabla_nueva, pos_nueva_elem, hash->tabla[i].clave, hash->tabla[i].valor, OCUPADO);
		cant_nuevo++;
	}
	free(hash->tabla);
	hash->tabla = tabla_nueva;
	hash->cant = cant_nuevo;
	hash->tam = tam_nuevo;
	hash->borrados = 0;
	return true;
}

/* ******************************************************************
 *                 PRIMITIVAS DEL ITERADOR DEL HASH
 * *****************************************************************/

hash_iter_t* hash_iter_crear(const hash_t* hash) {
	if(!hash) return NULL;
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if(!iter) return NULL;
	iter->hash = hash;
	iter->actual = hallar_nuevo_ocupado(0, hash);
	return iter;
}


bool hash_iter_avanzar(hash_iter_t* iter) {
	if(hash_iter_al_final(iter)) return false;
	iter->actual = hallar_nuevo_ocupado(iter->actual + 1, iter->hash);
	return true;
}


const char* hash_iter_ver_actual(const hash_iter_t* iter) {
	if(hash_iter_al_final(iter)) return NULL;
	return iter->hash->tabla[iter->actual].clave;
}


bool hash_iter_al_final(const hash_iter_t* iter) {
	return iter->actual >= iter->hash->tam;
}


void hash_iter_destruir(hash_iter_t* iter) {
	free(iter);
}


/* ******************************************************************
 *                         PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t* hash_crear(hash_destruir_dato_t destruir_dato) {
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;
	hash->tabla = calloc(TAM_INICIAL, sizeof(campo_hash_t));
	if (!hash->tabla) {
		free(hash);
		return NULL;
	}
	hash->tam = TAM_INICIAL;
	hash->cant = 0;
	hash->borrados = 0;
	hash->destruir_dato = destruir_dato;
	return hash;
}


bool hash_guardar(hash_t* hash, const char* clave, void* dato) {
	if ((float)(hash->cant + hash->borrados) / (float)hash->tam >= FACTOR_CARGA_TECHO) {
		bool redimension = hash_redimensionar(hash, hash->tam * FACTOR_REDIMENSION);
		if (!redimension) return false;
	}
	int pos = obtener_pos_clave(hash->tabla, clave, true, hash->tam);
	if(pos == -1) return false;
	if(hash->tabla[pos].estado == OCUPADO) {
		if (hash->destruir_dato) hash->destruir_dato(hash->tabla[pos].valor);
		hash->tabla[pos].valor = dato;
		return true;		
	}
	char* clave_copia = strdup(clave);
	if (!clave_copia) return false;
	setear_campo_hash(hash->tabla, (size_t)pos, clave_copia, dato, OCUPADO);
	hash->cant++;
	return true;
}


void* hash_borrar(hash_t* hash, const char* clave) {
	if(hash->cant == 0) return NULL;
	int pos_clave = obtener_pos_clave(hash->tabla, clave, false, hash->tam);
	if (pos_clave == -1) return NULL; // No se encontró la clave
	void* valor_devolver = hash->tabla[pos_clave].valor;
	free(hash->tabla[pos_clave].clave);
	hash->tabla[pos_clave].estado = BORRADO;
	hash->cant--;
	hash->borrados++;
	if ((float)(hash->cant + hash->borrados) / (float)hash->tam <= FACTOR_CARGA_PISO) {
		hash_redimensionar(hash, hash->tam / FACTOR_REDIMENSION);
	}
	return valor_devolver;
}


void* hash_obtener(const hash_t* hash, const char* clave) {
	int pos_clave = obtener_pos_clave(hash->tabla, clave, false, hash->tam);
	if (pos_clave == -1) return NULL; // No se encontró la clave
	return hash->tabla[pos_clave].valor;
}


bool hash_pertenece(const hash_t* hash, const char* clave) {
	return obtener_pos_clave(hash->tabla, clave, false, hash->tam) != -1;
}


size_t hash_cantidad(const hash_t* hash) {
	return hash->cant;
}


void hash_destruir(hash_t* hash) {
	for (size_t i = 0; i < hash->tam; i++) {
		if (hash->tabla[i].estado != OCUPADO) {
			continue;
		}
		if (hash->destruir_dato) hash->destruir_dato(hash->tabla[i].valor);
		free(hash->tabla[i].clave);
	}
	free(hash->tabla);
	free(hash);
}
