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

int obter_time_grupos(Time *t[], Time *grupos, Chave *chaves, int jogos_realizados);
void obter_time_copa(Time *t[], Time *times, Fase fase_atual, Chave *chaves, int jogos_realizados);
void marcar_jogo(Time *times, Time *grupos, Info *copa, Jogo *jogos, Chave *chaves);
void swap_pelo_terceiro(Time *times);
int times_restantes(Fase fase_atual);
void parear_times_eliminatorias(Time *times, Chave *chaves, int *jogos_pareados);
int qsort_ranquear_grupos(const void *p, const void *q);
int compara_ranque(const Time *t1, const Time *t2);
void update_ranque(Id vencedor, Id perdedor, Time *times);
bool validar_data(Horario *data);
void randomizar_times_grupos(Time *grupos, Chave *chaves);
void visualizar_grupos(Jogo *jogos, const Info *copa, Time *grupos);
void visualizar_jogos_realizados(const Jogo *jogos, const Info *copa, Time *times_lista);
void visualizar_ranque(const Time *times, const Info *copa);
void marcar_jogo_dados(Info *copa, Jogo *jogos, Fase fase);
bool escolher_estadio(int jogos_realizados, char *jogo_atual_local);
void checar_transicao_proxima_fase(Time *times, Info *copa, Chave *chaves);
Resultado jogar_jogo(Time *t1, Time *t2, Fase fase);
Time *pesquisar_time_ptr(Time *times, Id id, size_t size);
void set_resultado(Time *vencedor, Time *perdedor, Resultado *resultado,
                   int gols_vencedor, int gols_perdedor, Fase fase);
Fase calcular_fase(int jogo_numero);
int comparar_data(const Horario *d1, const Horario *d2);
void cadastrar_times(Time *grupos, int *times_cadastrados, Chave *chaves);
void exibir_lista_times(char lista[][MAX_NAME_LEN + 1], const Time *times,
                        int times_cadastrados, int abertos);
Id obter_novo_id(void);
bool time_repetido(const char *nome, const Time *times, int times_cadastrados);
bool obter_nome_por_id(char *buff, size_t max, const char lista[][MAX_NAME_LEN + 1]);
bool confirmar_resposta(void);
void limpar_tela(void);
void mensagem(char *msg);

int
main(void)
{
	setlocale(LC_ALL, "Portuguese");

	Time times[MAX_RANQUE_TIMES];
    	Time grupos[MAX_TIMES];
	Chave chaves[MAX_JOGOS];

    	Info copa = {0, 0, 0, {1, 1, 12, 30}};
    	Jogo jogos[MAX_JOGOS];

    	srand((unsigned)time(NULL));
    	limpar_tela();

    	for (int i = 0; i < MAX_TIMES; ++i) 
	    	grupos[i] = (Time){PENDENTE, "", obter_novo_id(), 0, 0, 0, 0, 0, 0};

	for (int i = 0; i < MAX_RANQUE_TIMES; ++i) 
		times[i] = (Time){PENDENTE, "", 0, 0, 0, 0, 0, 0, 0};

   	puts("------------------------------------------------------------------------------------------------------------------------------");
	puts("                                                           Copa do Mundo");
	puts("------------------------------------------------------------------------------------------------------------------------------");

	 for (;;) {
		puts("1 - Cadastrar times");
	    	puts("2 - Visualizar ranque");
    		puts("3 - Visualizar jogos");
		puts("4 - Visualizar Grupos");
	    	puts("5 - Marcar jogo");
    		puts("6 - Sair do programa");
	
		char input_str[3];
		int op;

		fgets_(input_str, 3, stdin);
		sscanf(input_str, "%d", &op);
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
			marcar_jogo(times, grupos, &copa, jogos, chaves);
			break;
		case 6:
			exit(EXIT_SUCCESS);
			break;
		default:
			puts("\nOpção inválida");
			break;
		}
	 }
}


int 
obter_time_grupos(Time *t[], Time *grupos, Chave *chaves, int jogos_realizados)
{
	t[0] = pesquisar_time_ptr(grupos, chaves[jogos_realizados].t1, MAX_TIMES);
	t[1] = pesquisar_time_ptr(grupos, chaves[jogos_realizados].t2, MAX_TIMES);

	int grupo  = t[0] - grupos;
	while (grupo % MAX_TIMES_GRUPO != 0)
		--grupo;

	return grupo;
}

int
times_restantes(Fase fase_atual)
{
	int times_restantes = MAX_RANQUE_TIMES;

	while (fase_atual > OITAVAS) {
		--fase_atual;
		times_restantes /= 2;
	}
	return times_restantes;
}

void
obter_time_copa(Time *t[], Time *times, Fase fase_atual, Chave *chaves, int jogos_realizados) 
{
	int jogos_eliminatorias = jogos_realizados - MAX_JOGOS_GRUPOS;

	t[0] = pesquisar_time_ptr(times, chaves[jogos_eliminatorias].t1, MAX_RANQUE_TIMES);
	t[1] = pesquisar_time_ptr(times, chaves[jogos_eliminatorias].t2, MAX_RANQUE_TIMES);
}

void
marcar_jogo(Time *times, Time *grupos, Info *copa, Jogo *jogos, Chave *chaves)
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
		grupo_atual = obter_time_grupos(t, grupos, chaves, copa->jogos_realizados);
		jogos[copa->jogos_realizados].resultado = jogar_jogo(t[0], t[1], fase_atual);
		qsort(&grupos[grupo_atual], MAX_TIMES_GRUPO, sizeof(grupos[0]), qsort_ranquear_grupos);
		if (copa->jogos_realizados >= MAX_JOGOS_GRUPOS - MAX_RANQUE_TIMES / 2) {
			times[copa->times_classificados].id = grupos[grupo_atual].id;
			times[copa->times_classificados + 1].id = grupos[grupo_atual + 1].id;
			strcpy(times[copa->times_classificados].nome, grupos[grupo_atual].nome);
			strcpy(times[copa->times_classificados + 1].nome, grupos[grupo_atual + 1].nome);
			copa->times_classificados += 2;
		}
		if (copa->jogos_realizados == MAX_JOGOS_GRUPOS - 1) {
			parear_times_eliminatorias(times, chaves, &copa->jogos_pareados);
		}
	} else { 
		obter_time_copa(t, times, fase_atual, chaves, copa->jogos_realizados);
		jogos[copa->jogos_realizados].resultado = jogar_jogo(t[0], t[1], fase_atual);
		if (fase_atual == PELO_TERCEIRO) {
			swap_pelo_terceiro(times);
		} else {
			update_ranque(jogos[copa->jogos_realizados].resultado.vencedor,
			              jogos[copa->jogos_realizados].resultado.perdedor,
				      times);
		}
		if (fase_atual == FINAL) {
			times[TERCEIRO].status = times[QUARTO].status = PENDENTE;
			chaves[copa->jogos_pareados].t1 = times[TERCEIRO].id;
			chaves[copa->jogos_pareados++].t2 = times[QUARTO].id;
		}
	}

	marcar_jogo_dados(copa, jogos, fase_atual);
	checar_transicao_proxima_fase(times, copa, chaves);
	copa->jogos_realizados++;
}

void
update_ranque(Id vencedor, Id perdedor, Time *times)
{
	Time *pivot = pesquisar_time_ptr(times, vencedor, MAX_RANQUE_TIMES) - 1;
	Time t = *(pivot + 1);

	while (pivot >= &times[0] && compara_ranque(&t, pivot) < 0) {
		*(pivot + 1) = *pivot;
		--pivot;
	}
	*(pivot + 1) = t;

	pivot = pesquisar_time_ptr(times, perdedor, MAX_RANQUE_TIMES) + 1;
	t = *(pivot - 1);

	while (pivot < &times[MAX_RANQUE_TIMES] && compara_ranque(&t, pivot) > 0) {
		*(pivot - 1) = *pivot;
		++pivot;
	}
	*(pivot - 1) = t;
}

int 
compara_ranque(const Time *t1, const Time *t2)
{
	if (t2->status != t1->status)
		return t2->status - t1->status;
	if (t1->vitorias != t2->vitorias)
		return t2->vitorias - t1->vitorias;
	else 
		return t1->derrotas - t2->derrotas;
}

void
swap_pelo_terceiro(Time *times)
{
	if (times[QUARTO].vitorias > times[TERCEIRO].vitorias) {
		Time tmp = times[QUARTO];
		times[QUARTO] = times[TERCEIRO];
		times[TERCEIRO] = tmp;
	}
}

void 
parear_times_eliminatorias(Time *times, Chave *chaves, int *jogos_pareados)
{
	int j = 0;
	Time tmp[MAX_RANQUE_TIMES];
	int chave = 0;

	for (int i = 0; i < MAX_RANQUE_TIMES; ++i) 
		tmp[i] = times[i];
	
	for (int i = 0; i < MAX_RANQUE_TIMES / 2; i++) {
		times[i] = tmp[j++];
		times[i + MAX_RANQUE_TIMES / 2] = tmp[j++];
	}

	for (int i = 0; i < MAX_RANQUE_TIMES; i += 2) {
		chaves[chave].t1 = times[i].id;
		chaves[chave].t2 = times[i + 1].id;
		++chave;
	}
	*jogos_pareados = chave;
}

int
qsort_ranquear_grupos(const void *p, const void *q)
{
	const Time *t1 = p;
	const Time *t2 = q;

	if (t1->pontos != t2->pontos)
		return t2->pontos - t1->pontos;
	else
		return (t2->gols - t2->gols_sofridos) - (t1->gols - t2->gols_sofridos);
}

bool
validar_data(Horario *data)
{
	const int dias[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
	return data->horas >= 0 && data->horas < 24  && data->minutos >= 0 && data->minutos < 60
	&& data->mes >= 1 && data->mes <= 12 && data->dia >= 1 && data->dia <= dias[data->mes - 1];
}


void
randomizar_times_grupos(Time *grupos, Chave *chaves)
{
	for (int i = 0; i < MAX_TIMES; ++i) {
		int j = rand() % MAX_TIMES;
		Time tmp = grupos[i];
		grupos[i] = grupos[j];
		grupos[j] = tmp;
	}
	
	int chave = 0;

	for (int i = 0; i < MAX_TIMES_GRUPO - 1; ++i) {
		for (int j = i + 1; j < MAX_TIMES_GRUPO; ++j) {
			for (int k = 0; k < MAX_TIMES; k += MAX_TIMES_GRUPO) {
				chaves[chave].t1 = grupos[k + i].id;
				chaves[chave].t2 = grupos[k + j].id;
				++chave;
			}
		}
	}
}

void
visualizar_grupos(Jogo *jogos, const Info *copa, Time *grupos)
{
	char grupo_letra = 'A';

	for (int i = 0; i < NUM_GRUPOS; i++) {
		int grupo = i * MAX_TIMES_GRUPO;

		puts("-----------------------------------------------------------------------------------");
		printf("                                    Grupo %c\n", grupo_letra++);
		puts("-----------------------------------------------------------------------------------");
		puts("Id   Time                  Vitórias  Derrotas  Empates  Gols  Gols sofridos  Pontos\n");
		for (int j = grupo; j < grupo + MAX_TIMES_GRUPO && j < copa->times_cadastrados; ++j) {
			printf("%-2d   %-24s %-2d       %-2d         %-2d     %-2d       %-2d           %-2d\n",
			       grupos[j].id, grupos[j].nome, grupos[j].vitorias, grupos[j].derrotas,
			       grupos[j].empates, grupos[j].gols, grupos[j].gols_sofridos, 
			       grupos[j].pontos);
		}
	}

	puts("Digite sair para voltar ao menu principal");
	while (!confirmar_resposta())
		continue;
	limpar_tela();
}

void
visualizar_ranque(const Time *times, const Info *copa)
{
	const char *STATUS[] = {"Eliminado", "Pendente", "Passou", "Campeão"};
	const char *FASES_NOMES[MAX_FASES + 1] = {"Grupos", "Oitavas de final", "Quartas de final",
	                                          "Semi Final", "Final", "Competição pelo terceiro lugar", "Fim da copa"};

	puts("------------------------------------------------------------------------------------------------------------------------------");
	puts("                                                           Ranque da Copa");
	puts("------------------------------------------------------------------------------------------------------------------------------");
	printf("                                                          %s\n", FASES_NOMES[calcular_fase(copa->jogos_realizados)]);
	puts("------------------------------------------------------------------------------------------------------------------------------");

	printf("        ID      Times                           Status                "
	"Vitórias  Derrotas Gols  Gols sofridos"); 
	printf("\n");
	for (int i = 0; i < copa->times_classificados; ++i) {
		printf("%2d°     %-2d     %-24s       %-15s            %-2d        "
		"%-2d     %-2d        %-2d \n", i + 1, times[i].id,
		times[i].nome, STATUS[times[i].status], times[i].vitorias, times[i].derrotas,
		times[i].gols, times[i].gols_sofridos);
	}
	if (copa->times_classificados == 0)
		puts("\n                                                "
		     "Nenhum time foi classificado para as oitavas ainda");

	printf("Digite sair para voltar ao menu principal:  ");
	while (!confirmar_resposta())
		continue;
	limpar_tela();
}

void
visualizar_jogos_realizados(const Jogo *jogos, const Info *copa, Time *times_lista)
{
	const char *FASES_NOMES[MAX_FASES] = {"Grupos", "Oivatas", "Quartas", "Semi Final", "Final", "Pelo 3°"};

	puts("   ID      Fase        Dia      Horario                 Vencedor  Placar  Perdedor                 Estádio             Penalty");

	for (int i = 0; i < copa->jogos_realizados; ++i) {
		if ((pesquisar_time_ptr(times_lista, jogos[i].resultado.vencedor, MAX_TIMES)) == NULL) {
			puts("\n\n\nerror");
			exit(1);
		}
		if ((pesquisar_time_ptr(times_lista, jogos[i].resultado.perdedor, MAX_TIMES)) == NULL) {
			puts("\n\n\nerror do 2");
			exit(1);
		}
		printf("\n   %-2d     %-11s %.2d/%.2d     %.2d:%.2d   %24.24s %2d x %-2d  %-24.24s %-18.18s",
				jogos[i].id, FASES_NOMES[jogos[i].fase], jogos[i].data.dia,
				jogos[i].data.mes, jogos[i].data.horas, jogos[i].data.minutos,
				pesquisar_time_ptr(times_lista, jogos[i].resultado.vencedor, MAX_TIMES)->nome,
				jogos[i].resultado.placar.vencedor, jogos[i].resultado.placar.perdedor,
				pesquisar_time_ptr(times_lista, jogos[i].resultado.perdedor, MAX_TIMES)->nome,
				jogos[i].local);

		if (jogos[i].resultado.estagio >= PENALTY)
			printf("  Sim");
		if (jogos[i].resultado.empate)
			printf("  Empate");
	}
	if (copa->jogos_realizados == 0)
		puts("\n                                                        Nenhum jogo foi Realizado ainda");

	printf("\nDigite sair para voltar ao menu principal:  ");
	while (!confirmar_resposta())
		continue;
	limpar_tela();
}
void
marcar_jogo_dados(Info *copa, Jogo *jogos, Fase fase)
{
	Jogo *jogo_atual = &jogos[copa->jogos_realizados];
	Horario data;

	#ifdef DEBUG
		data = copa->ultima_data;
		data.dia++;
		strcpy(jogos[copa->jogos_realizados].local, "debug");
		goto fora;
	#endif

	for (;;) {
		char input_str[10];

		printf("Escolha a data em que deseja fazer o jogo (dd/mm): ");
		fgets_(input_str, 9, stdin);
		sscanf(input_str,"%d /%d", &data.dia, &data.mes);
		printf("Escolha o horário em que o jogo aconteceu (hh:mm): ");
		fgets_(input_str, 9, stdin);
		sscanf(input_str, "%2d :%2d", &data.horas, &data.minutos);
		if (!validar_data(&data)) {
			mensagem("Essa data não é válida\n");
			continue;
		}

		int horas_diferenca = comparar_data(&copa->ultima_data, &data);

		if ((horas_diferenca >= 0 || abs(horas_diferenca) <= 12)
		    && copa->jogos_realizados > 0) {
			mensagem("Essa data já passou ou está muito próxima do último jogo\n");
			continue;
		}
		break;
	}

	while (!escolher_estadio(copa->jogos_realizados, jogo_atual->local))
			continue;
	fora:

	copa->ultima_data = data;
	jogo_atual->data = data;
	jogo_atual->fase= fase;
	jogo_atual->id = obter_novo_id();
	limpar_tela();
}

bool
escolher_estadio(int jogos_realizados, char *jogo_atual_local)
{
	char buff[MAX_NAME_LEN + 1];
	Id id;
	
	char estadios[MAX_LISTA_LEN][MAX_NAME_LEN + 1];
	int num_estadios = abrir_lista(estadios, MAX_LISTA_LEN, "estadios.txt");

	for (int i = 0; i < num_estadios; ++i)
		printf("%d - %s\n", i, estadios[i]);

	if (num_estadios > 0) {
		printf("Digite um nome de estádio ou digite um dos ids acima: ");
		fgets_(buff, MAX_NAME_LEN, stdin);
		if (!obter_nome_por_id(buff, num_estadios, estadios))
			return false;
	} else {
		printf("Digite o nome do estádio: ");
		fgets_(buff, MAX_NAME_LEN, stdin);
	}

	strcpy(jogo_atual_local, buff);
	return true;
}

void
checar_transicao_proxima_fase(Time *times, Info *copa, Chave *chaves)
{
	for (int i = 0; i < MAX_RANQUE_TIMES; ++i)
		if (times[i].status == PENDENTE)
			return;

	for (int i = 0; i < MAX_RANQUE_TIMES; ++i)
		if (times[i].status == PASSOU)
			times[i].status = PENDENTE;
	
	for (int i = 0; times[i].status == PENDENTE; i += 2) {
		chaves[copa->jogos_pareados].t1 = times[i].id;
		chaves[copa->jogos_pareados].t2 = times[i + 1].id;
		++copa->jogos_pareados;
	}
}

Resultado
jogar_jogo(Time *t1, Time *t2, Fase fase)
{
	Resultado resultado;
	Id id_vencedor = EOF;
	char input_str[4];
	unsigned t1_gols;
	unsigned t2_gols;

	#ifdef DEBUG 
	t1_gols = rand() % 5;
	t2_gols = rand() % 5;
	if (t1_gols == t2_gols)
		t1_gols++;
	printf("Time %s vs %s", t1->nome, t2->nome);
	goto fora2;
	#endif

	printf("Digite quantos gols o time %s fez: ", t1->nome);
	fgets_(input_str, 3, stdin);
	sscanf(input_str, "%d", &t1_gols);
	printf("Digite quantos gols o time %s fez: ", t2->nome);
	fgets_(input_str, 3, stdin);
	sscanf(input_str, "%d", &t2_gols);

	fora2:

	resultado.estagio = NORMAL;
	resultado.empate = false;
	t1->gols += t1_gols;
	t2->gols += t2_gols;
	t1->gols_sofridos += t2_gols;
	t2->gols_sofridos += t1_gols;

	if (t1_gols > t2_gols)
		id_vencedor = t1->id;
	else if (t2_gols > t1_gols)
		id_vencedor = t2->id;
	else if (fase != GRUPOS) {
		char buff_str[MAX_NAME_LEN + 1];
		resultado.estagio = PENALTY;
		printf("Os times empataram, digite quem foi o vencedor dos penaltys: %s ou %s:\n", t1->nome, t2->nome);
		for (;;) {
			fgets_(buff_str, MAX_NAME_LEN, stdin);
			if (strcmp(t1->nome, buff_str) == 0)
				id_vencedor = t1->id;
			else if (strcmp(t2->nome, buff_str) == 0)
				id_vencedor = t2->id;
			else {
				printf("O nome que você digitou não é válido, digite denovo: %s ou %s\n", t1->nome, t2->nome);
				continue;
			}
			break;
		}
	}


	if (id_vencedor == t1->id) {
		set_resultado(t1, t2, &resultado, t1_gols, t2_gols, fase);
	} else if (id_vencedor == t2->id) {
		set_resultado(t2, t1, &resultado, t2_gols, t1_gols, fase);
	} else {
		++t1->pontos;
		++t2->pontos;
		++t1->empates;
		++t2->empates;
		resultado.empate = true;
		resultado.vencedor = t1->id;
		resultado.perdedor = t2->id;
		resultado.placar.vencedor = t1_gols;
		resultado.placar.perdedor = t2_gols;
	}

	return resultado;
}


Time *pesquisar_time_ptr(Time *times, Id id, size_t max)
{
	for (int i = 0; i < max; ++i) 
		if (times[i].id == id)
			return &times[i];
	return NULL;
}

void
set_resultado(Time *vencedor, Time *perdedor, Resultado *resultado, int gols_vencedor, int gols_perdedor, Fase fase)
{
	resultado->vencedor = vencedor->id;
	resultado->perdedor = perdedor->id;
	resultado->placar.vencedor = gols_vencedor;
	resultado->placar.perdedor = gols_perdedor;
	if (fase != GRUPOS) {
		if (fase == FINAL) {
			vencedor->status = CAMPEAO;
		} else if (fase == PELO_TERCEIRO) {
			vencedor->status = ELIMINADO;
		} else {
			vencedor->status = PASSOU;
		}
		perdedor->status = ELIMINADO;
	}
	++vencedor->vitorias;
	++perdedor->derrotas;
	vencedor->pontos += 3;
}

Fase
calcular_fase(int jogo_numero)
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


int
comparar_data(const Horario *d1, const Horario *d2)
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

void
cadastrar_times(Time *grupos, int *times_cadastrados, Chave *chaves)
{
	if (*times_cadastrados == MAX_TIMES) {
		mensagem("Você já cadastrou todos os times\n");
		return;
	 }

	int n_disponiveis = MAX_TIMES - *times_cadastrados;
  	int n_a_cadastrar;
  	char input_str[5];

  	printf("Digite quantos times deseja cadastrar, %d disponíveis: ", n_disponiveis);
  	fgets_(input_str, 4, stdin);
	sscanf(input_str, "%d", &n_a_cadastrar);
  	if (n_a_cadastrar == 0)
  		return;
  	if (n_a_cadastrar > n_disponiveis) {
		mensagem("Não há tantos times disponíveis, retornando a sessão principal\n");
  		return;
  	}

  	char lista[MAX_LISTA_LEN][MAX_NAME_LEN + 1];
  	int abertos = 0;

  	printf("Deseja consultar a lista de times (S/n)?  ");
  	if (confirmar_resposta()) {
  		if ((abertos = abrir_lista(lista, MAX_LISTA_LEN, "lista.txt")) == 0)
  			puts("Lista não pôde ser aberta.");
  	}

  	for (int i = 0; i < n_a_cadastrar; ++i) {
  		char buff[MAX_NAME_LEN + 1];

  		if (abertos > 0) {
  			limpar_tela();
  			exibir_lista_times(lista, grupos, *times_cadastrados, abertos);
  			if (i > 0)
  				printf("\nVoc? selecionou %s\n", buff);
  			printf("\nDigite um dos ids acima ou dê um nome para o %d° time: ", *times_cadastrados + 1);
  			fgets_(buff, MAX_NAME_LEN, stdin);
  			if (!obter_nome_por_id(buff, abertos, lista))
  				continue;
  		} else {
  			printf("Digite um nome para o %d° time: ", *times_cadastrados + 1);
  			fgets_(buff, MAX_NAME_LEN, stdin);
  		}
  		if (time_repetido(buff, grupos, *times_cadastrados)) {
			mensagem("Você já cadastrou esse time, retornando a sessão principal\n");
  			return;
  		}
  		strcpy(grupos[(*times_cadastrados)++].nome, buff);
  	}

  	if (*times_cadastrados == MAX_TIMES)
  		randomizar_times_grupos(grupos, chaves);

  	limpar_tela();
}

void
exibir_lista_times(char lista[][MAX_NAME_LEN + 1], const Time *times, int times_cadastrados, int abertos)
{
	for (int i = 0; i < abertos; ++i)
		if (!time_repetido(lista[i], times, times_cadastrados))
			printf("%d - %s\n", i, lista[i]);
}

Id
obter_novo_id(void)
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


bool
time_repetido(const char *nome, const Time *times, int times_cadastrados)
{
	for (int i = 0; i < times_cadastrados; ++i)
		if ((strcmp(nome, times[i].nome)) == 0)
			return true;
	return false;
}

bool
obter_nome_por_id(char *buff, size_t  max, const char lista[][MAX_NAME_LEN + 1])
{
	Id id;

	if ((id = meu_atoi(buff)) == EOF)
		return true;
	if (id < 0 || id >= max) {
		mensagem("id inválido\n");
		return false;
	}
	strcpy(buff, lista[id]);
	return true;
}

bool
confirmar_resposta(void)
{
	int op;
	while (isspace(op = getchar()))
		continue;
	while (getchar() != '\n')
		continue;
	if (tolower(op) == 's')
		return true;
	else
		return false;
}

void
limpar_tela(void)
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
	sleep(1);
	limpar_tela();
}
