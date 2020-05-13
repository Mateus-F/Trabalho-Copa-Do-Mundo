#ifndef PROJETO_H
#define PROJETO_H

#include <stdbool.h>

#define MAX_TIMES 32
#define MAX_RANQUE_TIMES 16
#define MAX_TIMES_GRUPO 4
#define NUM_GRUPOS 8
#define MAX_JOGOS_GRUPOS 48
#define MAX_JOGOS 64
#define MAX_NAME_LEN 30
#define MAX_GOLS 18
#define MAX_IDS 100
#define MAX_FASES 6
#define TERCEIRO 2
#define QUARTO 3
#define MAX_LISTA_LEN 100

typedef int Id;

typedef enum Status {ELIMINADO, PENDENTE, PASSOU, CAMPEAO} Status;
typedef enum Estagio {NORMAL, PRORROGACAO, PENALTY} Estagio;
typedef enum Fase {GRUPOS, OITAVAS, QUARTAS, SEMI, FINAL, PELO_TERCEIRO, FIM} Fase;

typedef struct Horario {int dia, mes, horas, minutos;} Horario;

typedef struct Info {int jogos_realizados, times_cadastrados, times_classificados; Horario ultima_data; int jogos_pareados;} Info;

typedef struct Time {Status status; char nome[MAX_NAME_LEN + 1]; Id id; int vitorias, derrotas, gols, gols_sofridos, pontos, empates;} Time;

typedef struct Placar {int vencedor, perdedor;} Placar;

typedef struct Resultado {Placar placar; Estagio estagio;Id vencedor, perdedor; bool empate;} Resultado;

typedef struct Jogo {Horario data; Fase fase; Resultado resultado;
                      Id id; char local[MAX_NAME_LEN + 1];} Jogo;

typedef struct Chave {Id t1; Id t2;} Chave;

#endif
