#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "files.h"

static bool preposicao(char *str);

int abrir_lista(char ***lista, int name_len, char *filename)
{
    int i = 0;
    int max = 10;
    char buff[name_len + 1];
    FILE *file = fopen(filename, "r");


    if (file == NULL) {
        fprintf(stderr, "Arquivo não pôde ser aberto\n");
        return 0;
    }

    *lista = allocate(*lista, max);

    while (get_line(buff, name_len, file) != NULL) {
        (*lista)[i] = malloc(strlen(buff) + 1);
        strcpy((*lista)[i], buff);
        if (++i == max) {
            max *= 2;
            *lista = reallocate(*lista, max);
        }
    }

    *lista = reallocate(*lista, i);

    return (*lista == NULL) ? 0 : i;
}

char *get_line(char *str, size_t size, FILE *stream)
{
    int ch;
    unsigned i = 0;

    while ((ch = getc(stream)) != '\n' && ch != EOF)
        if (i < size)
            str[i++] = ch;
    if (ch == EOF && i == 0)
        return NULL;
    str[i] = '\0';
    return str;
}

int meu_atoi(const char *str)
{
    while (isspace(*str))
        str++;
    if (!isdigit(*str))
        return EOF;

    int numero = 0;

    while (isdigit(*str++)) {
        numero *= 10;
        numero += *str - '0';
    }
    return numero;
}

void toupper_all(char *str)
{
    bool out = true;

    while (*str != '\0') {
        if (isalpha(*str)) {
            if (out && !preposicao(str))
                *str = toupper(*str);
            else 
                *str = tolower(*str);
            out = false;
        } else {
            out = true;
        }
        ++str;
    }
}

static bool preposicao(char *str)
{
    int count = 0;

    while (isalpha(*str)) {
        if (count == 0 && tolower(*str) != 'd')
            return false;
        ++count;
        ++str;
    }

    return count == 2;
}

void *alloc(size_t size)
{
    void *new = malloc(size);
    if (new == NULL) {
        fprintf(stderr, "Não há memória suficiente\n");
        exit(EXIT_FAILURE);
    } 
    return new;
}
