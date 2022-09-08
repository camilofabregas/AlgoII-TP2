#define _POSIX_C_SOURCE 20089L
#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "strutil.h"

char* substr(const char* str, size_t n) {
	char* str_copia = malloc(sizeof(char) * (n + 1));
	if (!str_copia) return NULL;
	strncpy(str_copia, str, n);
	str_copia[n] = '\0';
	return str_copia;
}


char** split(const char* str, char sep) {
	size_t str_largo = strlen(str);
	size_t arr_largo = 0;
	for (size_t i = 0; i < str_largo; i++) {
		if (str[i] == sep) arr_largo += 1; 
	}
	char** arr = malloc(sizeof(char*) * (arr_largo + 2));
	if (!arr) return NULL;
	size_t substr_inicio = 0;
	size_t arr_actual = 0;
	for (size_t i = 0; i < str_largo+1; i++) {
		if (str[i] == sep || str[i] == '\0') { 
			arr[arr_actual] = substr(str + substr_inicio, i - substr_inicio);
			substr_inicio = i+1;
			arr_actual += 1;
		}
	}
	arr[arr_actual] = NULL;
	return arr;
}


char* join(char** strv, char sep) {
	if (strv[0] == NULL) {
		return strdup("");
	}
	size_t str_largo = 0;
	for (size_t i = 0; strv[i]; i++) {
		str_largo += strlen(strv[i]);
		if (sep != '\0') {
			if (!strv[i+1]) str_largo += i; //Si estoy al final, le sumo i (cantidad de separadores)
		}
	}
	char* str = malloc(sizeof(char) * (str_largo + 1));
	if (!str) return NULL;
	size_t str_fin = 0;
	for (size_t i = 0; strv[i]; i++) {
		if (strv[i+1]) {
			if (sep != '\0') snprintf(str + str_fin, str_largo+1, "%s%c", strv[i], sep);
			else snprintf(str + str_fin, str_largo+1, "%s", strv[i]);
		}
		else {
			snprintf(str + str_fin, str_largo+1, "%s", strv[i]);	
		}
		if (sep != '\0') str_fin += strlen(strv[i]) + 1;
		else str_fin += strlen(strv[i]);
	}
	return str;
}


void free_strv(char *strv[]) {
	for (size_t i = 0; strv[i]; i++) {
		free(strv[i]);
	}
	free(strv);
}