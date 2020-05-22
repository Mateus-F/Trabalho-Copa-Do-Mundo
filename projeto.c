#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <unistd.h>

#include "projeto.h"
#include "files.h"
#include "cores.h"

bool automatico = false;

int obter_time_grupos(Time *t[], Time *grupos, Chave *chaves, int jogos_realizados);
void obter_time_copa(Time *t[], Time *times, Fase fase_atual, Chave *chaves,
                     int jogos_realizados);
void marcar_jogo(Time *times, Time *grupos, Info *copa, Jogo *jogos, Chave **chaves);
void swap_pelo_terceiro(Time *times);
void parear_times_eliminatorias(Time *times, Chave **chaves, int *jogos_pareados);
bool horario_valido(const Horario *data);
int dias_mes(int mes);
void update_data(Horario *data);
int qsort_ranquear_grupos(const void *p, const void *q);
int compara_ranque(const Time *t1, const Time *t2);
void insertion_sort(Time *times, size_t size);
bool data_valida(const Horario *data);
void nandomizar_times_grupos(Time *grupos, Chave *chaves);
void visualizar_grupos(Jogo *jogos, const Info *copa, Time *grupos);
void visualizar_jogos_realizados(const Jogo *jogos, const Info *copa, Time *times_lista);
void visualizar_ranque(const Time *times, const Info *copa);
void marcar_jogo_dados(Info *copa, Jogo *jogos, Fase fase);
void escolher_estadio(int jogos_realizados, char *jogo_atual_local);
void checar_transicao_proxima_fase(Time *times, Info *copa, Chave *chaves);
Resultado jogar_jogo(Time *t1, Time *t2, Fase fase);
Time *pesquisar_time_ptr(Time *times, Id id, size_t size);
void set_resultado(Time *vencedor, Time *perdedor, Resultado *resultado,
                   int gols_vencedor, int gols_perdedor, Fase fase);
void set_empate(Time *t1, Time *t2, Resultado *resultado, int gols_t1,
                int gols_t2);
Fase calcular_fase(int jogo_numero);
int comparar_data(const Horario *d1, const Horario *d2);
void cadastrar_times(Time *grupos, int *times_cadastrados, Chave *chaves);
void exibir_lista_times(char **lista, const Time *times,
                        int times_cadastrados, int abertos);
Id obter_novo_id(void);
bool time_repetido(const char *nome, const Time *times, int times_cadastrados);
bool obter_nome_por_id(char *buff, size_t max, char **lista);
bool confirmar_resposta(void);
void limpar_tela(void);
void sair_menu(void);
void mensagem(char *msg);
int obter_gols(void);
void easter_egg(Time *t1, Time *t2, int *gols_t1, int *gols_t2);

int main(void)
{
    setlocale(LC_ALL, "Portuguese");

    Time *times= allocate(times, MAX_RANQUE_TIMES);
    Time *grupos= allocate(grupos, MAX_TIMES);
    Chave *chaves = allocate(chaves, MAX_JOGOS_GRUPOS);
    Jogo *jogos = allocate(jogos, MAX_JOGOS);
    Info copa = {0, 0, 0, {1, 1, 12, 30}}; 

    srand((unsigned)time(NULL));
    limpar_tela();

    for (int i = 0; i < MAX_TIMES; ++i) 
        grupos[i] = (Time){PENDENTE, "", obter_novo_id(), 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < MAX_RANQUE_TIMES; ++i) 
        times[i] = (Time){PENDENTE, "", 0, 0, 0, 0, 0, 0, 0};

    printf("Deseja realizar os jogos no automatico(S/n)? ");
    bold_yellow();
    if (confirmar_resposta())
        automatico = true;
    reset();
    limpar_tela();

    puts("---------------------------------------------------------------"
         "---------------------------------------------------------------");
    puts("                                                           Copa do Mundo");
    puts("---------------------------------------------------------------"
         "---------------------------------------------------------------");

    for (;;) {
        if (copa.times_cadastrados == MAX_TIMES) 
            red();
        else
            bold_yellow();
        puts("1 - Cadastrar times");
        reset();
        if (copa.times_classificados == 0)
            white();
        puts("2 - Visualizar ranque");
        reset();
        if (copa.jogos_realizados == 0)
            white();
        puts("3 - Visualizar jogos");
        reset();
        if (copa.times_cadastrados == 0)
            white();
        puts("4 - Visualizar Grupos");
        reset();
        if (copa.times_cadastrados < MAX_TIMES 
                || copa.jogos_realizados == MAX_JOGOS)
            red();
        else
            bold_yellow();
        puts("5 - Marcar jogos");
        reset();
        puts("6 - Sair do programa");

        char input_str[3];
        int op;

        bold_yellow();
        get_line(input_str, 2, stdin);
        sscanf(input_str, "%d", &op);
        reset();
        limpar_tela();

        switch (op) {
        case 1:
            cadastrar_times(grupos, &copa.times_cadastrados, chaves);
            break;
        case 2:
            visualizar_ranque(times, &copa);
            break;
        case 3:
            visualizar_jogos_realizados(jogos, &copa, grupos);
            break;
        case 4:
            visualizar_grupos(jogos, &copa, grupos);
            break;
        case 5:
            marcar_jogo(times, grupos, &copa, jogos, &chaves);
            break;
        case 6:
            free(grupos);
            free(times);
            free(chaves);
            free(jogos);
            exit(EXIT_SUCCESS);
            break;
        default:
            red();
            puts("Opção inválida");
            reset();
            break;
        }
    }
}


int obter_time_grupos(Time *t[], Time *grupos, Chave *chaves, int jogos_realizados)
{
    t[0] = pesquisar_time_ptr(grupos, chaves[jogos_realizados].t1, MAX_TIMES);
    t[1] = pesquisar_time_ptr(grupos, chaves[jogos_realizados].t2, MAX_TIMES);

    int grupo  = t[0] - &grupos[0];

    grupo -= (grupo % MAX_TIMES_GRUPO);

    return grupo;
}

void obter_time_copa(Time *t[], Time *times, Fase fase_atual, Chave *chaves, int jogos_realizados) 
{
    int jogos_eliminatorias = jogos_realizados - MAX_JOGOS_GRUPOS;

    t[0] = pesquisar_time_ptr(times, chaves[jogos_eliminatorias].t1, MAX_RANQUE_TIMES);
    t[1] = pesquisar_time_ptr(times, chaves[jogos_eliminatorias].t2, MAX_RANQUE_TIMES);
}

void marcar_jogo(Time *times, Time *grupos, Info *copa, Jogo *jogos, Chave **chaves)
{
    if (copa->jogos_realizados == MAX_JOGOS) {
        mensagem("A copa já terminou\n");
        return;
    }

    if (copa->times_cadastrados < MAX_TIMES) {
        mensagem("Os times ainda não foram cadastrados\n");
        return;
    }

    Fase fase_atual = calcular_fase(copa->jogos_realizados);
    Time *t[2];
    int gols[2];
    char input_str[3];
    int grupo_atual;

    if (fase_atual == GRUPOS) { 
        grupo_atual = obter_time_grupos(t, grupos, *chaves, copa->jogos_realizados);
        jogos[copa->jogos_realizados].resultado = jogar_jogo(t[0], t[1], fase_atual);
        qsort(&grupos[grupo_atual], MAX_TIMES_GRUPO, sizeof(grupos[0]), qsort_ranquear_grupos);

        if (copa->jogos_realizados >= MAX_JOGOS_GRUPOS - MAX_RANQUE_TIMES / 2) {
            for (int i = 0; i < 2; ++i) {
                times[copa->times_classificados].id = grupos[grupo_atual + i].id;
                strcpy(times[copa->times_classificados++].nome, grupos[grupo_atual + i].nome);
            }
        }
        if (copa->jogos_realizados == MAX_JOGOS_GRUPOS - 1) 
            parear_times_eliminatorias(times, chaves, &copa->jogos_pareados);
    } else { 
        obter_time_copa(t, times, fase_atual, *chaves, copa->jogos_realizados);
        jogos[copa->jogos_realizados].resultado = jogar_jogo(t[0], t[1], fase_atual);

        if (fase_atual == PELO_TERCEIRO) 
            swap_pelo_terceiro(times);
        else 
            insertion_sort(times, MAX_RANQUE_TIMES);

        if (fase_atual == FINAL) {
            times[TERCEIRO].status = times[QUARTO].status = PENDENTE;
            (*chaves)[copa->jogos_pareados].t1 = times[TERCEIRO].id;
            (*chaves)[copa->jogos_pareados++].t2 = times[QUARTO].id;
        }
    }

    marcar_jogo_dados(copa, jogos, fase_atual);
    checar_transicao_proxima_fase(times, copa, *chaves);
    copa->jogos_realizados++;
}

int compara_ranque(const Time *t1, const Time *t2)
{
    if (t2->status != t1->status)
        return t2->status - t1->status;
    else if (t1->vitorias != t2->vitorias)
        return t2->vitorias - t1->vitorias;
    else 
        return t1->derrotas - t2->derrotas;
}

void swap_pelo_terceiro(Time *times)
{
    if (times[QUARTO].vitorias > times[TERCEIRO].vitorias) {
        Time tmp = times[QUARTO];
        times[QUARTO] = times[TERCEIRO];
        times[TERCEIRO] = tmp;
    }
}

void parear_times_eliminatorias(Time *times, Chave **chaves, int *jogos_pareados)
{
    int j = 0;
    Time tmp[MAX_RANQUE_TIMES];
    int chave = 0;

    *chaves = reallocate(*chaves, (MAX_JOGOS - MAX_JOGOS_GRUPOS));

    for (int i = 0; i < MAX_RANQUE_TIMES; ++i) 
        tmp[i] = times[i];

    for (int i = 0; i < MAX_RANQUE_TIMES / 2; i++) {
        times[i] = tmp[j++];
        times[i + MAX_RANQUE_TIMES / 2] = tmp[j++];
    }

    for (int i = 0; i < MAX_RANQUE_TIMES; i += 2) {
        (*chaves)[chave].t1 = times[i].id;
        (*chaves)[chave].t2 = times[i + 1].id;
        ++chave;
    }

    *jogos_pareados = chave;
}

int qsort_ranquear_grupos(const void *p, const void *q)
{
    const Time *t1 = p;
    const Time *t2 = q;

    if (t1->pontos != t2->pontos)
        return (t2->pontos - t1->pontos);
    else
        return (t2->gols - t2->gols_sofridos) - (t1->gols - t1->gols_sofridos);
}

bool horario_valido(const Horario *data)
{
    return data->horas >= 0 && data->horas < 24
           && data->minutos >= 0 && data->minutos < 60;
}

bool data_valida(const Horario *data)
{
        return data->mes >= 1 && data->mes <= 12 
        && data->dia >= 1 && data->dia <= dias_mes(data->mes);
}


void randomizar_times_grupos(Time *grupos, Chave *chaves)
{
    for (int i = 0; i < MAX_TIMES; ++i) {
        int j = rand() % MAX_TIMES;
        Time tmp = grupos[i];
        grupos[i] = grupos[j];
        grupos[j] = tmp;
    }

    int chave = 0;

    for (int i = 0; i < MAX_TIMES_GRUPO - 1; ++i) 
        for (int j = i + 1; j < MAX_TIMES_GRUPO; ++j) 
            for (int k = 0; k < MAX_TIMES; k += MAX_TIMES_GRUPO) {
                chaves[chave].t1 = grupos[k + i].id;
                chaves[chave].t2 = grupos[k + j].id;
                ++chave;
            }
}

void visualizar_grupos(Jogo *jogos, const Info *copa, Time *grupos)
{
    char grupo_letra = 'A';

    for (int i = 0; i < NUM_GRUPOS; i++) {
        int grupo = i * MAX_TIMES_GRUPO;

        white();
        puts("-----------------------------------------------------------------------------------");
        printf("                                    Grupo %c\n", grupo_letra++);
        puts("-----------------------------------------------------------------------------------");
        reset();
        puts("\e[090mId   Time\e[0m                  \e[1;36mVitórias  \e[1;31mDerrotas\e[0m  \e[1;33mEmpates  "
                "\e[1;36mGols  \e[1;31mGols sofridos  \e[1;32mPontos\e[0m\n");
        for (int j = grupo; j < grupo + MAX_TIMES_GRUPO && j < copa->times_cadastrados; ++j) 
            printf("%-2d   \e[1;34m%-24s \e[1;36m%-2d        \e[1;31m%-2d        \e[1;33m%-2d     "
                    "\e[1;36m%-2d        \e[1;31m%-2d          \e[1;32m%-2d\e[0m\n",
                    grupos[j].id, grupos[j].nome, grupos[j].vitorias, grupos[j].derrotas,
                    grupos[j].empates, grupos[j].gols, grupos[j].gols_sofridos, 
                    grupos[j].pontos);

    }

    sair_menu();
    limpar_tela();
}

void visualizar_ranque(const Time *times, const Info *copa)
{
    static const char *STATUS[] = {"\e[0;31mEliminado\e[0m", "\e[1;33mPendente\e[0m", "\e[0;32mPassou\e[0m", "\e[1;36mCampeao\e[0m"};
    static const char *FASES_NOMES[MAX_FASES + 1] = {"Grupos", "Oitavas de final", "Quartas de final",
        "Semi Final", "Final", "Competição pelo terceiro lugar", "Fim da copa"};

    white();
    puts("------------------------------------------------------------------------------------------------------------------------------");
    puts("                                                           Ranque da Copa");
    puts("------------------------------------------------------------------------------------------------------------------------------");
    printf("                                                          %s\n", FASES_NOMES[calcular_fase(copa->jogos_realizados)]);
    puts("------------------------------------------------------------------------------------------------------------------------------");

    printf("        ID      Times                           Status\e[0m                "
            "\e[1;32mVitórias\e[0m  \e[1;31mDerrotas\e[0m     \e[1;32mGols\e[0m        \e[1;31mGols sofridos\e[0m"); 
    printf("\n");
    for (int i = 0; i < copa->times_classificados; ++i) {
        char *color = DEFAULT;

        if (times[i].derrotas > 0)
            color = RED;

        printf("%2d°     %-2d     \e[1;34m%-24s\e[0m       %-26s            \e[1;32m%-2d        "
                "%s%-2d         \e[0;32m%-2d              \e[1;31m%-2d\e[0m \n", i + 1, times[i].id,
                times[i].nome, STATUS[times[i].status], times[i].vitorias, color, times[i].derrotas,
                times[i].gols, times[i].gols_sofridos);
    }

    white();
    if (copa->times_classificados == 0)
        puts("\n\n                                                "
                "Nenhum time foi classificado para as oitavas ainda\n");
    reset();

    sair_menu();
}

void visualizar_jogos_realizados(const Jogo *jogos, const Info *copa, Time *times_lista)
{
    static const char *FASES_NOMES[MAX_FASES] = {"Grupos", "Oivatas", "Quartas", "Semi Final", "Final", "Pelo 3"};

    white();
    puts("   ID      Fase        Dia      Horario                 Vencedor  Placar  Perdedor                 Estádio             Penalty");
    reset();

    for (int i = 0; i < copa->jogos_realizados; ++i) {		
        char *color_t1;
        char *color_t2;
        Time *vencedor = pesquisar_time_ptr(times_lista, jogos[i].resultado.vencedor, MAX_TIMES);
        Time *perdedor = pesquisar_time_ptr(times_lista, jogos[i].resultado.perdedor, MAX_TIMES);

        if (jogos[i].resultado.empate) {
            color_t1 = color_t2 = BOLD_YELLOW;
        } else {
            color_t1 = BOLD_CYAN;
            color_t2 = BOLD_RED;
        }

        printf("\n   %-2d", jogos[i].id);
        printf("    %-11s", FASES_NOMES[jogos[i].fase]);
        printf(" %2d/%-2d", jogos[i].data.dia, jogos[i].data.mes);
        printf("    %.2d:%.2d", jogos[i].data.horas, jogos[i].data.minutos);
        printf("    %s%24.24s\e[0m", color_t1, vencedor->nome);
        printf(" %2d x %-2d", jogos[i].resultado.placar.vencedor,
                jogos[i].resultado.placar.perdedor);
        printf("  %s%-24.24s\e[0m", color_t2, perdedor->nome);
        printf("  %-18.18s", jogos[i].local);

        bold_yellow();
        if (jogos[i].resultado.estagio >= PENALTY)
            printf("  Sim");
        if (jogos[i].resultado.empate)
            printf("  Empate");
        reset();
    }

    white();
    if (copa->jogos_realizados == 0)
        puts("\n                                                        Nenhum jogo foi Realizado ainda");
    reset();

    sair_menu();	
}

void marcar_jogo_dados(Info *copa, Jogo *jogos, Fase fase)
{
    Jogo *jogo_atual = &jogos[copa->jogos_realizados];
    Horario data;

    if (automatico) {
        char *estadios[12] = {"Arena", "Maracana", "Mane Garrincha", "Morumbi",
            "Castelao", "Mineirao", "Arruda", "Alena do Gremio",
            "Parque do Sabia", "Albertao", "Beira-Rio", "Mangueirao"};

        data = copa->ultima_data;
        update_data(&data);
        strcpy(jogos[copa->jogos_realizados].local, estadios[rand() % 12]);
    } else {
        for (;;) {
            char input_str[10];

            printf("Escolha a data em que deseja fazer o jogo (dd/mm): ");
            bold_yellow();
            get_line(input_str, 9, stdin);
            sscanf(input_str,"%d /%d", &data.dia, &data.mes);
            reset();
            if (!data_valida(&data)) {
                red();
                mensagem("Essa data não é válida\n");
                continue;
            }
            printf("Escolha o horário em que o jogo aconteceu (hh:mm): ");
            bold_yellow();
            get_line(input_str, 9, stdin);
            sscanf(input_str, "%2d :%2d", &data.horas, &data.minutos);
            reset();
            if (!horario_valido(&data)) {
                red();
                mensagem("Esse Horário não é valido\n");
                continue;
            }

            int horas_diferenca = comparar_data(&copa->ultima_data, &data);

            if (copa->jogos_realizados > 0 && 
                    (horas_diferenca >= 0 || abs(horas_diferenca) <= 12)) {
                red();
                mensagem("Essa data já passou ou está muito próxima do último jogo\n");
                continue;
            }
            break;
        }

        escolher_estadio(copa->jogos_realizados, jogo_atual->local);
    }

    copa->ultima_data = data;
    jogo_atual->data = data;
    jogo_atual->fase= fase;
    jogo_atual->id = obter_novo_id();
    limpar_tela();
}

void update_data(Horario *data)
{
    int dias_mes_data = dias_mes(data->mes);
    const int minutos[6] = {10, 15, 20, 25, 30, 45};

    data->dia += rand() % 3 + 1;
    if (data->dia > dias_mes_data) {
        data->dia -= dias_mes_data;
        ++data->mes;
    }

    data->horas = rand() % 24;
    data->minutos = minutos[rand() % 6];
}


int dias_mes(int mes)
{
    static const int dias[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    return dias[mes - 1];
}

void escolher_estadio(int jogos_realizados, char *jogo_atual_local)
{
    char buff[MAX_NAME_LEN + 1];
    Id id;
    char **estadios;
    int num_estadios = abrir_lista(&estadios, MAX_NAME_LEN, "estadios.txt");

    for (int i = 0; i < num_estadios; ++i)
        printf("\e[1;32m%d\e[0m - \e[1;34m%s\e[0m\n", i, estadios[i]);

    for (;;) {
        if (num_estadios > 0) {
            printf("Digite um nome de estádio ou digite um dos ids acima: ");
            bold_yellow();
            get_line(buff, MAX_NAME_LEN, stdin);
            reset();
            if (!obter_nome_por_id(buff, num_estadios, estadios))
                continue;
        } else {
            printf("Digite o nome do estádio: ");
            bold_yellow();
            get_line(buff, MAX_NAME_LEN, stdin);
            reset();
        }
        break;
    }

    strcpy(jogo_atual_local, buff);
    reset();
}

void checar_transicao_proxima_fase(Time *times, Info *copa, Chave *chaves)
{
    for (int i = 0; i < MAX_RANQUE_TIMES; ++i)
        if (times[i].status == PENDENTE)
            return;

    for (int i = 0; times[i].status == PASSOU; ++i)
        times[i].status = PENDENTE;

    for (int i = 0; times[i].status == PENDENTE; i += 2) {
        chaves[copa->jogos_pareados].t1 = times[i].id;
        chaves[copa->jogos_pareados].t2 = times[i + 1].id;
        ++copa->jogos_pareados;
    }
}

Resultado jogar_jogo(Time *t1, Time *t2, Fase fase)
{
    Resultado resultado;
    Id id_vencedor = EOF;
    char input_str[4];
    int t1_gols;
    int t2_gols;

    resultado.estagio = NORMAL;
    resultado.empate = false;

    if (automatico) { 
        t1_gols = obter_gols();
        t2_gols = obter_gols();
        if (t1_gols == t2_gols) {
            if (fase != GRUPOS) {
                id_vencedor = rand() % 2 == 1 ? t1->id : t2->id;
                resultado.estagio = PENALTY;
            } else {
                resultado.empate = true;
            }
        }
        easter_egg(t1, t2, &t1_gols, &t2_gols);
        easter_egg(t2, t1, &t2_gols, &t1_gols);
    } else {
        printf("Time \e[1;33m%s\e[0m vs \e[1;33m%s\e[0m\n\n", t1->nome, t2->nome);
        printf("Digite quantos gols o time \e[1;33m%s\e[0m fez: ", t1->nome);

        bold_yellow();
        get_line(input_str, 3, stdin);
        sscanf(input_str, "%d", &t1_gols);
        reset();

        printf("Digite quantos gols o time \e[1;33m%s\e[0m fez: ", t2->nome);
        bold_yellow();
        get_line(input_str, 3, stdin);
        reset();
        sscanf(input_str, "%d", &t2_gols);
    }

    t1->gols += t1_gols;
    t2->gols += t2_gols;
    t1->gols_sofridos += t2_gols;
    t2->gols_sofridos += t1_gols;

    if (t1_gols > t2_gols) {
        id_vencedor = t1->id;
    } else if (t2_gols > t1_gols) {
        id_vencedor = t2->id;
    } else if (fase != GRUPOS && !automatico) {
        char buff_str[MAX_NAME_LEN + 1];

        resultado.estagio = PENALTY;
        printf("Os times empataram, digite quem foi o vencedor dos penaltys:"
                " \e[1;33m%s\e[0m ou \e[1;33m%s\e[0m:\n", t1->nome, t2->nome);
        for (;;) {
            bold_yellow();
            get_line(buff_str, MAX_NAME_LEN, stdin);
            reset();
            if (strcmp(t1->nome, buff_str) == 0)
                id_vencedor = t1->id;
            else if (strcmp(t2->nome, buff_str) == 0)
                id_vencedor = t2->id;
            else {
                red();
                limpar_tela();
                printf("O nome que você digitou não é válido, digite denovo:"
                        " \e[1;33m%s\e[0m ou \e[1;33m%s\e[0m\n", t1->nome, t2->nome);
                bold_yellow();
                sleep(1);
                reset();
                limpar_tela();
                continue;
            }
            break;
        }
    } 

    if (id_vencedor == t1->id) 
        set_resultado(t1, t2, &resultado, t1_gols, t2_gols, fase);
    else if (id_vencedor == t2->id) 
        set_resultado(t2, t1, &resultado, t2_gols, t1_gols, fase);
    else 
        set_empate(t1, t2, &resultado, t1_gols, t2_gols);
    

    return resultado;
}

void set_empate(Time *t1, Time *t2, Resultado *resultado, int gols_t1,
                int gols_t2) 
{
        ++t1->pontos;
        ++t2->pontos;
        ++t1->empates;
        ++t2->empates;
        resultado->empate = true;
        resultado->vencedor = t1->id;
        resultado->perdedor = t2->id;
        resultado->placar.vencedor = gols_t1;
        resultado->placar.perdedor = gols_t1;
}

Time *pesquisar_time_ptr(Time *times, Id id, size_t max)
{
    for (int i = 0; i < max; ++i) 
        if (times[i].id == id)
            return &times[i];
    return NULL;
}

void set_resultado(Time *vencedor, Time *perdedor, Resultado *resultado, int gols_vencedor, int gols_perdedor, Fase fase)
{
    resultado->vencedor = vencedor->id;
    resultado->perdedor = perdedor->id;
    resultado->placar.vencedor = gols_vencedor;
    resultado->placar.perdedor = gols_perdedor;
    if (fase != GRUPOS) {
        if (fase == FINAL) 
            vencedor->status = CAMPEAO;
        else if (fase == PELO_TERCEIRO) 
            vencedor->status = ELIMINADO;
        else 
            vencedor->status = PASSOU;
        perdedor->status = ELIMINADO;
    }
    ++vencedor->vitorias;
    ++perdedor->derrotas;
    vencedor->pontos += 3;
}

Fase calcular_fase(int jogo_numero)
{
    if (jogo_numero == MAX_JOGOS)
        return FIM;
    if (jogo_numero == MAX_JOGOS - 1)
        return PELO_TERCEIRO;
    if (jogo_numero < MAX_JOGOS_GRUPOS)
        return GRUPOS;

    int margem = (MAX_JOGOS - MAX_JOGOS_GRUPOS) / 2;
    int limite = margem + MAX_JOGOS_GRUPOS;
    Fase fase;

    for (fase = OITAVAS; jogo_numero >= limite; ++fase) {
        margem /= 2;
        limite += margem;
    }

    return fase; 
}


int comparar_data(const Horario *d1, const Horario *d2)
{
    if (d1->mes != d2->mes)
        return (d1->mes - d2->mes) * 24 * 31;
    else if (d1->dia != d2->dia)
        return (d1->dia - d2->dia) * 24 - (d1->dia - d2->dia);
    else if (d1->horas != d2->horas)
        return d1->horas - d2->horas;
    else
        return 0;
}

void cadastrar_times(Time *grupos, int *times_cadastrados, Chave *chaves)
{
    if (*times_cadastrados == MAX_TIMES) {
        mensagem("Você já cadastrou todos os times\n");
        return;
    }

    int n_disponiveis = MAX_TIMES - *times_cadastrados;
    int n_a_cadastrar;
    char input_str[5];

    printf("Digite quantos times deseja cadastrar, %d disponíveis: ", n_disponiveis);
    bold_yellow();
    get_line(input_str, 4, stdin);
    sscanf(input_str, "%d", &n_a_cadastrar);
    reset();
    if (n_a_cadastrar == 0)
        return;
    if (n_a_cadastrar > n_disponiveis) {
        mensagem("Não há tantos times disponíveis, retornando a sessão principal\n");
        return;
    }

    static char **lista = NULL;
    static int abertos = 0;

    if (abertos == 0) {
        printf("Deseja consultar a lista de times (S/n)?  ");
        bold_yellow();
        if (confirmar_resposta()) 
            if ((abertos = abrir_lista(&lista, MAX_NAME_LEN, "lista.txt")) == 0)
                puts("Lista não pôde ser aberta.");
    }
    reset();

    int i = 0;
    char buff[MAX_NAME_LEN + 1] = "";
    bool valido = true;

    while (i < n_a_cadastrar) {
        if (abertos > 0) {
            limpar_tela();
            exibir_lista_times(lista, grupos, *times_cadastrados, abertos);
            if (buff[0] != '\0' && valido)
                printf("\nVocê selecionou \e[1;33m%s\e[0m\n", buff);
            printf("\nDigite um dos ids acima ou dê um nome para o %d° time: ", *times_cadastrados + 1);
            bold_yellow();
            get_line(buff, MAX_NAME_LEN, stdin);
            reset();
            if (!obter_nome_por_id(buff, abertos, lista)) {
                valido = false;
                continue;
            }
        } else {
            limpar_tela();
            if (buff[0] != '\0')
                printf("\nVocê cadastrou \e[1;33m%s\e[0m\n", buff);
            printf("Digite um nome para o %d° time: ", *times_cadastrados + 1);
            bold_yellow();
            get_line(buff, MAX_NAME_LEN, stdin);
            reset();
        }
        if (strlen(buff) == 0)
            continue;
        toupper_all(buff);
        if (time_repetido(buff, grupos, *times_cadastrados)) {
            red();
            mensagem("Você já cadastrou esse time\n");
            continue;
        }
        strcpy(grupos[(*times_cadastrados)++].nome, buff);
        valido = true;
        ++i;
    }

    if (*times_cadastrados == MAX_TIMES) {
        randomizar_times_grupos(grupos, chaves);
        if (lista != NULL)
            for (int i = 0; i < abertos; ++i)
                free(lista[i]);
        free(lista);
    }

    sair_menu();
}

void exibir_lista_times(char **lista, const Time *times, int times_cadastrados, int abertos)
{
    for (int i = 0; i < abertos; ++i)
        if (!time_repetido(lista[i], times, times_cadastrados))
            printf("\e[1;32m%d\e[0m - \e[1;34m%s\e[0m\n", i, lista[i]);
}

Id obter_novo_id(void)
{
    static bool id_existe[MAX_IDS];
    int max_count = 0;

    for (;;) {
        Id novo_id = rand() % MAX_IDS;
        if (!id_existe[novo_id]) {
            id_existe[novo_id] = true;
            return novo_id;
        }
        if (++max_count == 1000)
            break;
    }
    for (Id id = 0; id < MAX_IDS; ++id)
        if (!id_existe[id]) {
            id_existe[id] = true;
            return id;
        }
}


bool time_repetido(const char *nome, const Time *times, int times_cadastrados)
{
    for (int i = 0; i < times_cadastrados; ++i)
        if ((strcmp(nome, times[i].nome)) == 0)
            return true;
    return false;
}

bool obter_nome_por_id(char *buff, size_t max, char **lista)
{
    Id id;

    if ((id = meu_atoi(buff)) == EOF)
        return true;
    if (id < 0 || id >= max) {
        red();
        mensagem("id inválido\n");
        return false;
    }
    strcpy(buff, lista[id]);
    return true;
}

bool confirmar_resposta(void)
{
    int op;

    while (isspace(op = getchar()))
        ;
    while (getchar() != '\n')
        ;
    return tolower(op) == 's';
}

void limpar_tela(void)
{
#ifdef WIN32
    system("cls");
#elif defined __linux__
    system("clear");
#endif
}

void mensagem(char *msg)
{
    limpar_tela();
    printf("%s", msg);
    bold_yellow();
    sleep(1);
    reset();
    limpar_tela();
}

void sair_menu(void)
{
    printf("\nDigite sair para voltar ao menu principal\n");
    bold_yellow();
    while (!confirmar_resposta())
        ;
    reset();
    limpar_tela();
}

void insertion_sort(Time *times, size_t size)
{
    for (int i = 0; i < size - 1; ++i) {
        int j = i;
        Time time = times[j + 1];

        while (j >= 0 && compara_ranque(&time, &times[j]) < 0) {
            times[j + 1] = times[j];
            --j;
        }
        times[j + 1] = time;
    }
}

int obter_gols(void)
{
    int gols = rand() % 20;
    if (gols > 10)
        gols = rand() % gols;
    if (gols > 6)
        gols = rand() % gols;
    if (gols > 3)
        gols = rand() % gols;
    return gols;
}

void easter_egg(Time *t1, Time *t2, int *gols_t1, int *gols_t2)
{
    static bool completed = false;

    if (strcmp(t1->nome, "Alemanha") == 0 && strcmp(t2->nome, "Brasil") == 0 && !completed) {
        *gols_t1 = 7;
        *gols_t2 = 1;
        completed = true;
    }
}
