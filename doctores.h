#ifndef _DOCTORES_Y_ESPECIALIDADES_
#define _DOCTORES_Y_ESPECIALIDADES_

#include "abb.h"
#include "hash.h"
#include "csv.h"
#include "lista.h"
#include "mensajes.h"
#include "pacientes.h"

typedef struct doctor doctor_t;

// Pre: recibe los datos de un doctor
// Post: devuelve el total de pacientes que atendio
size_t obtener_atendidos(doctor_t* doctor);

// Pre: recibe los datos de un doctor
// Post: modifica el total de pacientes que atendio

void doctor_atender(doctor_t* doctor);

// Pre: recibe los datos de un doctor
// Post: devuelve la especialidad del doctor
char* obtener_especialidad(doctor_t* doctor);

// Funcion para crear un doctor
// Pre: recibe un nombre y una especialidad
// Post: devuelve el doctor creado
doctor_t* crear_doctor(char* nombre, char* espec);

// Funcion para destruir el doctor y liberar la memorai utilizada
void destruir_doctor(doctor_t* doctor);

//void wrapper_dest_doctor(void* dato);

void* parse_doctor (char** params, void* extra);

// Funcion que toma un archivo, lo lee y crea las estructuras correspondientes.
// Pre: recibe la ruta al CSV, el arbol de doctores y el hash de especialidades.
// Post: devuelve true si la carga de datos fue satisfactoria, false si no.
bool cargar_doctores_y_especialidades (char* ruta_csv, abb_t* doctores, hash_t* especialidades);

#endif