#ifndef _LISTA_H
#define _LISTA_H
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

// Estructuras de la lista

typedef struct lista lista_t;

struct lista;
//typedef struct nodo nodo_t;

// Estructuras de los iteradores

typedef struct lista_iter lista_iter_t;

// Primitivas de la lista

// Crea una lista y reserva la memoria necesaria para ella
// Post: devolvio la lista creada, en caso de fallar devuelve NULL
lista_t *lista_crear(void);

// Comprueba si la lista esta vacia
// Pre: la lista fue creada
// Post: devolvio TRUE si esta vacia, FALSE en caso contrario
bool lista_esta_vacia(const lista_t *lista);

// Inserta un elemento en la primer posicion
// Pre: la lista fue creada
// Post: devolvio TRUE si se pudo insertar, FALSE si no pudo
bool lista_insertar_primero(lista_t *lista, void *dato);

// Inserta un elemento en la ultima posicion de la lista
// Pre: la lista fue creada
// Post: devolvio TRUE si se pudo insertar, FALSE en caso de fallar
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Obtiene el valor de la ultima posicion, NULL si esta vacia,
// y lo borra de la lista
// Pre: la lista esta creada
// Post: devolvio el dato de la ultima posicion, NULL si esta vacia,
// y borro el dato si no estaba vacia
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor de la primer posicion, NULL si esta vacia
// Pre: la lista esta creada
// Post: devolvio el dato de la primer posicion, NULL si esta vacia
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor de la ultima posicion, NULL si esta vacia
// Pre: la lista esta creada
// Post: devolvio el dato de la ultima posicion, NULL si esta vacia
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista
// Pre: la lista fue creada
// Post: devolvio el largo de la lista
size_t lista_largo(const lista_t *lista);

// Destruye la lista
// Pre: la lista fue creada.
// Post: se eliminaron todos los elementos de la lista 
// y se destruyo la misma
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));

// Iterador interno 

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

// Iterador externo

// Devuelve el iterador creado con la memoria reservada,
// o NULL en caso de fallar
// Pre: la lista fue creada
// Post: devolvio el iterador creado, NULL si fallo
lista_iter_t *lista_iter_crear(lista_t *lista);

// Devuelve si puede avanzar o no en la lista
// Pre: el iterador fue creado
// Post: devolvio TRUE si el siguiente es distinto de NULL, 
// FALSE en caso de que no pueda iterar mas
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el elemento actual de la iteracion
// Pre: el iterador fue creado
// Post: devolvio el elemento de la posicion actual del iterador
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve si el iterador esta al final
// Pre: el iterador fue creado
// Post: devolvio TRUE si el actual es el ultimo elemento, sino FALSE
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador devolviendo la memoria necesaria
// Pre: el iterador fue creado
// Post: el iterador fue borrado sin eliminar la lista
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un elemento entre la posicion anterior y actual 
// del iterador
// Pre: el iterador fue creado
// Post: inserto un elemento a la lista en la posicion ubicada
// entre el anterior y el actual del iterador
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Devuelve el elemento del iterador que estaba en la posicion actual
// Pre: el iterador fue creado
// Post: devolvio el elemento ubicado en la posicion actual del iterador
void *lista_iter_borrar(lista_iter_t *iter);

#endif // _LISTA.H