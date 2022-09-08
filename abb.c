#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include "pila.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct nodo_arbol {
	char* clave;
	void* dato;
	struct nodo_arbol* nodo_arbol_izq;
	struct nodo_arbol* nodo_arbol_der;
} nodo_arbol_t;


struct abb {
	nodo_arbol_t* raiz;
	abb_destruir_dato_t destruir_dato;
	abb_comparar_clave_t cmp;
	size_t cantidad;
};


struct abb_iter {
	pila_t* pila_nodos;
};


/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

// Funcion auxiliar para crear un nodo del arbol
// Post: devuelve un nodo vacio.
static nodo_arbol_t* nodo_arbol_crear(const char* clave, void* dato) {
	nodo_arbol_t* nodo_arbol = calloc(1, sizeof(nodo_arbol_t));
	if (!nodo_arbol) return NULL;
	char* clave_copia = strdup(clave);
	if (!clave_copia) {
		free(nodo_arbol);
		return NULL;
	}
	nodo_arbol->clave = clave_copia;
	nodo_arbol->dato = dato;
	return nodo_arbol;
}

// Wrapper de primitiva recursiva para el iterador interno.
// Itera mientras el puntero booleano 'seguir_visitando' sea true.
// Deja de iterar si 'seguir_visitando' es true pero la funcion visitar es false.
// Visitar recibe la clave, el valor y un puntero extra (que sirve como factor de corte).
// Pre: recibe el arbol, la funcion visitar, un puntero extra, y un puntero bool seguir_visitando.
static void _abb_in_order(nodo_arbol_t* nodo, bool visitar(const char*, void*, void*), void* extra, bool* seguir_visitando) {
	if(!nodo || !*seguir_visitando) return;
	_abb_in_order(nodo->nodo_arbol_izq, visitar, extra, seguir_visitando);
	if(*seguir_visitando && !visitar(nodo->clave, nodo->dato, extra)) {
		*seguir_visitando = false;
		return;
	}
	_abb_in_order(nodo->nodo_arbol_der, visitar, extra, seguir_visitando);
}

// Wrapper de primitiva recursiva para destruir el arbol.
void _abb_destruir(nodo_arbol_t* nodo, abb_destruir_dato_t destruir_dato) {
	if(!nodo) return;
	_abb_destruir(nodo->nodo_arbol_izq, destruir_dato);
	_abb_destruir(nodo->nodo_arbol_der, destruir_dato);
	if(destruir_dato) destruir_dato(nodo->dato);
	free(nodo->clave);
	free(nodo);
}

// Funcion auxiliar para apilar todos los nodos izquierdos de mayor a menor a la pila del iterador.
// Pre: recibe inicialmente el nodo raiz y la pila de nodos.
void apilar_nodos(nodo_arbol_t* nodo, pila_t* pila_nodos) {
	if(!nodo) return;
	pila_apilar(pila_nodos, nodo);
	apilar_nodos(nodo->nodo_arbol_izq, pila_nodos);
	return;
}

// Funcion auxiliar para determinar si un nodo es hoja o no.
// Pre: recibe un nodo.
// Post: devuelve true si no tiene hijos, false si sí.
bool es_hoja(const nodo_arbol_t* nodo) {
	if (!nodo) {
		return false;
	}
	return !nodo->nodo_arbol_izq && !nodo->nodo_arbol_der;
}

// Funcion auxiliar que devuelve la cantidad de hijos de un nodo.
// Pre: recibe un nodo.
// Post: devuelve la cantidad de hijos que tiene el nodo.
int cantidad_hijos(const nodo_arbol_t* nodo) {
	if (es_hoja(nodo)) return 0;
	else if (nodo->nodo_arbol_izq && nodo->nodo_arbol_der) {
		return 2;
	}
	return 1;
}


// Funcion auxiliar para poder obtener al padre y al hijo en una busqueda 
void buscar_nodo_y_nodo_padre(const abb_t* arbol, const char* clave, nodo_arbol_t** nodo, nodo_arbol_t** nodo_padre) {
	if(!arbol) return;
	int comparacion;
	while(*nodo != NULL) {
		comparacion = arbol->cmp(clave, (*nodo)->clave);
		if(comparacion == 0) {
			break;
		}
		*nodo_padre = *nodo;
		if(comparacion > 0) {
			*nodo = (*nodo)->nodo_arbol_der;
		}
		if(comparacion < 0) {
			*nodo = (*nodo)->nodo_arbol_izq;
		}
	}
}


// Funcion auxiliar para evitar repetir codigo al borrar con un solo hijo.
// Pre: recibe el arbol, el nodo obtenido, el padre del nodo, y el reemplazante para el nodo a borrar.
void operacion_borrar_aux(abb_t* arbol, nodo_arbol_t* nodo_obtenido, nodo_arbol_t* padre, nodo_arbol_t* reemplazo) {
	if(arbol->raiz == nodo_obtenido){
		arbol->raiz = reemplazo;
		return;
	}
	if(padre->nodo_arbol_der == nodo_obtenido) {
		padre->nodo_arbol_der = reemplazo;
	}
	else {
		padre->nodo_arbol_izq = reemplazo;
	}
}


void iterar_iter_interno_rango(nodo_arbol_t* nodo, char* inicio, char* fin, bool visitar(const char*, void*, void*), void* extra, abb_comparar_clave_t cmp) {
	if (!nodo) return;
	if (cmp(nodo->clave, inicio) > 0) {
		iterar_iter_interno_rango(nodo->nodo_arbol_izq, inicio, fin, visitar, extra, cmp);
	}
	if (cmp(nodo->clave, inicio) >= 0 && cmp(nodo->clave, fin) <= 0) {
		visitar(nodo->clave, nodo->dato, extra);
	}
	if (cmp(nodo->clave, fin) < 0) {
		iterar_iter_interno_rango(nodo->nodo_arbol_der, inicio, fin, visitar, extra, cmp);
	}
}


void abb_in_order_por_rango(abb_t* arbol, char* inicio, char* fin, bool visitar(const char*, void*, void*), void* extra) {
	iterar_iter_interno_rango(arbol->raiz, inicio, fin, visitar, extra, arbol->cmp);
}


/* ******************************************************************
 *                  PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

void abb_in_order(abb_t* arbol, bool visitar(const char*, void*, void*), void* extra) {
	if(!arbol || !arbol->raiz) return;
	bool seguir_visitando = true;
	_abb_in_order(arbol->raiz, visitar, extra, &seguir_visitando);	
}


/* ******************************************************************
 *                  PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

abb_iter_t* abb_iter_in_crear(const abb_t* arbol) {
	if(!arbol) return NULL;
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if(!iter) return NULL;
	iter->pila_nodos = pila_crear();
	if(!iter->pila_nodos) {
		free(iter);
		return NULL;
	}
	apilar_nodos(arbol->raiz, iter->pila_nodos);
	return iter;
}


bool abb_iter_in_avanzar(abb_iter_t* iter) {
	if(abb_iter_in_al_final(iter)) return false;
	nodo_arbol_t* nodo_desapilar = (nodo_arbol_t*)pila_desapilar(iter->pila_nodos);
	apilar_nodos(nodo_desapilar->nodo_arbol_der, iter->pila_nodos);
	return true;
}


const char* abb_iter_in_ver_actual(const abb_iter_t* iter) {
	if(abb_iter_in_al_final(iter)) return NULL;
	nodo_arbol_t* nodo = pila_ver_tope(iter->pila_nodos);
	return nodo->clave;
}


bool abb_iter_in_al_final(const abb_iter_t* iter) {
	return pila_esta_vacia(iter->pila_nodos);
}


void abb_iter_in_destruir(abb_iter_t* iter) {
	if(!iter) return;
	pila_destruir(iter->pila_nodos);
	free(iter);
}


/* ******************************************************************
 *                          PRIMITIVAS DEL ABB
 * *****************************************************************/

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
	abb_t* arbol = calloc(1, sizeof(abb_t));
	if (!arbol) return NULL;
	arbol->destruir_dato = destruir_dato;
	arbol->cmp = cmp;
	arbol->cantidad = 0;
	return arbol;
}


bool abb_guardar(abb_t* arbol, const char* clave, void* dato) {
	if(!arbol) return false;
	nodo_arbol_t* padre = NULL;
	nodo_arbol_t* nodo_obtenido = arbol->raiz;
	buscar_nodo_y_nodo_padre(arbol, clave, &nodo_obtenido, &padre);
	if(nodo_obtenido) {
		if(arbol->destruir_dato) {
			arbol->destruir_dato(nodo_obtenido->dato);
		}
		nodo_obtenido->dato = dato;
		return true;
	}

	nodo_arbol_t* nodo_nuevo = nodo_arbol_crear(clave, dato);
	if(!nodo_nuevo) return false;
 
	if(!padre) {
		arbol->raiz = nodo_nuevo;
	}
	else {
		int comparacion = arbol->cmp(clave, padre->clave);
		if (comparacion > 0) {
			padre->nodo_arbol_der = nodo_nuevo;
		}
		else if (comparacion < 0) {
			padre->nodo_arbol_izq = nodo_nuevo;
		}
	}
	arbol->cantidad++;
	return true;
}


void* abb_borrar(abb_t* arbol, const char* clave) {
	if(!arbol) return NULL;
	nodo_arbol_t* padre = NULL;
	nodo_arbol_t* nodo_obtenido = arbol->raiz;
	buscar_nodo_y_nodo_padre(arbol, clave, &nodo_obtenido, &padre);
	if(!nodo_obtenido) return NULL;
	void* valor_devolver = nodo_obtenido->dato;
	int cant_hijos = cantidad_hijos(nodo_obtenido);

	if(cant_hijos == 0) {
		operacion_borrar_aux(arbol, nodo_obtenido, padre, NULL);
	}
	else if(cant_hijos == 1) {
		if(nodo_obtenido->nodo_arbol_izq) {
			operacion_borrar_aux(arbol, nodo_obtenido, padre, nodo_obtenido->nodo_arbol_izq);
		}
		else {
			operacion_borrar_aux(arbol, nodo_obtenido, padre, nodo_obtenido->nodo_arbol_der);
		}
	}
	else { // Tiene 2 hijos
		nodo_arbol_t* padre_reemplazante = nodo_obtenido;
		nodo_arbol_t* reemplazante = nodo_obtenido->nodo_arbol_der;
		while(reemplazante->nodo_arbol_izq) {
			padre_reemplazante = reemplazante;
			reemplazante = reemplazante->nodo_arbol_izq;
		}

		operacion_borrar_aux(arbol, nodo_obtenido, padre, reemplazante);
		if(padre_reemplazante == nodo_obtenido) {
			padre_reemplazante->nodo_arbol_der = reemplazante->nodo_arbol_der;
		}
		else {
			padre_reemplazante->nodo_arbol_izq = reemplazante->nodo_arbol_der;
		}
		reemplazante->nodo_arbol_der = nodo_obtenido->nodo_arbol_der;
		reemplazante->nodo_arbol_izq = nodo_obtenido->nodo_arbol_izq;
	}
	free(nodo_obtenido->clave);
	free(nodo_obtenido);
	arbol->cantidad--;
	return valor_devolver;
}


void* abb_obtener(const abb_t* arbol, const char* clave) {
	if (abb_cantidad((abb_t*)arbol) == 0) return NULL;
	nodo_arbol_t* padre = NULL;
	nodo_arbol_t* nodo = arbol->raiz;
	buscar_nodo_y_nodo_padre(arbol, clave, &nodo, &padre);
	if(!nodo) return NULL;
	return nodo->dato;
}


bool abb_pertenece(const abb_t* arbol, const char* clave) {
	if (abb_cantidad((abb_t*)arbol) == 0) return false;
	nodo_arbol_t* padre = NULL;
	nodo_arbol_t* nodo = arbol->raiz;
	buscar_nodo_y_nodo_padre(arbol, clave, &nodo, &padre);
	return nodo != NULL;
}


size_t abb_cantidad(abb_t* arbol) {
	return arbol->cantidad;
}


void abb_destruir(abb_t* arbol) {
	if(!arbol) return;
	_abb_destruir(arbol->raiz, arbol->destruir_dato);
	free(arbol);
}
