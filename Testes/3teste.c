//
//  arv_b_arq.c
//
//
//  Created by Pedro Teixeira on 16/06/17.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arv_b_arq.h"
#include "debugger.h"

//gcc debugger.c debugger.h arv_b_arq.h 3teste.c

/*
 =====================================LOG=====================================

***[PP-DÚVIDA]*** Após recuperar os nós dos arquivos, deve-se dar free neles ? [@D3]
***[PP-WARNING]*** possiveis bugs na questão dos endereços char dos filhos ... irei verificar[@ D2]
***[PP-DÚVIDA]*** cond de parada correta?? [@ D1]


 =====================================LOG=====================================
*/

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
    
    /*
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
    printf("alou3");
    fwrite(&no -> nchaves, sizeof(int), 1 , fp);                          //grava nchaves
    fwrite(&no -> folha, sizeof(int), 1, fp);                             //grava se é folha
    
    for(i = 0; i < no->nchaves; i++) 
        fwrite(&no->chave[i], sizeof(int), 1, fp);                       //grava chaves 
        
    for(i = 0; i <= no->nchaves; i++){
        /**int j;                                                        //      ↓ seria isso?
        for(j=0;j<90;j++) fwrite(no->filho[i][j],sizeof(char),1,fp);     //grava caracter-a-caracter os nomes dos arquivos **/
        fputs(no->filho[i], fp);
    }
    
    fclose(fp);                                  //fecha o arq
    Libera_no(no);                               //Libera da mp
    
    return nome;
}

TAB *Leitura_arq(char *arq){
    FILE *fp = fopen (arq, "rb");
    if (!fp) return NULL;
    TAB *aux = Cria_no(2);
    fread(&aux->nchaves,sizeof(int),1,fp);
    fclose(fp);
    return aux;
}

TAB *Libera_no(TAB *a){
    if(!a) return a;
    if(a->folha == 0){
        int i;
        for(i=0;i<((a->nchaves))+1;i++) if(a->filho[i]) free(a->filho[i]);
    }
    free(a->chave);
    free(a->filho);
    free(a);
    return NULL;
}

int main (){
    passei(" ");
    TAB *aux = Cria_no(2);
    aux -> nchaves = 1;
    aux -> folha = 0;
    aux -> chave[0] = 20;
    aux -> filho[0] = "meu novo filho\0";
    Cria(aux, "arquivoTeste.dat");
    free(aux);
    return 0;
}

//PS : QUANDO PRONTA A FUNÇÃO Q RECUPERA UM TAB DO ARQUIVO Joga ela como 