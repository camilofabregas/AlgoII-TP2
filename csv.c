#define _POSIX_C_SOURCE 200809L //getline
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv.h"
#define SEPARADOR ','

static void eliminar_fin_linea(char* linea) {
	size_t len = strlen(linea);
	if (linea[len - 1] == '\n') {
		linea[len - 1] = '\0';
	}
}

lista_t* csv_crear_estructura(const char* ruta_csv, void* (*creador) (char**, void*), void* extra) {
	FILE* archivo = fopen(ruta_csv, "r");
	if (!archivo) {
		return NULL;
	}
	
	lista_t* lista = lista_crear();
	if (!lista) {
		fclose(archivo);
		return NULL;
	}

	char* linea = NULL;
	size_t c = 0;
	while (getline(&linea, &c, archivo) > 0) {
		eliminar_fin_linea(linea);
		char** campos = split(linea, SEPARADOR);
		lista_insertar_ultimo(lista, creador(campos, extra));
		free_strv(campos);
	}
	free(linea);
	fclose(archivo);
	return lista;
}

