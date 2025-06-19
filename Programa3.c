#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    long timestamp;
    char sensorID[20];
    char valor[30];
} Registro;

long converterParaTimestamp(const char *dataHora) {
    if (dataHora == NULL) {
        printf("Erro: Data/hora nula\n");
        return -1;
    }

    int d, m, y, h, min, s;
    if (sscanf(dataHora, "%d/%d/%d-%d:%d:%d", &d, &m, &y, &h, &min, &s) != 6) {
        printf("Erro: Formato de data/hora inválido. Use dd/mm/yyyy-hh:mm:ss\n");
        return -1;
    }

    if (d < 1 || d > 31 || m < 1 || m > 12 || y < 1900 || 
        h < 0 || h > 23 || min < 0 || min > 59 || s < 0 || s > 59) {
        printf("Erro: Valores de data/hora fora do intervalo permitido\n");
        return -1;
    }

    struct tm t = {0};
    t.tm_mday = d;
    t.tm_mon = m - 1;
    t.tm_year = y - 1900;
    t.tm_hour = h;
    t.tm_min = min;
    t.tm_sec = s;
    t.tm_isdst = -1;

    time_t ts = mktime(&t);
    if (ts == -1) {
        printf("Erro na conversão de data/hora\n");
        return -1;
    }

    return ts;
}

void gerarValorAleatorio(char *destino, const char *tipo) {
    if (destino == NULL || tipo == NULL) {
        printf("Erro: Parâmetros nulos em gerarValorAleatorio\n");
        return;
    }

    if (strcmp(tipo, "CONJ_Z") == 0) {
        snprintf(destino, 30, "%d", rand() % 1000);
    } 
    else if (strcmp(tipo, "CONJ_Q") == 0) {
        snprintf(destino, 30, "%.2f", (float)(rand() % 10000) / 100.0f);
    } 
    else if (strcmp(tipo, "BINARIO") == 0) {
        strncpy(destino, (rand() % 2) ? "true" : "false", 30);
    } 
    else if (strcmp(tipo, "TEXTO") == 0) {
        int tam = 5 + rand() % 6;
        for (int i = 0; i < tam && i < 30; i++) {
            destino[i] = 'a' + rand() % 26;
        }
        destino[tam < 30 ? tam : 29] = '\0';
    } 
    else {
        strncpy(destino, "tipo_invalido", 30);
        printf("Erro: Tipo de dado desconhecido: %s\n", tipo);
    }
}

int gerarDadosSensor(FILE *saida, const char *nomeSensor, const char *tipo, long inicio, long fim) {
    if (saida == NULL || nomeSensor == NULL || tipo == NULL) {
        printf("Erro: Parâmetros nulos em gerarDadosSensor\n");
        return -1;
    }

    if (inicio >= fim) {
        printf("Erro: Intervalo de tempo inválido\n");
        return -1;
    }

    Registro r;
    strncpy(r.sensorID, nomeSensor, 20);
    r.sensorID[19] = '\0';

    for (int i = 0; i < 2000; i++) {
        r.timestamp = inicio + (rand() % (fim - inicio + 1));
        gerarValorAleatorio(r.valor, tipo);
        
        if (fprintf(saida, "%ld %s %s\n", r.timestamp, r.sensorID, r.valor) < 0) {
            printf("Erro ao escrever no arquivo\n");
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 5 || (argc - 3) % 2 != 0) {
        printf("Uso: %s <inicio> <fim> <sensor1> <tipo1> [<sensor2> <tipo2> ...]\n", argv[0]);
        printf("Tipos: CONJ_Z (inteiro), CONJ_Q (float), TEXTO (string), BINARIO (booleano)\n");
        printf("Maximo 10 sensores\n");
        return 1;
    }

    int totalSensores = (argc - 3) / 2;
    if (totalSensores > 10) {
        printf("Erro: Maximo de 10 sensores\n");
        return 1;
    }

    long inicio = converterParaTimestamp(argv[1]);
    long fim = converterParaTimestamp(argv[2]);
    if (inicio == -1 || fim == -1 || inicio >= fim) {
        printf("Erro: Datas invalidas\n");
        return 1;
    }

    srand(time(NULL));

    FILE *saida = fopen("dados_sensores.txt", "w");
    if (saida == NULL) {
        printf("Erro ao criar arquivo\n");
        return 1;
    }

    for (int i = 0; i < totalSensores; i++) {
        const char *nome = argv[3 + i * 2];
        const char *tipo = argv[4 + i * 2];

        if (strcmp(tipo, "CONJ_Z") != 0 && 
            strcmp(tipo, "CONJ_Q") != 0 && 
            strcmp(tipo, "TEXTO") != 0 && 
            strcmp(tipo, "BINARIO") != 0) {
            printf("Erro: Tipo desconhecido: %s\n", tipo);
            fclose(saida);
            return 1;
        }

        if (gerarDadosSensor(saida, nome, tipo, inicio, fim) != 0) {
            printf("Erro ao gerar dados para %s\n", nome);
            fclose(saida);
            return 1;
        }
    }

    fclose(saida);
    printf("Arquivo gerado com sucesso!\n");
    printf("Sensores: %d, Registros: 2000 cada\n", totalSensores);
    printf("Periodo: %ld a %ld\n", inicio, fim);

    return 0;
}
