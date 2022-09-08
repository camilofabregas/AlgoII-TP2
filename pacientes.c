#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lista.h"
#include "csv.h"
#include "pacientes.h"
#include "mensajes.h"

struct paciente {
	char* nombre_paciente;
	int anio_inscripcion;
};

char* obtener_nombre_paciente(paciente_t* paciente) {
	return paciente->nombre_paciente;
}

int obtener_anio_paciente(paciente_t* paciente) {
	return paciente->anio_inscripcion;
}

paciente_t* paciente_crear(char* nombre_paciente, int anio_inscripcion) {
	paciente_t* paciente = malloc(sizeof(paciente_t));
	if (!paciente) return NULL;
	paciente->nombre_paciente = strdup(nombre_paciente);
	if (!paciente->nombre_paciente) {
		free(paciente);
		return NULL;
	}
	paciente->anio_inscripcion = anio_inscripcion;
	return paciente;
}

void destruir_paciente (paciente_t* paciente) {
	free(paciente->nombre_paciente);
	free(paciente);
}

void* parse_paciente(char** params, void* extra) {
	int anio_num = atoi(params[1]);
	if (strcmp(params[1], "0") != 0 && anio_num == 0) {
		printf(ENOENT_ANIO, params[1]);
		return NULL;
	}
	return paciente_crear(params[0], atoi(params[1]));
}

bool cargar_pacientes(char* ruta_csv, hash_t* pacientes) {
	lista_t* listado_pacientes = csv_crear_estructura(ruta_csv, parse_paciente, NULL);
	if(!listado_pacientes) {
		printf(ENOENT_ARCHIVO, ruta_csv);
		return false;
	}
	while (!lista_esta_vacia(listado_pacientes)) {
		paciente_t* paciente = lista_borrar_primero(listado_pacientes);
		if (!paciente) return false;
		hash_guardar(pacientes, paciente->nombre_paciente, paciente);
	}
	lista_destruir(listado_pacientes, NULL);
	return true;
}