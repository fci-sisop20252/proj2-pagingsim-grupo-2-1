#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Max_PROCESSOS 100
#define MAX_PAGINAS 1000
#define MAX_FRAMES 1000


int nProcesso;
int nFrame;
int tamPagina;


int idProcesso[Max_PROCESSOS];
int quantPagina_Processo[Max_PROCESSOS];

int paginaCarregada[Max_PROCESSOS][MAX_PAGINAS];
int framePagina[Max_PROCESSOS][MAX_PAGINAS];
int R_Pagina[Max_PROCESSOS][MAX_PAGINAS];


int idFrame[MAX_FRAMES];
int numPagina_Frame[MAX_FRAMES];
int frameLivre[MAX_FRAMES];


int posiFifo = 0;
int posiClock = 0;

int total_acessos = 0;
int total_faults = 0;


int buscaProcesso(int id){
    for(int i = 0; i < nProcesso; i++){
        if(idProcesso[i] == id){
            return i;
        }
    }
    return -1;
}

int buscarFrameLivre(){
    for(int i = 0; i < nFrame; i++){
        if(frameLivre[i] == 1){
            return i;
        }
    }
    return -1;
}

int selecionaVitimaFIFO(){
    int v = posiFifo;
    posiFifo = (posiFifo + 1) % nFrame;
    return v;
}

void fifo(int id, int ende){
    total_acessos++;
    int idProc = buscaProcesso(id);
    int pagina = ende / tamPagina;
    int deslocamento = ende % tamPagina;

    printf("Acesso: PID %d, Endereço %d (Pagina %d, Deslocamento: %d) -> ", id, ende, pagina, deslocamento);

    if(paginaCarregada[idProc][pagina]){
        printf("HIT: Pagina %d (PID %d) ja esta no frame %d\n",pagina, id,framePagina[idProc][pagina]);
        return;
    }
    total_faults++;
    int f = buscarFrameLivre();

    if(f != -1){
        frameLivre[f] = 0;
        idFrame[f] = id;
        numPagina_Frame[f] = pagina;

        paginaCarregada[idProc][pagina] = 1;
        framePagina[idProc][pagina] = f;

        printf("PAGE FAULT: Pagina %d (PID %d) alocada no frame livre %d\n", pagina, id, f);
        return;
    }

    int vitima = selecionaVitimaFIFO();
    int idAntigo = idFrame[vitima];
    int paginaAntiga = numPagina_Frame[vitima];
    int idProcAntigo = buscaProcesso(idAntigo);

    paginaCarregada[idProcAntigo][paginaAntiga] = 0;

    idFrame[vitima] = id;
    numPagina_Frame[vitima] = pagina;

    paginaCarregada[idProc][pagina] = 1;
    framePagina[idProc][pagina] = vitima;

    printf("Page FAULT: Memoria cheia. Pagina %d (PID %d) (Frame %d) sera desalocada -> Pagina %d (PID %d) alocada no frame %d\n", paginaAntiga, idAntigo, vitima, pagina, id, vitima);


}

int main(int argc, char *argv[]){

    if(argc != 4){
        printf("Uso: %s <fifo|clock> <config> <acessos> \n", argv[0]);
        return 1;
    }

    char *alg = argv[1];
    char *fconf = argv[2];
    char *facs = argv[3];


    FILE *fc = fopen(fconf, "r");
    if(!fc){
        printf("ERRO ao abrir o arquivo de configuração\n");
        return 1;
    }

    fscanf(fc, "%d", &nFrame);
    fscanf(fc, "%d", &tamPagina);
    fscanf(fc, "%d", &nProcesso);

    for(int i = 0; i < nFrame; i++){
        frameLivre[i] = 1;
        idFrame[i] = -1;
        numPagina_Frame[i] = -1;
    }

    for(int i = 0; i < nProcesso; i++){
        int id;
        int tamVirtual;

        fscanf(fc, "%d %d", &id, &tamVirtual);

        idProcesso[i] = id;

        quantPagina_Processo[i] = (tamVirtual + tamPagina - 1) / tamPagina;

        for(int j = 0; j < quantPagina_Processo[i]; j++){
            paginaCarregada[i][j] = 0;
            framePagina[i][j] = -1;
            R_Pagina[i][j] = 0;
        }
    }
    fclose(fc);

    FILE *fa = fopen(facs, "r");
    if(!fa){
        printf("ERRO ao abrir o arquivo de acesso\n");
        return 1;
    }

    int id;
    int ende;

    while(fscanf(fa, "%d %d", &id, &ende) == 2){
        if(strcmp(alg, "fifo") == 0){
            fifo(id,ende);
        }

        else{
            printf("Algoritimo invalido\n");
            fclose(fa);
            return 1;
        }
    }
    fclose(fa);


    printf("\nTeste com o %s:\n", alg);
    printf("----Total de acessos: %d\n", total_acessos);
    printf("----Total de Page Faults: %d\n", total_faults);

    return 0;
}


