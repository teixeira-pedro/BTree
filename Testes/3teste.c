#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arv_b_arq.h"


TAB *Cria_no(int t){
    int i;
    TAB *n=(TAB *)malloc(sizeof(TAB));
    n->nchaves = 0;
    n->chave = (int*)malloc(sizeof(int)*((t*2)-1));
    n->folha = 1;
    n->filho = (char **)malloc((t*2) * (sizeof(char *)));
    for(i = 0; i < (t*2); i++) n->filho[i] = (char *) malloc(90*sizeof(char));
    return n;
}

char *Cria(TAB *no, char *nome){
    
    /**
    salvaremos os arquivos da seguinte forma:

    +------------+
    |  nchaves   |← nfilhos implicito
    +------------+
    |  folha     |← é follha
    +------------+
    |1,3,4,...   |← chaves 
    +------------+
    |<nomes dos  |
    | arquivos>  |
    +------------+
    
    **/
    
    if((!no)||(!nome)) return NULL; 
    FILE *fp = fopen (nome,"wb");                                         //abre o arquivo de nome "nome"
    if(!fp) return NULL;                                                  //cria o nó
    int i;
    fwrite(&no -> nchaves, sizeof(int), 1 , fp);                          //grava nchaves
    fwrite(&no -> folha, sizeof(int), 1, fp);                             //grava se é folha
    
    for(i = 0; i < no -> nchaves; i++) 
        fwrite(&no->chave[i], sizeof(int), 1, fp);                       //grava chaves 
    for(i = 0; i <= no -> nchaves; i++){
        // copia a string pra aux pois queremos manipular para adicionar um "\n" no final para lermos mais tarde
        char aux [90];
        strcpy(aux, no->filho[i]);
        aux[strlen(aux)] = '\n';
        aux[strlen(aux) + 1] = '\0';
        fputs(aux, fp);                                                  // grava a string efetivamente
    }
    fclose(fp);                                                          //fecha o arq 
    return nome;
}

void Libera_no(TAB *a){
    if(!a) return;
    
    if(a->folha == 0){
        int i;
        for(i = 0; i <= (a->nchaves); i++)
            if(a->filho[i]){
                a-> filho[i] = NULL;
                free(a -> filho[i]);
            }
    }
    free(a->chave);
    free(a->filho);
    free(a);
    return ;
}

TAB *recupera (char *nome){
    FILE *fp = fopen (nome, "rb");
    if (!fp) return NULL;
    TAB *aux = Cria_no(2);
    fread(&aux->nchaves, sizeof(int), 1, fp);               // pega nchaves
    fread(&aux->folha, sizeof(int), 1, fp);                 // pega se é folha ou n
    int i;
    for(i = 0; i < aux->nchaves; i++) 
        fread(&aux->chave[i], sizeof(int), 1, fp);          // pega o número de chaves que for necessário
    for(i = 0; i <= aux->nchaves; i++){
        fgets(aux->filho[i], 90, fp);                       // pega o número de filhos que for necessário (1 a mais de chaves)
        aux->filho[i][strlen(aux->filho[i]) - 1] = '\0';    // tira o "\n" e substitui por "\0", assim n vai bugar pras próximas funções que usarem o nome do arquivo
    }
    fclose(fp);
    return aux;
}

void imprime_data(TAB *a){
    if(!a) {
        printf("→//\n");
        return;
    }
    int i;
    printf("chaves → [");
    for(i=0;i<a->nchaves;i++) printf("%d ",a->chave[i]);
    printf("]\n");
    printf("filhos → ");
    if(!a->folha){
        printf("[");
        for(i=0;i<a->nchaves+1;i++) printf(" <<%s.dat>>  ",a->filho[i]);
        printf("]");
    }else{
        printf("//");
    }
    printf("\n");
}

void Imprime(char *nome, int andar){
    if(!nome) return;                                   // vê se o arq existe
    TAB *a = recupera(nome);                            // recupera do arquivo
    if(a){                                              // imprime recursivamente.
        int i,j;
        for(i = 0; i < a->nchaves; i++){            // cond de parada correta?? Não seria i<=a->nchaves ? [D1] - Nein.
            Imprime(a->filho[i],andar+1);           // filhos da esquerda
            for(j=0; j<=andar; j++) printf("   ");
            printf("%d\n", a->chave[i]);
        }
        Imprime(a->filho[i],andar+1);               // filho da direita
    }
    Libera_no(a);                               //"Free" na MP [D3]
}



void Libera(char *nome){
    if(!nome) exit(-1);
    TAB *a=recupera(nome);                        //recupera o nó para apagar os filhos recursivamente
    if(a){
        int i;
        for(i=0; i<=a->nchaves; i++) Libera(a->filho[i]);//mata filhos
    }
    remove(nome);                                 //mata o (arquivo) nó atual
    Libera_no(a);                                 //"Free" na MP
}

char *Busca_arq(char x[90], int ch){
    TAB *resp = NULL;
    if(!x) return NULL;
    resp=recupera(x);                                                        //recupera nó atual
    if(!resp) return NULL;                                                   //se deu ruim na leitura, XIBU
    int i = 0;
    while( (i < resp->nchaves) && (ch > resp->chave[i]) ) i++;               //busca para ver se está nas chaves do nó recuperado
    if( (i < resp->nchaves) && (ch == resp->chave[i]) ) return x;            //se achou retorna o endereço
    if(resp->folha) {
        Libera_no(resp);                                                     //mata da MP
        return NULL;                                                         //se não tá ali e não tem filhos, acabou
    }
    //char *tmp=resp->filho[i];
    char filho_i[90];
    strcpy(filho_i, resp->filho[i]);                                         //Guardando nome do filho para liberar a MP
    //return Busca_arq(resp->filho[i], ch);
    Libera_no(resp);                                                         //mata da MP
    return Busca_arq(filho_i, ch);                                           //se tem, busca nos filhos
}

TAB *Busca(char* x, int ch){
    if(!x) return NULL;
    TAB *resp = NULL;
    resp=recupera(x);                                                       // recupera nó atual
    if(!resp) return NULL;                                                  // se deu ruim na leitura, XIBU
    int i = 0;
    while( (i < resp->nchaves) && (ch > resp->chave[i]) ) i++;              // busca para ver se está nas chaves do nó recuperado
    if( (i < resp->nchaves) && (ch == resp->chave[i]) ) return resp;        // se achou retorna o nó
    if(resp->folha == 1) return NULL;                                       // se não tá ali e não tem filhos, acabou
    char filho_i[90];
    strcpy(filho_i, resp->filho[i]);                                         // Guardando nome do filho para liberar a MP
    Libera_no(resp);                                                        // mata da MP
    return Busca(resp->filho[i], ch);                                       // se tem, busca nos filhos
}

TAB *pega_filho(TAB *arv, int qualFilho){
    if(!arv){
        printf("Erro no pega_filho: Árvore vazia.\n");
        return NULL;
    }
    TAB *resp = recupera(arv->filho[qualFilho]);
    if(!resp) {
        printf("Erro no pega_filho: Erro ao ler filho.\n");
        return NULL;
    }
    return resp;
}

char *pega_filho_arq(char *nArq, int qualFilho){
    // TESTADO - pergola
    if(!nArq){
        printf("Erro no pega_filho_arq: nome vazio.\n");
        return NULL;
    }
    
    FILE *fp = fopen(nArq, "rb");
    int nchaves, i;
    fread(&nchaves, sizeof(int), 1, fp);              // lê quantas chaves tem
    if (nchaves == 0){
        printf("Erro no pega_filho_arq: Arquivo de entrada vazio.\n");
        return NULL;
    }
    
    // posiciona o ponteiro do stream no inicio da string do primeiro filho
    fseek(fp, (sizeof(int)*nchaves + 2*sizeof(int)), SEEK_SET);     // + 1 pelo int de nchaves e + 1 pelo int de folha (+2 no total)
    
    char aux [90];
    char *resp = (char *)malloc(sizeof(char) * 90);
    for (i = 0; i <= qualFilho; i++) {
        fgets(aux, 90, fp);
        
    }
    aux[strlen(aux) - 1] = '\0';    // tira o "\n" e substitui por "\0", assim n vai bugar pras próximas funções que usarem o nome do arquivo
    strcpy(resp, aux);
    fclose(fp);
    
    if(!resp) {
        printf("Erro no pega_filho_arq: Erro ao ler filho.\n");
        return NULL;
    }
    //printf("Pega ar filho: %s\n", resp);
    return resp;
}

void Imprime_ms(char *nome, int andar){
    
    if(!nome) return;
    FILE *fp = fopen(nome, "rb");
    if (!fp) return;
    // imprime recursivamente
    int nchaves, chave, i, j, folha;
    fread(&nchaves, sizeof(int), 1, fp);
    fread(&folha, sizeof(int), 1, fp);
    
    for (i = 0; i < nchaves; i++){
        if (folha == 0) Imprime_ms(pega_filho_arq(nome, i), andar + 1);     // só continua a imprimir recursivamente se não for folha
        for(j = 0; j < andar; j++) printf("\t");
        fread(&chave, sizeof(int), 1, fp);
        printf("%d", chave);
        printf("\n");
    }
    
    if (folha == 0) Imprime_ms(pega_filho_arq(nome, i), andar +1);     // imprime ultimo arquivo
    fclose(fp);
    
    
}

int main (){
    
    /** Testa criar, salvar e carrega um arquivo raiz **/
    TAB *raiz = Cria_no(2);
    printf("Arvore B\n");
    printf("Insira chave inicial: \n");
    scanf("%d", &raiz->chave[0]);
    printf("Insira chave segunda inicial: \n");
    scanf("%d", &raiz->chave[1]);
    raiz->nchaves = 2;
    raiz->folha = 0;
    raiz->filho[0] = "meufilho1.dat";
    raiz->filho[1] = "meufilho2.dat";
    raiz->filho[2] = "meufilho3.dat";
    Cria(raiz, "minharaiz.dat");
    Libera_no(raiz);
    raiz = recupera("minharaiz.dat");
    
    
    /** Testa criar, salvar e carregar filhos do arquivo raiz criado **/
    TAB *filho1 = Cria_no(2);
    TAB *filho2 = Cria_no(2);
    TAB *filho3 = Cria_no(2);
    
    printf("Insira chave do filho1: \n");
    scanf("%d", &filho1->chave[0]);
    printf("Insira chave do filho2: \n");
    scanf("%d", &filho2->chave[0]);
    printf("Insira chave do filho3: \n");
    scanf("%d", &filho3->chave[0]);
    
    filho1-> nchaves = 1;
    filho2-> nchaves = 1;
    filho3-> nchaves = 1;
    
    filho1-> folha = 1;
    filho2-> folha = 1;
    filho3-> folha = 1;
    
    Cria(filho1, "meufilho1.dat");
    Cria(filho2, "meufilho2.dat");
    Cria(filho3, "meufilho3.dat");
    
    Libera_no(filho1);
    Libera_no(filho2);
    Libera_no(filho3);
    
    Libera_no(raiz);
    
    printf("\n\t\tImprimindo...\t\n ");
    
    /** Testa imprimir **/
    Imprime_ms("minharaiz.dat", 0);
    
    return 0;
}


