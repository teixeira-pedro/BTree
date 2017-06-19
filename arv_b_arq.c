//
//  arv_b_arq.c
//  
//
//  Created by Pedro Teixeira on 16/06/17.
//
//

const int t = 2;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debugger.h"
#include "arv_b_arq.h"
/*
 =====================================LOG=====================================
 
***[PP-DÚVIDA]*** Após recuperar os nós dos arquivos, deve-se dar free neles?
***[PP-WARNING]*** possiveis bugs na questão dos endereços char dos filhos ... irei verificar
 

 =====================================LOG=====================================
*/
char *Cria(int t,char *nome){
    if(!nome) return NULL;
    FILE *fp=fopen(nome,"wb");                  //abre o arquivo de nome "nome"
    if(!fp) return NULL;                        //cria o nó
    TAB* novo = (TAB*)malloc(sizeof(TAB));
    novo->nchaves = 0;
    novo->chave =(int*)malloc(sizeof(int*)*((t*2)-1));
    novo->folha=1;
    novo->filho = (char**)malloc(sizeof((char)*4)*t*2);//matriz de caracteres contendo endereços dos filhos
    fwrite(novo,sizeof(TAB),1,fp);              //grava o nó
    fclose(fp);                                 //fecha o arq
    int i;                                      //libera da MP
    for(i=0;i<(t*2);i++) free(novo->chave[i]);
    free(novo);
    return nome;
}

void Imprime(char *nome, int andar){
    if(!nome) return;                           //vê se o arq existe
    FILE *fp=fopen(nome,"rb");
    if(!fp) return;
    TAB *a;                                     //recupera do arquivo
    fread(a,sizeof(TAB),1,fp);
    fclose(fp);
    if(a){                                      //imprime recsvmt.
        int i,j;
        for(i=0; i<=a->nchaves-1; i++){
            Imprime(a->filho[i],andar+1);
            for(j=0; j<=andar; j++) printf("   ");
            printf("%d\n", a->chave[i]);
        }
        Imprime(a->filho[i],andar+1);
    }
}


void Libera(char *nome){
    if(!nome) exit(-1);
    FILE *fp=fopen(nome,"wb");                  //abre o arquivo de nome "nome"
    if(!fp) exit(-1);                           //XIBU
    TAB *a;                                     //recupera o nó para apagar os filhos recursivamente
    fread(a,sizeof(TAB),1,fp);
    fclose(fp);
    if(a){
        for(i=0; i<=a->nchaves-1; i++) Libera(a->filho[i]);//mata filhos
    }
    remove(nome);                               //mata o nó atual
}

TAB *Inicializa(){
    return NULL;
}




