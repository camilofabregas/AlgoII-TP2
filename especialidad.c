#include "especialidad.h"

struct especialidad{
	cola_t* urgentes;
	heap_t* regulares;
	size_t pacientes;
};

// Pre: recibe los pacientes
// Post: devuelve la prioridad segun su antiguedad
int _comparador_prioridades (const paciente_t* paciente1, const paciente_t* paciente2) {
	int anio_paciente1 = obtener_anio_paciente((paciente_t*)paciente1);
	int anio_paciente2 = obtener_anio_paciente((paciente_t*)paciente2);
	return (int)anio_paciente2 - (int)anio_paciente1;
}

// Comparador de prioridades principal 
int comparador_prioridades (const void* valor1, const void* valor2) {
	return _comparador_prioridades(valor1, valor2);
}

especialidad_t* crear_especialidad() {
	especialidad_t* especialidad = malloc(sizeof(especialidad_t));
	if(!especialidad) {
		return NULL;
	}
	cola_t* urgentes  = cola_crear();
	heap_t* regulares = heap_crear(comparador_prioridades);
	especialidad->urgentes = urgentes;
	especialidad->regulares = regulares;
	especialidad->pacientes = 0;
	return especialidad;
}

cola_t* obtener_urgentes(especialidad_t* especialidad) {
	return especialidad->urgentes;
}

heap_t* obtener_regulares(especialidad_t* especialidad) {
	return especialidad->regulares;
}

void aumentar_tamanio_fila(especialidad_t* especialidad) {
	especialidad->pacientes = especialidad->pacientes + 1;
}

void disminuir_tamanio_fila(especialidad_t* especialidad) {
	especialidad->pacientes = especialidad->pacientes - 1;	
}

size_t obtener_tamanio_fila(especialidad_t* especialidad) {
	return especialidad->pacientes;
}

void especialidad_destruir(especialidad_t* especialidad, void (*destruir_dato_cola)(void*), void (*destruir_dato_heap)(void*)) {
	cola_t* urgentes = especialidad->urgentes;
	heap_t* regulares = especialidad->regulares;
	cola_destruir(urgentes, destruir_dato_cola);
	heap_destruir(regulares, destruir_dato_heap);
	free(especialidad);
}

void especialidad_encolar_paciente(char* urgencia, char* nombre_paciente, hash_t* pacientes, especialidad_t* especialidad) {
	paciente_t* paciente_a_guardar = hash_obtener(pacientes, nombre_paciente);
	if (strcmp(urgencia, "URGENTE") == 0) {
		cola_t* urgentes = obtener_urgentes(especialidad);
		cola_encolar(urgentes, paciente_a_guardar);
	}
	else if (strcmp(urgencia, "REGULAR") == 0) {
		heap_t* regulares = obtener_regulares(especialidad);
		heap_encolar(regulares, paciente_a_guardar); 
	}
	aumentar_tamanio_fila(especialidad);
}

paciente_t* especialidad_desencolar_paciente(especialidad_t* especialidad) {
	paciente_t* paciente = NULL;
	cola_t* urgentes  = obtener_urgentes(especialidad);
	heap_t* regulares = obtener_regulares(especialidad);
	if(!cola_esta_vacia(urgentes)) {
		paciente = (paciente_t*)cola_desencolar(urgentes);
		disminuir_tamanio_fila(especialidad);
	}
	else if(!heap_esta_vacio(regulares)) {
		paciente = (paciente_t*)heap_desencolar(regulares);
		disminuir_tamanio_fila(especialidad);
	}
	return paciente;
}