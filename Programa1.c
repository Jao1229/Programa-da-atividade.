#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    long timestamp;
    char sensorID[20];
    char valor[30];
} Registro;

int contarLinhasArquivo(char *argv[]);
int carregarRegistros(Registro *registros, int total, char *nomeArquivo);
int separarSensores(Registro *registros, int total);
int salvarArquivoSensor(Registro *lista, int total, const char *nomeSensor);
void ordenarPorTempo(Registro *lista, int total);
int arquivoExiste(const char *nomeArquivo);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return -1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Arquivo invalido ou nao encontrado!\nPrograma encerrado.\n");
        return -1;
    }
    fclose(file);

    printf("Processando arquivo: %s\n", argv[1]);

    int cont = contarLinhasArquivo(argv);
    if (cont == -1) {
        printf("Erro ao contar as linhas do arquivo!\nPrograma encerrado.\n");
        return -1;
    }
    if (cont == 0) {
        printf("Arquivo vazio!\nPrograma encerrado.\n");
        return -1;
    }

    printf("Quantidade de linhas: %d\n", cont);

    Registro *dados = malloc(cont * sizeof(Registro));
    if (dados == NULL) {
        printf("Erro ao alocar memoria!\nPrograma encerrado.\n");
        return -1;
    }

    if (carregarRegistros(dados, cont, argv[1]) != 0) {
        printf("Erro ao carregar os dados!\nPrograma encerrado.\n");
        free(dados);
        return -1;
    }

    if (separarSensores(dados, cont) != 0) {
        printf("Erro ao processar os dados por sensor!\nPrograma encerrado.\n");
        free(dados);
        return -1;
    }

    free(dados);
    return 0;
}

int contarLinhasArquivo(char *argv[]) {
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) return -1;
    int cont = 0;
    char linha[128];
    while (fgets(linha, sizeof(linha), file)) ++cont;
    fclose(file);
    return cont;
}

int carregarRegistros(Registro *registros, int total, char *nomeArquivo) {
    if (registros == NULL)
    {
        printf("Erro ao carregar o registro\n");
        return -1;
    }

    if (nomeArquivo == NULL)
    {
         printf("Erro ao carregar o nome do arquivo\n");
        return -1;
    }
    
    
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) return -1;
    char linha[128];
    for (int i = 0; i < total; i++) {
        if (!fgets(linha, sizeof(linha), file)) {
            fclose(file);
            return -1; // arquivo terminou antes do esperado
        }
        char *t = strtok(linha, " ");
        char *s = strtok(NULL, " ");
        char *v = strtok(NULL, "\n");
        if (t == NULL || s == NULL || v == NULL) {
            registros[i].timestamp = 0;
            strcpy(registros[i].sensorID, "");
            printf("Linha com %d esta com problema\n", i);
            continue; 
        } 
        
        registros[i].timestamp = strtol(t, NULL, 10);
        strncpy(registros[i].sensorID, s, sizeof(registros[i].sensorID)-1);
        registros[i].sensorID[sizeof(registros[i].sensorID)-1] = '\0';
        strncpy(registros[i].valor, v, sizeof(registros[i].valor)-1);
        registros[i].valor[sizeof(registros[i].valor)-1] = '\0';
    }
    fclose(file);
    return 0;
}

// Função para checar se arquivo existe
int arquivoExiste(const char *nomeArquivo) {

      if (nomeArquivo == NULL)
    {
         printf("Erro ao carregar o nome do arquivo\n");
        return -1;
    }

    FILE *f = fopen(nomeArquivo, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

// Função para comparar registros pelo timestamp para qsort
int cmpRegistros(const void *a, const void *b) {
    Registro *r1 = (Registro *)a;
    Registro *r2 = (Registro *)b;
    if (r1->timestamp < r2->timestamp) return 1;  
    if (r1->timestamp > r2->timestamp) return -1;  
    return 0;                                      
}


void ordenarPorTempo(Registro *lista, int total) {
      
    qsort(lista, total, sizeof(Registro), cmpRegistros);
}

int salvarArquivoSensor(Registro *lista, int total, const char *nomeSensor) {

      if (lista == NULL)
    {
        printf("Erro ao carregar a lista\n");
        return -1;
    }

    if (nomeSensor == NULL)
    {
         printf("Erro ao carregar o nome do sensor\n");
        return -1;
    }
    char nomeArquivo[64];
    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s.txt", nomeSensor);

    if (arquivoExiste(nomeArquivo)) {
        printf("Arquivo %s já existe. Deseja sobrescrever? (s/n): ", nomeArquivo);
        int c = getchar();
        while (getchar() != '\n');  // limpar buffer
        if (c != 's' && c != 'S') {
            printf("Arquivo %s nao sobrescrito.\n", nomeArquivo);
            return 1;
        }
    }

    FILE *file = fopen(nomeArquivo, "w");
    if (!file) {
        printf("Erro ao criar arquivo %s\n", nomeArquivo);
        return -1;
    }

    for (int i = 0; i < total; i++) {
        fprintf(file, "%ld %s %s\n", lista[i].timestamp, lista[i].sensorID, lista[i].valor);
    }
    fclose(file);
    printf("Arquivo %s criado com sucesso!\n", nomeArquivo);
    return 0;
}

int separarSensores(Registro *registros, int total) {

      if (registros == NULL)
    {
        printf("Erro ao carregar o registro\n");
        return -1;
    }

    bool aindaTemSensor = true;

    while (aindaTemSensor) {
        aindaTemSensor = false;
        char sensorAtual[20] = "";

        // Encontrar próximo sensor não processado
        for (int i = 0; i < total; i++) {
            if (registros[i].timestamp != 0) { // timestamp 0 indica registro já processado
                strcpy(sensorAtual, registros[i].sensorID);
                aindaTemSensor = true;
                break;
            }
        }
        if (!aindaTemSensor) break;

        // Contar quantos registros deste sensor
        int count = 0;
        for (int i = 0; i < total; i++) {
            if (registros[i].timestamp != 0 && strcmp(registros[i].sensorID, sensorAtual) == 0)
                count++;
        }

        Registro *sensorData = malloc(count * sizeof(Registro));
        if (!sensorData) {
            printf("Erro ao alocar memória para sensor %s\n", sensorAtual);
            return -1;
        }

        int j = 0;
        for (int i = 0; i < total; i++) {
            if (registros[i].timestamp != 0 && strcmp(registros[i].sensorID, sensorAtual) == 0) {
                sensorData[j++] = registros[i];
                // Marca registro como processado zerando timestamp e sensorID
                registros[i].timestamp = 0;
                registros[i].sensorID[0] = '\0';
                registros[i].valor[0] = '\0';
            }
        }

        ordenarPorTempo(sensorData, count);

        if (salvarArquivoSensor(sensorData, count, sensorAtual) != 0) {
            free(sensorData);
            return -1;
        }

        free(sensorData);
    }

    return 0;
}
