#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura para armazenar um registro
typedef struct {
    long timestamp;
    char sensorID[20];
    char valor[30];
} Registro;

// Converte uma string no formato "dd/mm/yyyy-hh:mm:ss" para timestamp (long)
long converterParaTimestamp(const char *dataHora) {
    int d, m, y, h, min, s;
    if (sscanf(dataHora, "%d/%d/%d-%d:%d:%d", &d, &m, &y, &h, &min, &s) != 6)
        return -1;

    struct tm t = {0};
    t.tm_mday = d;
    t.tm_mon = m - 1;
    t.tm_year = y - 1900;
    t.tm_hour = h;
    t.tm_min = min;
    t.tm_sec = s;

    return mktime(&t);
}

// Gera uma string representando o valor aleatório de acordo com o tipo especificado
void gerarValorAleatorio(char *destino, const char *tipo) {
    if (strcmp(tipo, "int") == 0) {
        int numero = rand() % 1000;
        sprintf(destino, "%d", numero);
    } else if (strcmp(tipo, "float") == 0) {
        float numero = (float)(rand() % 10000) / 100.0f;
        sprintf(destino, "%.2f", numero);
    } else if (strcmp(tipo, "bool") == 0) {
        int b = rand() % 2;
        strcpy(destino, b ? "true" : "false");
    } else if (strcmp(tipo, "string") == 0) {
        int tam = 5 + rand() % 6;
        for (int i = 0; i < tam; i++) {
            destino[i] = 'a' + rand() % 26;
        }
        destino[tam] = '\0';
    } else {
        strcpy(destino, "tipo?");
    }
}

// Gera 2000 registros para um sensor com base no intervalo fornecido, escrevendo no arquivo já aberto
int gerarDadosSensor(FILE *saida, const char *nomeSensor, const char *tipo, long inicio, long fim) {
    for (int i = 0; i < 2000; i++) {
        Registro r;
        r.timestamp = inicio + rand() % (fim - inicio + 1);
        strncpy(r.sensorID, nomeSensor, sizeof(r.sensorID) - 1);
        r.sensorID[sizeof(r.sensorID) - 1] = '\0';
        gerarValorAleatorio(r.valor, tipo);

        fprintf(saida, "%ld %s %s\n", r.timestamp, r.sensorID, r.valor);
    }
    return 0;
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 5 || (argc - 3) % 2 != 0) {
        printf("Uso: %s <inicio> <fim> <sensor1> <tipo1> [<sensor2> <tipo2> ...]\n", argv[0]);
        return -1;
    }

    long inicio = converterParaTimestamp(argv[1]);
    long fim = converterParaTimestamp(argv[2]);

    if (inicio == -1 || fim == -1 || inicio >= fim) {
        printf("Datas inválidas. Use: dd/mm/yyyy-hh:mm:ss (e inicio < fim).\n");
        return -1;
    }

    srand(time(NULL));

    int totalSensores = (argc - 3) / 2;

    FILE *saida = fopen("dados_sensores.txt", "w");
    if (!saida) {
        printf("Erro ao criar o arquivo dados_sensores.txt\n");
        return -1;
    }

    for (int i = 0; i < totalSensores; i++) {
        const char *nome = argv[3 + i * 2];
        const char *tipoEntrada = argv[4 + i * 2];
        char tipoConvertido[10];

        if (strcmp(tipoEntrada, "CONJ_Z") == 0) {
            strcpy(tipoConvertido, "int");
        } else if (strcmp(tipoEntrada, "CONJ_Q") == 0) {
            strcpy(tipoConvertido, "float");
        } else if (strcmp(tipoEntrada, "TEXTO") == 0) {
            strcpy(tipoConvertido, "string");
        } else if (strcmp(tipoEntrada, "BINARIO") == 0) {
            strcpy(tipoConvertido, "bool");
        } else {
            printf("Tipo desconhecido: %s\n", tipoEntrada);
            fclose(saida);
            return -1;
        }

        if (gerarDadosSensor(saida, nome, tipoConvertido, inicio, fim) != 0) {
            printf("Erro ao gerar dados para o sensor '%s'\n", nome);
            fclose(saida);
            return -1;
        }
    }

    fclose(saida);
    printf("Arquivo 'dados_sensores.txt' gerado com sucesso!\n");
    return 0;
}
