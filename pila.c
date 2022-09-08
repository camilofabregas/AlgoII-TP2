#include "pila.h"
#include <stdlib.h>
#define CAPACIDAD_INICIAL 10
#define COEF_AUMENTO 2
#define COEF_DISMINUCION 4

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

bool redimensionar_pila(pila_t* pila, size_t nueva_capacidad);
/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 *	       Mariano Fortunato Rossi - 104.420 
 * *****************************************************************/

pila_t* pila_crear(void) {
	pila_t* pila = malloc(sizeof(pila_t));
	if (!pila) return NULL;
	pila->datos = malloc(CAPACIDAD_INICIAL * sizeof(void*));
	if (!pila->datos) {
		free(pila);
		return NULL;
	}
	pila->capacidad = CAPACIDAD_INICIAL;
	pila->cantidad = 0;
	return pila;
}

void pila_destruir(pila_t *pila) {
	free(pila->datos);
	free(pila);
}

bool pila_esta_vacia(const pila_t *pila) {
	return (pila->cantidad == 0);
}

bool pila_apilar(pila_t *pila, void* valor) {
	if(pila->cantidad == pila->capacidad) {
		if(redimensionar_pila(pila, pila->capacidad * COEF_AUMENTO) != true) return false;
		//Si la redimension falla, el apilar tambien y por lo tanto sale sin apilar
	}
	pila->datos[pila->cantidad] = valor;
	pila->cantidad += 1;
	return true;
}

void* pila_ver_tope(const pila_t *pila) {
	if (pila->cantidad == 0) return NULL;
	return (pila->datos[pila->cantidad - 1]);
}

void* pila_desapilar(pila_t *pila) {
	if (pila->cantidad == 0) return NULL;
	if (pila->capacidad > CAPACIDAD_INICIAL && ((pila->capacidad / COEF_DISMINUCION) >= pila->cantidad )){
		if (redimensionar_pila(pila, pila->capacidad / COEF_DISMINUCION) != true) return NULL;
	}
	pila->cantidad -= 1;
	return pila->datos[pila->cantidad];
}

// FUNCION AUXILIAR: REDIMENSIONAR_PILA
//    SI PUEDE REDIMENSIONAR -> TRUE
//    NO PUEDE REDIMENSIONAR -> FALSE
bool redimensionar_pila(pila_t* pila, size_t nueva_capacidad) {
	void** aux = realloc(pila->datos, nueva_capacidad * sizeof(void*));
	if (!aux) {
		free(pila->datos);
		return false;
	}
	pila->datos = aux;
	pila->capacidad = nueva_capacidad;
	return true;
}
