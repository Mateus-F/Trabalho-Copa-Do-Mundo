
#ifndef FILES_H
#define FILES_H

#include <stdio.h>

#include "projeto.h"

#define allocate(var, size) (alloc(sizeof(*var) * size))
#define reallocate(var, size) (realloc(var, sizeof(*var) * size))

int abrir_lista(char ***lista, int name_len, char *filename);
void *alloc(size_t size);
int meu_atoi(const char *str);
char *get_line(char *str, size_t max, FILE *stream);
void toupper_all(char *str);

#endif
