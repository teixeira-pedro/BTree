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
 
***[PP-DÚVIDA]*** Após recuperar os nós dos arquivos, deve-se dar free neles ? [@D3]
***[PP-WARNING]*** possiveis bugs na questão dos endereços char dos filhos ... irei verificar[@ D2]
 ***[PP-DÚVIDA]*** cond de parada correta?? [@ D1]
 

 =====================================LOG=====================================
*/
char *Cria(int t,char *nome){
    if(!nome) return NULL;
    FILE *fp=fopen(nome,"rb+");                  //abre o arquivo de nome "nome"
    if(!fp) return NULL;                        //cria o nó
    TAB* novo = (TAB*)malloc(sizeof(TAB));
    novo->nchaves = 0;
    novo->chave =(int*)malloc(sizeof(int*)*((t*2)-1));
    novo->folha=1;
    novo->filho = (char**)malloc(sizeof((char)*4)*t*2);//lista de strings contendo endereços dos filhos [D2]
    int i;
    for(i=0; i<(t*2); i++) novo->filho[i] = NULL;
    fwrite(novo,sizeof(TAB),1,fp);              //grava o nó
    fclose(fp);                                 //fecha o arq
    int i;
    for(i=0;i<(t*2);i++) free(novo->chave[i]);
    Libera_no(novo);                            //Libera da mp
    return nome;
}

void Imprime(char *nome, int andar){
    if(!nome) return;                           //vê se o arq existe
    FILE *fp=fopen(nome,"rb");                  //abre arquivo
    if(!fp) return;                             //XIBU
    TAB *a;                                     //recupera do arquivo
    fread(a,sizeof(TAB),1,fp);
    fclose(fp);
    if(a){                                      //imprime recsvmt.
        int i,j;
        for(i=0; i<=a->nchaves-1; i++){         //cond de parada correta?? Não seria i<=a->nchaves ? [D1]
            Imprime(a->filho[i],andar+1);
            for(j=0; j<=andar; j++) printf("   ");
            printf("%d\n", a->chave[i]);
        }
        Imprime(a->filho[i],andar+1);
    }
    Libera_no(a);                               //"Free" na MP [D3]
}


void Libera(char *nome){
    if(!nome) exit(-1);
    FILE *fp=fopen(nome,"rb+");                  //abre o arquivo de nome "nome"
    if(!fp) exit(-1);                           //XIBU
    TAB *a;                                     //recupera o nó para apagar os filhos recursivamente
    fread(a,sizeof(TAB),1,fp);
    fclose(fp);
    if(a){
        for(i=0; i<=a->nchaves; i++) Libera(a->filho[i]);//mata filhos
    }
    remove(nome);                               //mata o (arquivo) nó atual
    Libera_no(a);                               //"Free" na MP [D3]
    
}

TAB *Libera_no(TAB *a){
    if(a){
        if(!a->folha){
            int i;
            for(i = 0; i <= a->nchaves; i++) free(a->filho[i]);
        }
        free(a->chave);
        free(a->filho);
        free(a);
        return NULL;
    }
}

TAB *Inicializa(){
    return NULL;
}

TAB *Busca_arq(char* x, int ch){
    TAB *resp = NULL;
    FILE *fp=fopen(x,"rb");
    if(!fp) return resp;
    fread(resp,sizeof(TAB),1,fp);               //recupera nó atual
    fclose(fp);                                 //fecha arq
    if(!resp) return resp;
    int i = 0;
    while(i < resp->nchaves && ch > resp->chave[i]) i++;//busca para ver se está nas chaves do nó recuperado
    if(i < resp->nchaves && ch == resp->chave[i]) return x;//se achou retorna o nome do arquivo onde ele esta
    if(resp->folha) return NULL;                //se não tá ali e não tem filhos, acabou
    return Busca_arq(x->filho[i], ch);          //se tem, busca nos filhos
}

TAB *Busca(char* x, int ch){
    TAB *resp = NULL;
    FILE *fp=fopen(x,"rb");
    if(!fp) return NULL;
    int r =fread(resp,sizeof(TAB),1,fp);        //recupera nó atual
    fclose(fp);                                 //fecha arq
    if(r==-1) return NULL;                      //se deu ruim na leitura, XIBU
    int i = 0;
    while(i < resp->nchaves && ch > resp->chave[i]) i++;//busca para ver se está nas chaves do nó recuperado
    if(i < resp->nchaves && ch == resp->chave[i]) return resp;//se achou retorna o nó
    if(resp->folha) return NULL;                //se não tá ali e não tem filhos, acabou
    return Busca(x->filho[i], ch);              //se tem, busca nos filhos
}

TAB *Cria_no(int t){
    TAB* novo = (TAB*)malloc(sizeof(TAB));
    novo->nchaves = 0;
    novo->chave =(int*)malloc(sizeof(int*)*((t*2)-1));
    novo->folha=1;
    novo->filho = (char**)malloc(sizeof((char)*4)*t*2);//lista de strings contendo endereços dos filhos [D2]
    int i;
    for(i=0; i<(t*2); i++) novo->filho[i] = NULL;
    return novo;
}

void imprime_bloco(char *nm){
    if (!nm) return;
    FILE *fp=fopen(nm,"rb");
    TAB *a;
    int r = fread(a,sizeof(TAB),1,fp);
    if(r==-1) exit(1);
    fclose(fp);
    printf("[");
    for(i=0; i<=a->nchaves-1; i++) printf("%d,",a->chave[i]);
    printf("]\n");
    printf("arquivos dos filhos:\n")
    printf("");
    for(i=0; i<=a->nchaves-1; i++) if(a->filho[i]) printf("filho[%d] → %s.dat \n",i,a->filho[i]);
    Libera_no(a);
}