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
***[NQ-DÚVIDA]*** Verificar se isso funciona na função da Rosseti [@ D4]


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
    | arquivos>  |
    +------------+
    
    **/

    if((!no)||(!nome)) return NULL; 
    FILE *fp=fopen(nome,"wb");                                            //abre o arquivo de nome "nome"
    if(!fp) return NULL;                                                  //cria o nó
    int i;
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
    FILE *fp=fopen(x,"rb");
    if(!fp) return NULL;
    TAB *resp = NULL;
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

TAB *pega_filho(TAB *arv, int qualFilho){
  if(!arv){
    printf("Erro no pega_filho: Árvore vazia.\n");
    return NULL;
  }
  FILE *fpFilho = fopen(arv->filho[qualFilho], "rb");
  if(!fpFilho) exit(1);
  TAB *resp = NULL;
  r = fread(resp, sizeof(TAB), 1, fpFilho);
  fclose(fpFilho);
  if(r != 1) {
    printf("Erro no pega_filho: Erro ao ler filho.\n");
    return NULL;
  }
  return resp;
}

void remover(char *nArq, int ch, int t){
    FILE *fp = fopen(nArq, "rb");
    if(!fp) exit(1);
    TAB *arv = NULL;
    int r = fread(arv, sizeof(TAB), 1,fp);
    fclose(fp);
    if(r != 1) return NULL;
    int i;
    for(i = 0; ( (i < arv->nchaves) && (arv->chave[i]) ) < ch; i++);
    if( (i < arv->nchaves) && (ch == arv->chave[i]) ){ //CASOS 1, 2A, 2B e 2C
        if(arv->folha){ 
            //CASO 1
        }
        
        TAB *y = NULL;
        y = pega_filho(arv, i); //Encontrar o predecessor k' de ch na árvore com raiz em y
        if(!y) {
          printf("Erro no remover: Erro ao ler filho y.\n");
          return;
        }
        
        if( (!arv->folha) && (y->nchaves >= t) ){ //CASO 2A
          printf("\nCASO 2A\n");
          while(!y->folha) y = pega_filho(y, y->nchaves);
          int temp = y->chave[y->nchaves-1]; //temp é k'
          TAB *filho = pega_filho(arv, i);
          if(!filho) {
            printf("Erro no remover: Erro ao ler filho (Caso 2a).\n");
            return;
          }
          filho = remover(filho, temp, t); //Eliminar recursivamente k' 
          arv->chave[i] = temp; //Substitua ch por k' em x
          Cria(arv, nArq);
        }
        
        TAB *z = NULL;
        z = pega_filho(arv, i+1); //Encontrar o sucessor k' de k na árvore com raiz em z
        if(!z) {
          printf("Erro no remover: Erro ao ler filho z.\n");
          return;
        }
        
        if( (!arv->folha) && (z->nchaves >= t) ){  //CASO 2B
          printf("\nCASO 2B\n");
          while(!z->folha) z = pega_filho(z, 0);
          int temp = z->chave[0]; //temp é k'
          z = remover(pega_filho(arv, i+1), temp, t); //Eliminar recursivamente k' [D4]
          arv->chave[i] = temp; // Substitua ch por k' em x
          Cria(arv, nArq);
        }
        if( (!arv->folha) && (z->nchaves == t-1) && (y->nchaves == t-1) ){ 
            //CASO 2C
        }
    }
}

TAB *Insere_Nao_Completo(char *n_x, int k, int t){
  int i = x->nchaves-1;
  
  if(x->folha){
    while((i>=0) && (k<x->chave[i])){
      x->chave[i+1] = x->chave[i];
      i--;
    }
    x->chave[i+1] = k;
    x->nchaves++;
    return x;
  }
  while((i>=0) && (k<x->chave[i])) i--;
  i++;
  if(x->filho[i]->nchaves == ((2*t)-1)){
    x = Divisao(x, (i+1), x->filho[i], t);
    if(k>x->chave[i]) i++;
  }
  x->filho[i] = Insere_Nao_Completo(x->filho[i], k, t);
  return x;
}

void main (){
    return NULL;
}


