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

/*
 =====================================LOG=====================================

***[PP-DÚVIDA]*** Após recuperar os nós dos arquivos, deve-se dar free neles ? [@D3]
***[PP-WARNING]*** possiveis bugs na questão dos endereços char dos filhos ... irei verificar[@ D2]
***[PP-DÚVIDA]*** cond de parada correta?? [@ D1]


 =====================================LOG=====================================
*/

TAB *Cria_no(int t){
    TAB *n=(TAB *)malloc(sizeof(TAB));
    n->nchaves = 0;
    n->chave = (int*)malloc(sizeof(int*)*((t*2)-1));
    n->folha = 1;
    n->filho = malloc((t*2) * (sizeof(char *)));
    int i;
    for(i=0;i<(t*2);i++) n->filho[i] = NULL;       //=malloc(4*sizeof(char));
    return n;
}

char *Cria(TAB *no,char *nome){
    
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
    | arquivos..>|
    +------------+
    
    *** alterar função que mostra do arquivo e criar outra para recuperar dados pra TAB
    */
    
    if((!no)||(!nome)) return NULL; 
    FILE *fp=fopen(nome,"wb");                                            //abre o arquivo de nome "nome"
    if(!fp) return NULL;                                                  //cria o nó
    int i;
    fwrite(&no->nchaves, sizeof(int), 1 , fp);                           //grava nchaves
    fwrite(&no -> folha, sizeof(int), 1, fp);                            //grava se é folha
    for(i=0;i<no->nchaves;i++) fwrite(no->nchaves[i],sizeof(int),1,fp); //grava chaves 
    for(i=0;i<no->nchaves+1;i++){
        int j;                                                        //      ↓ seria isso?
        for(j=0;j<90;j++) fwrite(no->filho[i][j],sizeof(char),1,fp);  //grava caracter-a-caracter os nomes dos arquivos 
    }
    fclose(fp);                                 //fecha o arq
    //for(i=0;i<(t*2);i++) free(n->chave[i]);   // Não há necessidade? - pergola;;há sim, foi cuidado em Libera_no() - PP
    Libera_no(no);                               //Libera da mp
    return nome;
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
    printf("poww!");
    return NULL;
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
    if(!nome) return;                           //vê se o arq existe
    FILE *fp=fopen(nome,"rb");                  //abre arquivo
    if(!fp) return;                             //XIBU
    TAB *a;                                     //recupera do arquivo
    fread(a,sizeof(TAB),1,fp);
    fclose(fp);
    if(a){                                      //imprime recsvmt.
        int i,j;
        for(i=0; i<a->nchaves; i++){            //cond de parada correta?? Não seria i<=a->nchaves ? [D1]
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
    FILE *fp=fopen(nome,"rb");                  //abre o arquivo de nome "nome"
    if(!fp) exit(-1);                           //XIBU
    TAB *a;                                     //recupera o nó para apagar os filhos recursivamente
    fread(a,sizeof(TAB),1,fp);
    fclose(fp);
    if(a){
        int i;
        for(i=0; i<=a->nchaves; i++) Libera(a->filho[i]);//mata filhos
    }
    remove(nome);                               //mata o (arquivo) nó atual
    Libera_no(a);                               //"Free" na MP [D3]

}

TAB *Inicializa(){
    return NULL;
}

TAB *Busca_arq(char *x, int ch){
    TAB *resp = NULL;
    FILE *fp = fopen(x,"rb");
    if(!fp) return resp;
    fread(resp,sizeof(TAB),1,fp);                                //recupera nó atual
    fclose(fp);                                                  //fecha arq
    if(!resp) return resp;
    int i = 0;
    while(i < resp->nchaves && ch > resp->chave[i]) i++;        //busca para ver se está nas chaves do nó recuperado
    if(i < resp->nchaves && ch == resp->chave[i]) return resp;  //se achou retorna o TAB que ele está (retornava o nome do arquivo, alterei para obecer a assinatura) - pergola
    if(resp->folha) return NULL;                                //se não tá ali e não tem filhos, acabou
    char *tmp=resp->filho[i];
    printf("%s\n",tmp);
    return Busca_arq(tmp, ch);                                  //se tem, busca nos filhos
}

TAB *Busca(char* x, int ch){
    TAB *resp = NULL;
    FILE *fp=fopen(x,"rb");
    if(!fp) return NULL;
    int r =fread(resp,sizeof(TAB),1,fp);        //recupera nó atual
    fclose(fp);                                 //fecha arq
    if(r==-1) return NULL;                      //se deu ruim na leitura, XIBU
    int i = 0;
    while( (i < resp->nchaves) && (ch > resp->chave[i]) ) i++;//busca para ver se está nas chaves do nó recuperado
    if( (i < resp->nchaves) && (ch == resp->chave[i]) ) return resp;//se achou retorna o nó
    if(resp->folha) return NULL;                //se não tá ali e não tem filhos, acabou
    char *tmp=resp->filho[i];
    printf("%s\n",tmp);
    return Busca(tmp, ch);              //se tem, busca nos filhos
}

void remover(char *nArq, int ch, int t){
    FILE *fp = fopen(nArq, "rb+");
    if(!fp) exit(1);
    TAB arv;
    int r = fread(arv, sizeof(TAB),1 ,fp);
    fclose(fp);
    if(r != 1) return NULL;
    int i;
    for(i = 0; ( (i < arv->nchaves) && (arv->chave[i]) ) < ch; i++);
    if( (i < arv->nchaves) && (ch == arv->chave[i]) ){ //CASOS 1, 2A, 2B e 2C
        if(arv->folha){ 
            //CASO 1
        }
        if( (!arv->folha) && (arv->filho[i]->nchaves >= t) ){ //CASO 2A
          printf("\nCASO 2A\n");
          TAB *y = arv->filho[i];  //Encontrar o predecessor k' de ch na árvore com raiz em y
          while(!y->folha) y = y->filho[y->nchaves];
          int temp = y->chave[y->nchaves-1];
          arv->filho[i] = remover(arv->filho[i], temp, t); 
          //Eliminar recursivamente K e substitua K por K' em x
          arv->chave[i] = temp;
        }
        if( (!arv->folha) && (arv->filho[i+1]->nchaves >= t) ){ 
            //CASO 2B
        }
        if( (!arv->folha) && (arv->filho[i+1]->nchaves == t-1) && (arv->filho[i]->nchaves == t-1) ){ 
            //CASO 2C
        }
    }
}

void main (){
    return NULL;
}

