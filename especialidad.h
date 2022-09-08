#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cola.h"
#include "heap.h"
#include "pacientes.h"

typedef struct especialidad especialidad_t;

// Funcion para crear una estructura de especialidad
especialidad_t* crear_especialidad();

// Pre: recibe la especialidad
// Post: devuelve los pacientes urgentes
cola_t* obtener_urgentes(especialidad_t* especialidad);

// Pre: recibe la especialidad
// Post: devuelve los pacientes regulares
heap_t* obtener_regulares(especialidad_t* especialidad);

// Aumenta el tamanio de la fila de espera de pacientes en una especialidad
void aumentar_tamanio_fila(especialidad_t* especialidad);

// Disminuye el tamanio de la fila de espera de pacientes de una especialidad
void disminuir_tamanio_fila(especialidad_t* especialidad);

// Pre: recibe la especialidad
// Post: devuelve el tamanio de la fila de espera de pacientes de una especialidad
size_t obtener_tamanio_fila(especialidad_t* especialidad);

// Funcion para liberar la memoria necesaria utilizada en una especialidad
// Pre: recibe la especialidad, el destructor de la cola y el destructor del heap
void especialidad_destruir(especialidad_t* especialidad, void (*destruir_dato_cola)(void*), void (*destruir_dato_heap)(void*));

// Pre: recibe la urgencia, el nombre del paciente, la estructura de pacientes y la especialidad
// Post: guarda el paciente en la estructura correspondiente segun la urgencia
void especialidad_encolar_paciente(char* urgencia, char* nombre_paciente, hash_t* pacientes, especialidad_t* especialidad);

// Pre: recibe la estructura de la especialidad
// Post: devuelve el proximo paciente a atender
paciente_t* especialidad_desencolar_paciente(especialidad_t* especialidad);