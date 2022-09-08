#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doctores.h"
#include "especialidad.h"

struct doctor {
	char* nombre;
	char* especialidad;
	size_t atendidos;
};

doctor_t* crear_doctor(char* nombre, char* espec) {
	doctor_t* doctor = malloc(sizeof(doctor_t));
	if (!doctor) {
		return NULL;
	}
	doctor->nombre = strdup(nombre);
	if(!doctor->nombre) {
		free(doctor);
		return NULL;
	}
	doctor->especialidad = strdup(espec);
	if(!doctor->especialidad) {
		free(doctor->nombre);
		free(doctor);
		return NULL;
	}
	doctor->atendidos = 0;
	return doctor;
}

void destruir_doctor(doctor_t* doctor) {
	free(doctor->nombre);
	free(doctor->especialidad);
	free(doctor);
}

size_t obtener_atendidos(doctor_t* doctor) {
	return doctor->atendidos;
}

void doctor_atender(doctor_t* doctor) {
	doctor->atendidos = doctor-> atendidos + 1;
}

char* obtener_especialidad(doctor_t* doctor) {
	return doctor->especialidad;
}

void* parse_doctor (char** params, void* extra) {
	return crear_doctor(params[0], params[1]);
}

bool cargar_doctores_y_especialidades (char* ruta_archivo, abb_t* doctores, hash_t* especialidades) {
	lista_t* lista_doctores = csv_crear_estructura(ruta_archivo, parse_doctor, NULL);
	if(!lista_doctores) {
		printf(ENOENT_ARCHIVO, ruta_archivo);
		return false;
	}
	while(!lista_esta_vacia(lista_doctores)) {
		doctor_t* doctor = (doctor_t*)lista_borrar_primero(lista_doctores);
		abb_guardar(doctores, doctor->nombre, doctor);
		if(!hash_pertenece(especialidades, doctor->especialidad)) {
			especialidad_t* nueva_especialidad = crear_especialidad();
			hash_guardar(especialidades, doctor->especialidad, nueva_especialidad);
		}
	}
	lista_destruir(lista_doctores, NULL);
	return true;
}