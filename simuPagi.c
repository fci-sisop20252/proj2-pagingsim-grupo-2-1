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
int quantPagina_Processo[Max_PROCESSOS]; // quantidade de paginas que fazem parte de um projeto 

int paginaCarregada[Max_PROCESSOS][MAX_PAGINAS]; // indica se a pagina j do processo i esta carregada (1 = esta carregada, 0 = nao esta)
int framePagina[Max_PROCESSOS][MAX_PAGINAS]; // indica em qual frame a pagina j foi colocada (x = indice do frame, -1 = nao esta carregado)
int R_Pagina[Max_PROCESSOS][MAX_PAGINAS]; // indica o R-bit da pagina j 


int idFrame[MAX_FRAMES]; 
int numPagina_Frame[MAX_FRAMES]; // indica o numero da pagina que esta no frame
int frameLivre[MAX_FRAMES]; // (1 = livre, 0 = ocupado)


int posiFifo = 0; // posição do ponteiro no FIFO
int posiClock = 0; // posição do ponteiro no FIFO

int total_acessos = 0;
int total_faults = 0;

// função responsavel por pegar o indice de um processo a partir do seu id
int buscaProcesso(int id){
    for(int i = 0; i < nProcesso; i++){
        if(idProcesso[i] == id){
            return i;
        }
    }
    return -1; // não encontrou
}

// função responsavel por percorrer os frames ate achar um livre
int buscarFrameLivre(){
    for(int i = 0; i < nFrame; i++){
        if(frameLivre[i] == 1){
            return i;
        }
    }
    return -1; // não encontrou
}

// função responsavel por selecionar qual frame vai ter sua pagina substituida por meio do FIFO
int selecionaVitimaFIFO(){
    int v = posiFifo;
    posiFifo = (posiFifo + 1) % nFrame;
    return v;
}

// função responsavel por selecionar qual frame vai ter sua pagina substituida por meio do CLOCK (segunda chance)
int selecionaVitimaClock(){
    while(1){
        int v = posiClock;
        int idv = idFrame[v];
        int paginav = numPagina_Frame[v];
        int idProcv = buscaProcesso(idv);

        // se R = 0 passa para o proximo, mas retorna o frame com R = 0
        if(R_Pagina[idProcv][paginav] == 0){
            posiClock = (posiClock + 1) % nFrame;
            return v;
        }
        // se R = 1 zera e passa para o proximo
        R_Pagina[idProcv][paginav] = 0;
        posiClock = (posiClock + 1) % nFrame;
    }
}

// função responsavel por simular o FIFO
void fifo(int id, int ende){
    total_acessos++;
    int idProc = buscaProcesso(id);
    int pagina = ende / tamPagina; // calculo do numero da pagina
    int deslocamento = ende % tamPagina; // calculo do deslocamento

    printf("Acesso: PID %d, Endereço %d (Pagina %d, Deslocamento: %d) -> ", id, ende, pagina, deslocamento);

    if(paginaCarregada[idProc][pagina]){ // se a pagina ja esta carregada
        printf("HIT: Pagina %d (PID %d) ja esta no frame %d\n",pagina, id,framePagina[idProc][pagina]);
        return;
    }
    total_faults++;
    int f = buscarFrameLivre();

    if(f != -1){ // se nao esta carregada, mas ainda temos frames livres
        frameLivre[f] = 0;
        idFrame[f] = id;
        numPagina_Frame[f] = pagina;

        paginaCarregada[idProc][pagina] = 1;
        framePagina[idProc][pagina] = f;

        printf("PAGE FAULT: Pagina %d (PID %d) alocada no frame livre %d\n", pagina, id, f);
        return;
    }

    // pagina nao carregada e nao temos mais espaço livre
    int vitima = selecionaVitimaFIFO();

    // pego as informaçoes do frame antigo (vitima) para poder "descartar" ele
    int idAntigo = idFrame[vitima]; 
    int paginaAntiga = numPagina_Frame[vitima];
    int idProcAntigo = buscaProcesso(idAntigo);

    paginaCarregada[idProcAntigo][paginaAntiga] = 0;

    idFrame[vitima] = id;
    numPagina_Frame[vitima] = pagina;

    // Aqui ja estou carregando a nova pagina no frame
    paginaCarregada[idProc][pagina] = 1;
    framePagina[idProc][pagina] = vitima;

    printf("Page FAULT: Memoria cheia. Pagina %d (PID %d) (Frame %d) sera desalocada -> Pagina %d (PID %d) alocada no frame %d\n", paginaAntiga, idAntigo, vitima, pagina, id, vitima);

}


// função responsavel por simular o CLOCK
void clock(int id, int ende){
    total_acessos++;
    int idProc = buscaProcesso(id);
    int pagina = ende / tamPagina;
    int deslocamento = ende % tamPagina;

    printf("Acesso: PID %d, Endereço %d (Pagina %d, Deslocamento: %d) -> ", id, ende, pagina, deslocamento);

    if(paginaCarregada[idProc][pagina]){
        R_Pagina[idProc][pagina] = 1; // se a pagina ja esta carregada setamos o R-bit para 1
        printf("HIT: Página %d (PID %d) já está no Frame %d\n", pagina, id, framePagina[idProc][pagina]);
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
        R_Pagina[idProc][pagina] = 1; // se carregamos a pagina agora, setamos o R-bit em 1

        printf("PAGE FAULT -> Página %d (PID %d) alocada no Frame livre %d\n", pagina, id, f);
        return;
    }

    int vitima = selecionaVitimaClock();
    int idAntigo = idFrame[vitima];
    int paginaAntiga = numPagina_Frame[vitima];
    int idProcAntigo = buscaProcesso(idAntigo);

    paginaCarregada[idProcAntigo][paginaAntiga] = 0;
    R_Pagina[idProcAntigo][paginaAntiga] = 0; // como vamos "descartar" essa pagina antiga garantimos que seu R-bit = 0

    idFrame[vitima] = id;
    numPagina_Frame[vitima] = pagina;

    paginaCarregada[idProc][pagina] = 1;
    framePagina[idProc][pagina] = vitima;
    R_Pagina[idProc][pagina] = 1; // marca o R-bit dessa nova pagina como 1

    printf("PAGE FAULT -> Memória cheia. Página %d (PID %d) (Frame %d) será desalocada. -> Página %d (PID %d) alocada no Frame %d\n", paginaAntiga, idAntigo, vitima, pagina, id, vitima);

}


int main(int argc, char *argv[]){

    // temos a verificação se temos o nome do programa + 3 argumentos ao tentarmos executa o programa
    if(argc != 4){
        printf("Uso: %s <fifo|clock> <config> <acessos> \n", argv[0]);
        return 1;
    }

    char *alg = argv[1]; // 1 argumento = qual algoritimo usado
    char *fconfig = argv[2]; // 2 argumento = qual arquivo de configuração
    char *facessos = argv[3]; // 3 argumento = qual arquivo de acesso

    // abertura do arquivo de configuração
    FILE *fc = fopen(fconfig, "r");
    if(!fc){
        printf("ERRO ao abrir o arquivo de configuração\n");
        return 1;
    }

    // le do arquivo configuração as informações necessarias para a execução do codigo
    fscanf(fc, "%d", &nFrame);
    fscanf(fc, "%d", &tamPagina);
    fscanf(fc, "%d", &nProcesso);

    // deixamos inicialmete todos os frames livres, sem id e sem o numero da pagina
    for(int i = 0; i < nFrame; i++){
        frameLivre[i] = 1;
        idFrame[i] = -1;
        numPagina_Frame[i] = -1;
    }


    for(int i = 0; i < nProcesso; i++){
        int id;
        int tamVirtual;

        // le do arquivo configuração outras informações necessarias para a execução do codigo
        fscanf(fc, "%d %d", &id, &tamVirtual);

        idProcesso[i] = id;

        quantPagina_Processo[i] = (tamVirtual + tamPagina - 1) / tamPagina;

        // inicialização da tabela de paginas
        for(int j = 0; j < quantPagina_Processo[i]; j++){
            paginaCarregada[i][j] = 0;
            framePagina[i][j] = -1;
            R_Pagina[i][j] = 0;
        }
    }
    fclose(fc);

    //abre o arquivo de acesso
    FILE *fa = fopen(facessos, "r");
    if(!fa){
        printf("ERRO ao abrir o arquivo de acesso\n");
        return 1;
    }

    int id;
    int ende;

    // le do arquivo acessos as informações necessarias para a execução do codigo
    while(fscanf(fa, "%d %d", &id, &ende) == 2){
        // se o argumento 1 digitado ao tentar executar for fifo, faz fifo 
        if(strcmp(alg, "fifo") == 0){
            fifo(id,ende);
        }
        // se o argumento  digitado ao tentar executar for clock, faz clock 
        else if(strcmp(alg, "clock") == 0){
            clock(id,ende);
        }
        else{
            printf("Algoritimo invalido\n");
            fclose(fa);
            return 1;
        }
    }
    fclose(fa);

    // estatisticas finais
    printf("\nTeste com o algoritimo %s:\n", alg);
    printf("----Total de acessos: %d\n", total_acessos);
    printf("----Total de Page Faults: %d\n", total_faults);

    return 0;
}


