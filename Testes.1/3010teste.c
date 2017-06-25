#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arv_b_arq.h"

TAB *Cria_no(int t){
    
    int i;
    TAB *n = (TAB *)malloc(sizeof(TAB));
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

TAB *recupera (char *nome, int t){
    FILE *fp = fopen (nome, "rb");
    if (!fp) return NULL;
    TAB *aux = Cria_no(t);
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

TAB *pega_filho(TAB *arv, int qualFilho, int t){
    if(!arv){
        printf("Erro no pega_filho: Árvore vazia.\n");
        return NULL;
    }
    TAB *resp = recupera(arv->filho[qualFilho], t);
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

void remover(char *nArq, int ch, int t){
    if(!nArq){
        printf("Erro no remover: Nome do arquivo vazio.\n");
        return;
    }
    TAB *arv = NULL;
    arv = recupera(nArq, t);
    int i;
    for(i = 0; ( (i < arv->nchaves) && (arv->chave[i]) ) < ch; i++);
    if( (i < arv->nchaves) && (ch == arv->chave[i]) ){ //CASOS 1, 2A, 2B e 2C
        if(arv->folha){ 
            //CASO 1
        }
        
        TAB *y = NULL;
        y = pega_filho(arv, i, t); //Encontrar o predecessor k' de ch na árvore com raiz em y
        if(!y) {
          printf("Erro no remover: Erro ao ler filho y.\n");
          Libera_no(arv);
          Libera_no(y);
          return;
        }
        
        if( (!arv->folha) && (y->nchaves >= t) ){ //CASO 2A
          printf("\nCASO 2A\n");
          while(!y->folha) y = pega_filho(y, y->nchaves, t); // Serve para preparar a função pro caso 3
          int temp = y->chave[y->nchaves-1]; //temp é k'
          char *filho = pega_filho_arq(nArq, i);
          if(!filho) {
            printf("Erro no remover: Filho inválido (Caso 2A).\n");
            Libera_no(arv);
            Libera_no(y);
            return;
          }
          //filho = remover(filho, temp, t); //Eliminar recursivamente k' (comentei a linha pois estava atribuindo ao filho uma função void)
          remover(filho, temp, t); //Eliminar recursivamente k'
          arv->chave[i] = temp; //Substitua ch por k' em x
          Cria(arv, nArq);
          Libera_no(arv);
          Libera_no(y);
          return;
        }
        
        TAB *z = NULL;
        z = pega_filho(arv, i+1, t); //Encontrar o sucessor k' de k na árvore com raiz em z
        if(!z) {
          printf("Erro no remover: Erro ao ler filho z.\n");
          Libera_no(arv);
          Libera_no(z);
          return;
        }
        
        if( (!arv->folha) && (z->nchaves >= t) ){  //CASO 2B
          printf("\nCASO 2B\n");
          while(!z->folha) z = pega_filho(z, 0, t); // Serve para preparar a função pro caso 3
          int temp = z->chave[0]; //temp é k'
          char *filho = pega_filho_arq(nArq, i+1);
          if(!filho) {
            printf("Erro no remover: Filho inválido (Caso 2B).\n");
            Libera_no(arv);
            Libera_no(z);
            return;
          }
          remover(filho, temp, t); //Eliminar recursivamente k'
          arv->chave[i] = temp; // Substitua ch por k' em x
          Cria(arv, nArq);
          Libera_no(arv);
          Libera_no(z);
          return;
        }
        if( (!arv->folha) && (z->nchaves == t-1) && (y->nchaves == t-1) ){ 
            //CASO 2C
        }
    }
}

void main(){
    /** 
       Legenda:
        A	B	C	D	E	F	G	H	I	J	K	L	M	N	O	P	Q	R	S	T	U	V	W	X	Y	Z
        1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26
    **/
    
    /** Recria árvore do caderno **/
    /** Raiz **/
    int t = 3;
    TAB *raiz = Cria_no(t);
    printf("Arvore B\n");
    printf("Inserindo P na raiz: \n");
    raiz->chave[0] = 16;
    raiz->nchaves = 1;
    raiz->folha = 0;
    raiz->filho[0] = "filho1.dat";
    raiz->filho[1] = "filho2.dat";
    Cria(raiz, "raiz.dat");
    Libera_no(raiz);
    
    /** Filhos **/
    TAB *filho1 = Cria_no(t);
    TAB *filho2 = Cria_no(t);
    
    printf("Inserindo C no filho 1: \n");
    filho1->chave[0] = 3;
    printf("Inserindo G no filho 1: \n");
    filho1->chave[1] = 7;
    printf("Inserindo M no filho 1: \n");
    filho1->chave[2] = 13;
    printf("Inserindo T no filho 2: \n");
    filho2->chave[0] = 20;
    printf("Inserindo X no filho 2: \n");
    filho2->chave[1] = 24;
    
    filho1->nchaves = 3;
    filho1->folha = 0;
    filho1->filho[0] = "neto1.dat";
    filho1->filho[1] = "neto2.dat";
    filho1->filho[2] = "neto3.dat";
    filho1->filho[3] = "neto4.dat";
    
    filho2->nchaves = 2;
    filho2->folha = 0;
    filho2->filho[0] = "neto5.dat";
    filho2->filho[1] = "neto6.dat";
    filho2->filho[2] = "neto7.dat";
    
    Cria(filho1, "filho1.dat");
    Cria(filho2, "filho2.dat");
    
    Libera_no(filho1);
    Libera_no(filho2);
    
    /** Netos **/
    TAB *neto1 = Cria_no(t);
    TAB *neto2 = Cria_no(t);
    TAB *neto3 = Cria_no(t);
    TAB *neto4 = Cria_no(t);
    TAB *neto5 = Cria_no(t);
    TAB *neto6 = Cria_no(t);
    TAB *neto7 = Cria_no(t);
    
    printf("Inserindo A no neto 1: \n");
    neto1->chave[0] = 1;
    printf("Inserindo B no neto 1: \n");
    neto1->chave[1] = 2;
    printf("Inserindo D no neto 2: \n");
    neto2->chave[0] = 4;
    printf("Inserindo G no neto 2: \n");
    neto2->chave[1] = 7;
    printf("Inserindo J no neto 3: \n");
    neto3->chave[0] = 10;
    printf("Inserindo K no neto 3: \n");
    neto3->chave[1] = 11;
    printf("Inserindo L no neto 3: \n");
    neto3->chave[2] = 12;
    printf("Inserindo N no neto 4: \n");
    neto4->chave[0] = 14;
    printf("Inserindo O no neto 4: \n");
    neto4->chave[1] = 15;
    printf("Inserindo Q no neto 5: \n");
    neto5->chave[0] = 17;
    printf("Inserindo R no neto 5: \n");
    neto5->chave[1] = 18;
    printf("Inserindo S no neto 5: \n");
    neto5->chave[2] = 19;
    printf("Inserindo U no neto 6: \n");
    neto6->chave[0] = 21;
    printf("Inserindo V no neto 6: \n");
    neto6->chave[1] = 22;
    printf("Inserindo Y no neto 7: \n");
    neto7->chave[0] = 25;
    printf("Inserindo Z no neto 7: \n");
    neto7->chave[1] = 26;
    
    neto1->nchaves = 2;
    neto1->folha = 1;
    neto2->nchaves = 2;
    neto2->folha = 1;
    neto3->nchaves = 3;
    neto3->folha = 1;
    neto4->nchaves = 2;
    neto4->folha = 1;
    neto5->nchaves = 3;
    neto5->folha = 1;
    neto6->nchaves = 2;
    neto6->folha = 1;
    neto7->nchaves = 2;
    neto7->folha = 1;
    
    Cria(neto1, "neto1.dat");
    Cria(neto2, "neto2.dat");
    Cria(neto3, "neto3.dat");
    Cria(neto4, "neto4.dat");
    Cria(neto5, "neto5.dat");
    Cria(neto6, "neto6.dat");
    Cria(neto7, "neto7.dat");
    
    Libera_no(neto1);
    Libera_no(neto2);
    Libera_no(neto3);
    Libera_no(neto4);
    Libera_no(neto5);
    Libera_no(neto6);
    Libera_no(neto7);
    
    printf("\n\t\tImprimindo...\t\n ");
    
    /** Testa imprimir **/
    Imprime_ms("raiz.dat", 0);
}