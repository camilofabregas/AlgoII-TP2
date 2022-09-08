#include "lista.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#define LARGO_CERO 0

typedef struct nodo {
	void* dato;
	struct nodo* siguiente;
}nodo_t;

struct lista {
	nodo_t* primero;
	nodo_t* ultimo;
	size_t  largo;
};

struct lista_iter {
	lista_t* lista;
	nodo_t* anterior;
	nodo_t* actual;
};

nodo_t* nodo_crear_l(void* dato, nodo_t* siguiente) {
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) return NULL;
	nodo->dato = dato;
	nodo->siguiente = siguiente;
	return nodo;
}

lista_t *lista_crear(void) {
	lista_t* lista = malloc(sizeof(lista_t));
	if (!lista) return NULL;
	lista->primero = NULL;
	lista->ultimo  = NULL;
	lista->largo = LARGO_CERO;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista) {
	return (lista->largo == LARGO_CERO);
}

bool lista_insertar_primero(lista_t *lista, void *dato) {
	nodo_t* nodo = nodo_crear_l(dato, lista->primero);
	lista->primero = nodo;
	if (lista->largo == LARGO_CERO) {
		lista->ultimo = nodo;
	}
	lista->largo += 1;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato) {
	nodo_t* nodo = nodo_crear_l(dato, NULL);
	if (lista->largo == LARGO_CERO) {
		lista->primero = nodo;
	}
	else {
		lista->ultimo->siguiente = nodo;
	}
	lista->ultimo = nodo;
	lista->largo += 1;
	return true;
}

void *lista_borrar_primero(lista_t *lista) {
	if (lista->largo == LARGO_CERO) return NULL;
	nodo_t* nodo_borrar = lista->primero;
	void* dato = nodo_borrar->dato;
	lista->primero = nodo_borrar->siguiente;
	if (lista->largo == 1) lista->ultimo = NULL;
	lista->largo -= 1;
	free(nodo_borrar);
	return dato;
}

void *lista_ver_primero(const lista_t *lista) {
	if (lista->largo == LARGO_CERO) return NULL;
	return lista->primero->dato;
}

void *lista_ver_ultimo(const lista_t* lista) {
	if (lista->largo == LARGO_CERO) return NULL;
	return lista->ultimo->dato;
}

size_t lista_largo(const lista_t *lista) {
	return (lista->largo);
}

void lista_destruir(lista_t *lista, void (*destruir_dato)(void *)) {
	void* dato;
	while (!lista_esta_vacia(lista)) {
		dato = lista_borrar_primero(lista);
		if (destruir_dato != NULL) destruir_dato(dato);
	}
	free(lista);
}

// Iterador interno

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra) {
	nodo_t* nodo_actual = lista->primero;
	if (!nodo_actual) return;
	while (visitar(nodo_actual->dato, extra)){
		nodo_actual = nodo_actual->siguiente;
		if (!nodo_actual) break;
	}
	return;
}

// Iterador externo

lista_iter_t *lista_iter_crear(lista_t *lista) {
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (!iter) return NULL;
	iter->lista = lista;
	iter->anterior = NULL;
	iter->actual = lista->primero;
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter) {
	if (iter->actual == NULL) return false;
	iter->anterior = iter->actual;
	iter->actual = iter->actual->siguiente;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter) {
	if(!iter->actual) return NULL;
	return (iter->actual->dato);
}

bool lista_iter_al_final(const lista_iter_t *iter) {
	return (iter->actual == NULL);
}

void lista_iter_destruir(lista_iter_t *iter) {
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato) {
	nodo_t* nodo = nodo_crear_l(dato, iter->actual);
	if (!iter->anterior) iter->lista->primero = nodo;
	else iter->anterior->siguiente = nodo;
	if (!iter->actual) iter->lista->ultimo = nodo;
	iter->actual = nodo;
	iter->lista->largo += 1;
	return true;
}

void *lista_iter_borrar(lista_iter_t *iter) {
	nodo_t* nodo_borrar = iter->actual;
	if (!nodo_borrar) return NULL;
	void* dato = nodo_borrar->dato;
	iter->actual = iter->actual->siguiente;
	if(!iter->anterior) iter->lista->primero = iter->actual;
	else iter->anterior->siguiente = iter->actual;
	if (!iter->actual)iter->lista->ultimo = iter->anterior;
	iter->lista->largo -= 1;
	free(nodo_borrar);
	return dato;
}