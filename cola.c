#include "cola.h"
#include <stdlib.h>

typedef struct nodo{
	void* dato;
	struct nodo* siguiente;
}nodo_t;

struct cola{
	nodo_t* primero;
	nodo_t* ultimo;
	size_t cant;
};

nodo_t* nodo_crear(void* valor) {
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) return NULL;
	nodo->dato = valor;
	nodo->siguiente = NULL;
	return nodo;
}

cola_t* cola_crear(void) {
	cola_t* cola = malloc(sizeof(cola_t));
	if (!cola) return NULL;
	cola->primero = NULL;
	cola->ultimo = NULL;
	cola->cant = 0;
	return cola;
}

void cola_destruir(cola_t *cola, void (*destruir_dato)(void*)) {
	void* dato;
	while (!cola_esta_vacia(cola)){
		dato = cola_desencolar(cola);
		if (destruir_dato != NULL) destruir_dato(dato);
	}
	free(cola);
}

bool cola_esta_vacia(const cola_t *cola){
	return cola->cant == 0;
}

bool cola_encolar(cola_t *cola, void* valor){
	nodo_t* nodo = nodo_crear(valor);
	if (!cola->primero) cola->primero = nodo;
	else cola->ultimo->siguiente = nodo;
	cola->ultimo = nodo;
	cola->cant += 1;
	return true;
}

void* cola_ver_primero(const cola_t *cola){
	if ((!cola->primero) && (!cola->ultimo)) return NULL;
	return (cola->primero->dato);
}

void* cola_desencolar(cola_t *cola){
	if ((!cola->primero) && (!cola->ultimo)) return NULL;
	nodo_t* nodo_desencolar = cola->primero;
	void* dato = nodo_desencolar->dato;
	cola->primero = cola->primero->siguiente;
	if (cola->primero == NULL) cola->ultimo = NULL;
	free(nodo_desencolar);
	cola->cant -= 1; 
	return dato;
}

size_t cola_cantidad(const cola_t* cola) {
	return cola->cant;
}