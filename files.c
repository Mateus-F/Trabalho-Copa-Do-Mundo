#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "files.h"
#include "projeto.h"

static bool preposicao(char *str);

int abrir_lista(char lista[][MAX_NAME_LEN + 1], size_t size, char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "O arquivo não pôde ser aberto\n");
        return 0;
    }

    int i = 0;
    char buff[MAX_NAME_LEN + 1];

    while (i < size) {
        if (fgets_(buff, MAX_NAME_LEN, file) == NULL)
            break;
        strcpy(lista[i], buff);
        ++i;
    }
    fclose(file);

    return i;
}

char *fgets_(char *str, size_t size, FILE *stream)
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
    while (isdigit(*str)) {
        numero *= 10;
        numero += *str - '0';
        ++str;
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
            out = false;
        } else {
            out = true;
        }
        ++str;
    }
}

static bool 
preposicao(char *str)
{
    int count = 0;

    while (isalpha(*str)) {
        if (count == 0 && *str != 'd')
            return false;
        ++count;
        ++str;
    }

    return count == 2;
}
