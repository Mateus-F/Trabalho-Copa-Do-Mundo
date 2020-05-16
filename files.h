#ifndef FILES_H
#define FILES_H

#include <stdbool.h>

#include "projeto.h"

int abrir_lista(char lista[][MAX_NAME_LEN + 1], size_t size, char *filename);
int meu_atoi(const char *str);
char *fgets_(char *str, size_t size, FILE *io_stream);
void toupper_all(char *str);
bool preposicao(char *str);

#endif
