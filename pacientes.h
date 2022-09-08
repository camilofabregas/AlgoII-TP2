#ifndef _PACIENTES_
#define _PACIENTES_

#include "hash.h"

// Defino el tipo de dato de los pacientes.
typedef struct paciente paciente_t;

// Pre: recibe el paciente
// Post: devuelve el nombre del paciente
char* obtener_nombre_paciente(paciente_t* paciente);

// Pre: recibe el paciente
// Post: devuelve el anio de inscripcion del paciente
int obtener_anio_paciente(paciente_t* paciente);

// Crea la estructura del paciente.
// Pre: recibe el nombre del paciente y su año de inscripcion.
// Post: devuelve la estructura del paciente.
paciente_t* paciente_crear(char* nombre_paciente, int anio_inscripcion);

// Pre: recibe la estructura del paciente
// Post: libera la memoria utilizada
void destruir_paciente (paciente_t* paciente);

// Parsea los datos del paciente que serán insertados en la lista, con csv_crear_estructura.
// Pre: recibe los parametros a parsear, y un puntero extra.
// Post: devuelve el paciente creado (paciente_t*) o NULL si hubo un error.
void* parse_paciente(char** params, void* extra);


// Carga los pacientes leidos a un listado, y ese listado se pasa a un Hash.
// Pre: recibe el hash de pacientes vacío, y la ruta al archivo CSV.
// Post: devuelve true si la carga de datos fue satisfactoria, false si no.
bool cargar_pacientes(char* ruta_csv, hash_t* pacientes);


#endif