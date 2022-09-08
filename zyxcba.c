#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "heap.h"
#include "abb.h"
#include "cola.h"
#include "hash.h"
#include "strutil.h"
#include "mensajes.h"
#include "doctores.h"
#include "pacientes.h"
#include "especialidad.h"

#define COMANDO_PEDIR_TURNO "PEDIR_TURNO"
#define COMANDO_ATENDER "ATENDER_SIGUIENTE"
#define COMANDO_INFORME "INFORME"


/* ******************************************************************
 *                      FUNCIONES SECUNDARIAS
 * *****************************************************************/

// Destruye el paciente de la cola de regulares.
// Pre: recibe el paciente.
void destruir_paciente_wrapper(void* paciente) {
	destruir_paciente(paciente);
}


// Destruye la especialidad.
// Pre: recibe la especialidad.
void destruir_especialidades(void* especialidad) {
	especialidad_destruir(especialidad, NULL, NULL);
}


// Destruye el dato del doctor.
// Pre: recibe el dato del doctor.
void destruir_doctor_aux(void* doctor) {
	destruir_doctor(doctor);
}


// Chequea si el parametro ingresado es vacio o no.
// Pre: recibe el parametro.
// Post: devuelve true si lo es, false si no.
bool es_vacio(const char* parametro) {
	return strcmp(parametro, "") == 0;
}


// Chequea que el rango de doctores ingresado en el comando INFORME esté en orden alfabético.
// Pre: recibe los dos parametros.
// Post: devuelve true si están en orden alfabético, false si no.
bool validar_orden_alfabetico(const char* doctor1, const char* doctor2) {
	if (strcmp(doctor1, doctor2) > 0) {
		printf(ENOENT_FORMATO, "INFORME");
		return false;
	}
	return true;
}


// Imprime el doctor iterado actualmente.
// Pre: recibe el nombre del doctor, sus datos, y el numero actual del doctor iterado.
void imprimir_doctor(const char* nombre, doctor_t* doctor, size_t numero) {
	printf(INFORME_DOCTOR, numero, nombre, obtener_especialidad(doctor), obtener_atendidos(doctor));
}


// Funcion que recibe el iterador interno para imprimir los doctores.
bool imprimir(const char* dato, void* valor, void* extra) {
	imprimir_doctor(dato, valor, *(size_t*)extra += 1);
	return false;
}


// Funcion que recibe el iterador para contar los doctores.
bool contar_docs(const char* dato, void* valor, void* extra) {
	*(size_t*)extra += 1;
	return true;
}


// Imprime por pantalla al paciente encolado.
// Pre: recibe el nombre del paciente, la especialidad de su turno, el tamanio restante de la fila de espera.
void imprimir_paciente_encolado(const char* nombre_paciente, const char* especialidad, size_t tamanio_fila) {
	printf(PACIENTE_ENCOLADO, nombre_paciente);
	printf(CANT_PACIENTES_ENCOLADOS, tamanio_fila, especialidad);
}


/* ******************************************************************
 *                      FUNCIONES PRINCIPALES
 * *****************************************************************/

void comando_pedir_turno(const char** parametros, hash_t* pacientes, hash_t* especialidades) {
	int errores_pedir_turno = 0;
	if(!hash_pertenece(pacientes, parametros[0])) {
		printf(ENOENT_PACIENTE, parametros[0]);
		errores_pedir_turno += 1;
	}
	especialidad_t* especialidad = hash_obtener(especialidades, parametros[1]);
	if(!especialidad) {
		printf(ENOENT_ESPECIALIDAD, parametros[1]);
		errores_pedir_turno += 1;
	}
	if(strcmp(parametros[2], "URGENTE") != 0 && strcmp(parametros[2], "REGULAR") != 0) {
		printf(ENOENT_URGENCIA, parametros[2]);
		errores_pedir_turno += 1;
	}
	if (errores_pedir_turno) {
		return;
	}
	especialidad_encolar_paciente((char*)parametros[2], (char*)parametros[0], pacientes, especialidad);
	imprimir_paciente_encolado(parametros[0], parametros[1], obtener_tamanio_fila(especialidad));
}


void comando_atender(const char** parametros, hash_t* pacientes, abb_t* doctores, hash_t* especialidades) {
	doctor_t* doctor = abb_obtener(doctores, parametros[0]);
	if(!doctor) {
		printf(ENOENT_DOCTOR, parametros[0]);
		return;
	}
	char* nombre_especialidad = obtener_especialidad(doctor); 
	especialidad_t* especialidad = hash_obtener(especialidades, nombre_especialidad);
	paciente_t* paciente = especialidad_desencolar_paciente(especialidad);
	if (!paciente) {
		printf(SIN_PACIENTES);
		return;	
	}

	char* nombre_paciente = (char*)obtener_nombre_paciente(paciente);
	doctor_atender(doctor);
	printf(PACIENTE_ATENDIDO, nombre_paciente);
	printf(CANT_PACIENTES_ENCOLADOS, obtener_tamanio_fila(especialidad), nombre_especialidad);
}


void comando_informe(const char** parametros, abb_t* doctores) {
	char* inicio = (char*)parametros[0];
	char* fin = (char*)parametros[1];
	if (!inicio || !fin) {
		printf(ENOENT_PARAMS, "INFORME");
		return;
	}
	if (es_vacio(inicio)) inicio = "A";
	if (es_vacio(fin)) fin = "Z";
	size_t cant_doctores = 0;
	abb_in_order_por_rango(doctores, inicio, fin, contar_docs, &cant_doctores);
	printf(DOCTORES_SISTEMA, cant_doctores);
	size_t numero_de_doctor = 0;
	abb_in_order_por_rango(doctores, inicio, fin, imprimir, &numero_de_doctor);
}


bool cargar_datos(char** argv, hash_t* pacientes, abb_t* doctores, hash_t* especialidades) {
	bool carga_doctores_esp_ok = cargar_doctores_y_especialidades(argv[1], doctores, especialidades);
	bool carga_pacientes_ok = cargar_pacientes(argv[2], pacientes);
	if (!carga_doctores_esp_ok || !carga_pacientes_ok) {
		return false;
	}
	return true;
}


void procesar_comando(const char* comando, const char** parametros, hash_t* pacientes, abb_t* doctores, hash_t* especialidades) {
	if (strcmp(comando, COMANDO_PEDIR_TURNO) == 0) {
		if(!parametros[0] || !parametros[1] || !parametros[2]) {
			printf(ENOENT_FORMATO, comando);
			return;
		}
		comando_pedir_turno(parametros, pacientes, especialidades);
	} else if (strcmp(comando, COMANDO_ATENDER) == 0) {
		if(!parametros[0] || parametros[1]) {
			printf(ENOENT_FORMATO, comando);
			return;
		}
		comando_atender(parametros, pacientes, doctores, especialidades);
	} else if (strcmp(comando, COMANDO_INFORME) == 0) {
		comando_informe(parametros, doctores);
	} else {
		printf(ENOENT_CMD, comando);
	}
}


void eliminar_fin_linea(char* linea) {
	size_t len = strlen(linea);
	if (linea[len - 1] == '\n') {
		linea[len - 1] = '\0';
	}
}


void procesar_entrada(hash_t* pacientes, abb_t* doctores, hash_t* especialidades) {
	char* linea = NULL;
	size_t c = 0;
	while (getline(&linea, &c, stdin) > 0) {
		eliminar_fin_linea(linea);
		char** campos = split(linea, ':');
		if (campos[1] == NULL) {
			printf(ENOENT_FORMATO, linea);
			free_strv(campos);
			continue;	
		}
		char** parametros = split(campos[1], ',');
		procesar_comando(campos[0], (const char**)parametros, pacientes, doctores, especialidades);
		free_strv(parametros);
		free_strv(campos);
	}
	free(linea);
}


/* ******************************************************************
 *                              MAIN
 * *****************************************************************/

int main(int argc, char** argv) {
	if(!argv[1] || !argv[2]) {
		printf(ENOENT_CANT_PARAMS);
		return 1;
	}
	// ### Creación de estructuras y carga de datos ### //
	hash_t* pacientes = hash_crear(destruir_paciente_wrapper);
	abb_t* doctores = abb_crear(strcmp, destruir_doctor_aux);
	hash_t* especialidades = hash_crear(destruir_especialidades);
	bool carga_ok = cargar_datos(argv, pacientes, doctores, especialidades);

	// ### Procesar la entrada de datos ### //
	if (!carga_ok) return 1;
	procesar_entrada(pacientes, doctores, especialidades);

	// ### Destruir las estructuras utilizadas ### //
	hash_destruir(pacientes);
	abb_destruir(doctores);
	hash_destruir(especialidades);
	return 0;
}
