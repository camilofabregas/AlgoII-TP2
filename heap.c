#include <stdlib.h>
#include "heap.h"
#define TAM_INICIAL 1
#define FACTOR_REDIMENSION 2

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

struct heap {
	void** datos;
	size_t cant;
	size_t tam;
	cmp_func_t cmp;
};


/* ******************************************************************
 *                        FUNCIONES AUXILIARES
 * *****************************************************************/

// Funcion para obtener hijo izquierdo
// Pre: recibe la posicion del elemento
// Post: devuelve la posicion del hijo izquierdo del elemento
size_t obtener_hijo_izq(size_t pos) {
	return pos * 2 + 1;
}

// Funcion para obtener hijo derecho
// Pre: recibe la posicion del elemento
// Post: devuelve la posicion del hijo derecho del elemento
size_t obtener_hijo_der(size_t pos) {
	return pos * 2 + 2;
}

// Funcion para obtener padre del elemento en la posicion
// Pre: recibe la posicion del elemento
// Post: Devuelve la posicion del padre del elemento
size_t obtener_padre(size_t pos) {
	return (pos - 1) / 2;
}

// Funcion para swappear (intercambiar) dos posiciones del arreglo.
// Pre: recibe el arreglo, y dos posiciones del mismo.
// Post: Los elementos en las posiciones del arreglo fueron intercambiadas
void swap(void** arreglo, size_t pos1, size_t pos2) {
	void* aux = arreglo[pos1];
	arreglo[pos1] = arreglo[pos2];
	arreglo[pos2] = aux;
}

// Funcion para evaluar si un elemento debe ser swappeado o no, para aplicar el metodo upheap.
// Pre: recibe el arreglo, la posicion del hijo, y la funcion de comparacion.
// Post: si el elemento tiene mayor valor que el padre los intercambia y se llama recursivamente,
//  sino finaliza su ejecucion
void upheap(void** arreglo, size_t pos_hijo, cmp_func_t cmp) {
	if(!pos_hijo) return;
	size_t pos_padre = obtener_padre(pos_hijo);
	if(cmp(arreglo[pos_padre], arreglo[pos_hijo]) < 0) {
		swap(arreglo, pos_padre, pos_hijo);
		upheap(arreglo, pos_padre, cmp);
	}
}

// Funcion para encontrar el maximo entre 3 valores.
// Pre: recibe el arreglo, la posicion actual, la posicion de su hijo izq y der, la cantidad de valores y la funcion de comparacion.
// Post: devuelve el maximo de los tres valores.
size_t encontrar_max(void** arreglo, size_t pos, size_t izq, size_t der, size_t cant, cmp_func_t cmp) {
	size_t max = pos;
	if((izq < cant) && (cmp(arreglo[izq], arreglo[max]) > 0)) {
		max = izq;
	}
	if((der < cant) && (cmp(arreglo[der], arreglo[max]) > 0)) {
		max = der;
	}
	return max;
}

// Funcion para evaluar si un elemento debe ser swappeado o no, para aplicar el metodo downheap.
// Pre: recibe el arreglo, la posicion actual, la cantidad de elementos y la funcion de comparacion.
// Post: se compara el elemento actual con sus dos hijos. Si el elemento es el mayor entre los 3 se termina 
//  la ejecucion sino se intercambia el elemento con el maximo de sus hijos y se llama recursivamente
void downheap(void** arreglo, size_t pos, size_t cant, cmp_func_t cmp) {
	if(pos >= cant) return;
	size_t pos_hijo_izq = obtener_hijo_izq(pos);
	size_t pos_hijo_der = obtener_hijo_der(pos);
	size_t maximo = encontrar_max(arreglo, pos, pos_hijo_izq, pos_hijo_der, cant, cmp);
	if(maximo != pos) {
		swap(arreglo, pos, maximo);
		downheap(arreglo, maximo, cant, cmp);
	}
}

// Funcion para convertir un arreglo generico en un heap. 
// Pre: recibe el arreglo, la longitud y la funcion de comparacion.
void heapify(void** arreglo, size_t n, cmp_func_t cmp) {
	for(int i = (int)n / 2; i >= 0; i--) {
		downheap(arreglo, (size_t)i, n, cmp);
	}
}

// Funcion para redimensionar el tamaño del heap
// Devuelve true si la redimension fue exitosa, false si no
bool redimensionar_heap(heap_t* heap, size_t nuevo_tam) {
	if (nuevo_tam < TAM_INICIAL) nuevo_tam = TAM_INICIAL;
	void** aux = realloc(heap->datos, nuevo_tam * sizeof(void*));
	if (!aux) return false;

	heap->datos = aux;
	heap->tam = nuevo_tam;
	return true;
}

/* ******************************************************************
 *                         PRIMITIVAS DEL HEAP
 * *****************************************************************/

heap_t* heap_crear(cmp_func_t cmp) {
	heap_t* heap = calloc(1, sizeof(heap_t));
	if (!heap) return NULL;
	heap->datos = malloc(TAM_INICIAL * sizeof(void*));
	if (!heap->datos) {
		free(heap);
		return NULL;
	}
	heap->tam = TAM_INICIAL;
	heap->cant = 0;
	heap->cmp = cmp;
	return heap;
}

heap_t* heap_crear_arr(void* arreglo[], size_t n, cmp_func_t cmp) {
	heap_t* heap = malloc(sizeof(heap_t));
	if(!heap) return NULL;
	void** arr_heap = malloc(sizeof(void*) * n);
	if(!arr_heap) {
		free(heap);
		return NULL;
	}
	for(size_t i = 0; i < n; i++) {
		arr_heap[i] = arreglo[i];
	}
	heapify(arr_heap, n, cmp);
	heap->cmp = cmp;
	heap->datos = arr_heap;
	heap->tam = n;
	heap->cant = n;
	return heap;
}

void heap_destruir(heap_t* heap, void (*destruir_elemento)(void* e)) {
	if (!heap) return;
	if (destruir_elemento) {
		for (size_t i = 0; i < heap->cant; i++) {
			destruir_elemento(heap->datos[i]);
		}
	}
	free(heap->datos);
	free(heap);
}

size_t heap_cantidad(const heap_t* heap) {
	if (!heap) return 0;
	return heap->cant;
}

bool heap_esta_vacio(const heap_t* heap) {
	if (!heap) return false;
	return heap->cant == 0;
}

bool heap_encolar(heap_t* heap, void* elem) {
	if (!heap) return false; 
	if(heap->tam == heap->cant) {
		if(!redimensionar_heap(heap, heap->tam * FACTOR_REDIMENSION )) return false;
	}
	heap->datos[heap->cant] = elem;
	upheap(heap->datos, heap->cant, heap->cmp);
	heap->cant++;
	return true;
}

void* heap_ver_max(const heap_t* heap) {
	if (!heap || heap_esta_vacio(heap)) return NULL;
	return heap->datos[0];
}

void* heap_desencolar(heap_t* heap) {
	if(!heap || heap_esta_vacio(heap)) return NULL;
	void* elemento = heap->datos[0];
	swap(heap->datos, 0, heap->cant - 1);
	heap->cant--;
	downheap(heap->datos, 0, heap->cant, heap->cmp);
	if(heap->cant == heap->tam / FACTOR_REDIMENSION && heap->tam / FACTOR_REDIMENSION > TAM_INICIAL) {
		redimensionar_heap(heap, heap->tam / FACTOR_REDIMENSION);
	}
	return elemento;
}

void heap_sort(void* elementos[], size_t cant, cmp_func_t cmp) {
	heapify(elementos, cant, cmp);
	for(size_t i = 0; i < cant; i++) {
		swap(elementos, 0, cant - i - 1);
		downheap(elementos, 0, cant - i - 1, cmp);
	}
}
