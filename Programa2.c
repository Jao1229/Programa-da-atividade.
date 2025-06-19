#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definições das estruturas
typedef struct {
    long timestamp;
    char sensor[10];
    char medicao[17];
} Registro;

typedef struct {
    int dia, mes, ano, hora, minuto, segundo;
} RegistroHora;
// Protótipos de funções
int contarLinhas(char *nomeArquivo);
int carregarRegistros(Registro *dados, int total, char *nomeArquivo);
int buscaBinaria(Registro *dados, long alvo, int tamanho);
long converterParaTimestamp(char *dataHora);
int decomporTimestamp(Registro *dados, RegistroHora *horas, int total);
void exibirResultado(int indice, long alvo, Registro *dados, RegistroHora *horas, int tamanho);

// Função principal
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso incorreto! Informe: <arquivo_sensor> <dd/mm/yyyy-hh:mm:ss>\n");
        return -1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", argv[1]);
        return -1;
    }
    fclose(file);

    if (strlen(argv[2]) != 19) {
        printf("Formato da data/hora inválido! Use: dd/mm/yyyy-hh:mm:ss\n");
        return -1;
    }

    int total = contarLinhas(argv[1]);
    if (total <= 0) {
        printf("Arquivo vazio ou erro na contagem de linhas.\n");
        return -1;
    }

    Registro *dados = malloc(total * sizeof(Registro));
    RegistroHora *horas = malloc(total * sizeof(RegistroHora));

    if (carregarRegistros(dados, total, argv[1]) != 0) {
        printf("Erro ao carregar registros.\n");
        free(dados);
        free(horas);
        return -1;
    }

    decomporTimestamp(dados, horas, total);

    long alvo = converterParaTimestamp(argv[2]);
    if (alvo == -1) {
        printf("Erro ao converter data/hora.\n");
        free(dados);
        free(horas);
        return -1;
    }

    int indice = buscaBinaria(dados, alvo, total);
    exibirResultado(indice, alvo, dados, horas, total);

    free(dados);
    free(horas);
    return 0;
}

// Conta linhas do arquivo
int contarLinhas(char *nomeArquivo) {

    if (nomeArquivo == NULL)
    {
         printf("Erro ao carregar o nome do arquivo\n");
        return -1;
    }
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) return -1;

    int count = 0;
    char linha[100];
    while (fgets(linha, sizeof(linha), f)) count++;
    fclose(f);
    return count;
}

// Carrega dados para a struct
int carregarRegistros(Registro *dados, int total, char *nomeArquivo) {

      if (dados == NULL)
    {
        printf("Erro ao carregar o registro\n");
        return -1;
    }

    if (nomeArquivo == NULL)
    {
         printf("Erro ao carregar o nome do arquivo\n");
        return -1;
    }

    FILE *f = fopen(nomeArquivo, "r");
    if (!f) return -1;

    for (int i = 0; i < total; i++) {
        char linha[100];
        if (!fgets(linha, sizeof(linha), f)) break;

        char *ts = strtok(linha, " ");
        char *sensor = strtok(NULL, " ");
        char *med = strtok(NULL, "\n");

        dados[i].timestamp = strtol(ts, NULL, 10);
        strncpy(dados[i].sensor, sensor, sizeof(dados[i].sensor));
        strncpy(dados[i].medicao, med, sizeof(dados[i].medicao));
    }

    fclose(f);
    return 0;
}

// Busca binária
int buscaBinaria(Registro *dados, long alvo, int tamanho) {

      if (dados == NULL)
    {
        printf("Erro ao carregar o registro\n");
        return -1;
    }

    int inicio = 0, fim = tamanho - 1;

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        if (dados[meio].timestamp == alvo) return meio;
        else if (dados[meio].timestamp < alvo) fim = meio - 1;
        else inicio = meio + 1;
    }

    return -1; 
}

// Conversão de string para timestamp
long converterParaTimestamp(char *entrada) {
      if (entrada == NULL)
    {
        printf("Erro ao carregar a Data e Hora\n");
        return -1;
    }

    int d, m, y, h, min, s;
    if (sscanf(entrada, "%d/%d/%d-%d:%d:%d", &d, &m, &y, &h, &min, &s) != 6)
        return -1;

    struct tm t = {0};
    t.tm_mday = d; t.tm_mon = m - 1; t.tm_year = y - 1900;
    t.tm_hour = h; t.tm_min = min; t.tm_sec = s;

    return mktime(&t);
}

// Converte os timestamps da struct para data/hora legível
int decomporTimestamp(Registro *dados, RegistroHora *horas, int total) {

      if (dados == NULL)
    {
        printf("Erro ao carregar a estrutura de registro\n");
        return -1;
    }

    if (horas == NULL)
    {
         printf("Erro ao carregar a estrutura de Hora\n");
        return -1;
    }

    for (int i = 0; i < total; i++) {
        time_t temp = dados[i].timestamp;
        struct tm *info = localtime(&temp);
        horas[i].ano = 1900 + info->tm_year;
        horas[i].mes = 1 + info->tm_mon;
        horas[i].dia = info->tm_mday;
        horas[i].hora = info->tm_hour;
        horas[i].minuto = info->tm_min;
        horas[i].segundo = info->tm_sec;
    }
    return 0;
}

// Exibe o resultado da busca
void exibirResultado(int indice, long alvo, Registro *dados, RegistroHora *horas, int tamanho) {

    if (indice >= 0) {
        printf("Registro exato encontrado:\n");
        printf("%02d/%02d/%04d %02d:%02d:%02d %s %s\n",
            horas[indice].dia, horas[indice].mes, horas[indice].ano,
            horas[indice].hora, horas[indice].minuto, horas[indice].segundo,
            dados[indice].sensor, dados[indice].medicao);
    } else {
        printf("Registro não encontrado. Exibindo o mais próximo:\n");
        int maisProx = 0;
        long minDif = labs(alvo - dados[0].timestamp);
        for (int i = 1; i < tamanho; i++) {
            long dif = labs(alvo - dados[i].timestamp);
            if (dif < minDif) {
                minDif = dif;
                maisProx = i;
            }
        }
        printf("%02d/%02d/%04d %02d:%02d:%02d %s %s\n",
            horas[maisProx].dia, horas[maisProx].mes, horas[maisProx].ano,
            horas[maisProx].hora, horas[maisProx].minuto, horas[maisProx].segundo,
            dados[maisProx].sensor, dados[maisProx].medicao);
    }
}
