#ifndef ABB_H
#define ABB_H
#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DECLARACION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct abb abb_t;


typedef struct abb_iter abb_iter_t;


typedef int (*abb_comparar_clave_t) (const char*, const char*);


typedef void (*abb_destruir_dato_t) (void*);


/* ******************************************************************
 *                  PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

// Itera mientras la funcion 'visitar' devuelva true. Deja de iterar si 'visitar' devuelve false.
// Visitar recibe la clave, el valor y un puntero extra.
// Pre: recibe el arbol, la funcion visitar, y un puntero extra. 
void abb_in_order(abb_t* arbol, bool visitar(const char*, void*, void*), void* extra);

// Itera igual que abb_in_order pero desde un inicio hasta un fin (haciendo busqueda binaria para hallar el inicio).
// Pre: recibe el arbol, un inicio, un fin, la funcion visitar y un puntero extra.
void abb_in_order_por_rango(abb_t* arbol, char* inicio, char* fin, bool visitar(const char*, void*, void*), void* extra);


/* ******************************************************************
 *                  PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crea el iterador externo.
// Pre: recibe el arbol creado con abb_crear.
// Post: devuelve un puntero al iterador externo creado.
abb_iter_t* abb_iter_in_crear(const abb_t* arbol);


// Avanza al siguiente segun corresponda (hijo izq, padre o hijo der).
// Pre: recibe el iterador.
// Post: devuelve true si pudo avanzar, o false si no (esta al final)
bool abb_iter_in_avanzar(abb_iter_t* iter);


// Muestra la clave del nodo apuntado actualmente por el iterador.
// Pre: recibe el iterador.
// Post: devuelve un puntero a la clave del nodo actual.
const char* abb_iter_in_ver_actual(const abb_iter_t* iter);


// Indica si el iterador se encuentra apuntando al final del arbol.
// Pre: recibe el iterador.
// Post: devuelve true si esta al final del arbol, false si no.
// El iterador esta al final si apunta a NULL (antes apuntaba a una hoja).
bool abb_iter_in_al_final(const abb_iter_t* iter);


// Destruye el iterador creado.
// Pre: recibe el iterador.
void abb_iter_in_destruir(abb_iter_t* iter);


/* ******************************************************************
 *                          PRIMITIVAS DEL ABB
 * *****************************************************************/

// Crea un arbol.
// Pre: recibe la funcion cmp, y la funcion destruir_dato.
// Post: devuelve un arbol vacio.
// cmp compara dos claves y devuelve: 1 si clave1 > clave2, -1 si clave1 < clave2, o 0 si clave1 = clave2.
// destruir_dato destruye el dato, o no hace nada si es NULL.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);


// Guarda la clave y el dato asociado donde corresponde, segun la clave.
// Pre: recibe el arbol, la clave y el dato.
// Post: devuelve true si se pudo guardar, false si no.
bool abb_guardar(abb_t* arbol, const char* clave, void* dato);


// Borra la clave y el dato asociado.
// Pre: recibe el arbol y la clave.
// Post: devuelve el dato que fue eliminado del arbol.
void* abb_borrar(abb_t* arbol, const char* clave);


// Muestra el dato que esta asociado a la clave recibida.
// Pre: recibe el arbol y la clave.
// Post: devuelve el dato asociado a la clave.
void* abb_obtener(const abb_t* arbol, const char* clave);


// Indica si la clave recibida esta en el arbol o no.
// Pre: recibe el arbol y la clave.
// Post: devuelve true si esta en el arbol, false si no.
bool abb_pertenece(const abb_t* arbol, const char* clave);


// Indica la cantidad de nodos que tiene el arbol.
// Pre: recibe el arbol.
// Post: devuelve la cantidad de nodos que tiene el arbol.
size_t abb_cantidad(abb_t* arbol);

// Destruye el arbol, liberando toda la memoria.
// Pre: recibe el arbol.
void abb_destruir(abb_t* arbol);


#endif // HASH_H