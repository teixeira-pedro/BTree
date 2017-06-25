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

***[PP-DÚVIDA]*** Após recuperar os nós dos arquivos, deve-se dar free neles ? R: Não, por que faria isso? Damos free quando não precisamos mais só. -pergola
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
    FILE *fp = fopen(nome,"wb");                                         //abre o arquivo de nome "nome"
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
    while( (i < resp->nchaves) && (ch > resp->chave[i]) ) i++;               // busca para ver se está nas chaves do nó recuperado
    if( (i < resp->nchaves) && (ch == resp->chave[i]) ) return x;            // se achou retorna o endereço
    if(resp->folha) {
        Libera_no(resp);                                                     // mata da MP
        return NULL;                                                         // se não tá ali e não tem filhos, acabou
    }
    //char *tmp=resp->filho[i];
    char filho_i[90];
    strcpy(filho_i, resp->filho[i]);                                         // Guardando nome do filho para liberar a MP
    //return Busca_arq(resp->filho[i], ch);
    Libera_no(resp);                                                         // mata da MP
    return Busca_arq(filho_i, ch);                                           // se tem, busca nos filhos
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

TAB *Busca(char* x, int ch){
    if (!x) return NULL;    // Nome inexistente
    TAB *aux = NULL;
    aux = recupera(x);
    if (!aux) return NULL;  // XIBU
    
    // procura a chave nessa TAB
    int i;
    for (i = 0; i < aux->nchaves; i++)
        if (ch == aux -> chave[i]) return aux;    // achou a exata chave então retorna TAB sem preocupação
    
    // Nó atual é folha e n encontrou a chave
    if (aux -> folha == 1) return NULL;
    
    // não achou a chave nessa TAB então vamos ver o qual é o único filho que a chave buscada pode estar
    for (i = 0; i < aux -> nchaves; i++){
        if (ch < aux -> chave[i]){
            // se chave buscada for menos que chave analisada, buscamos o único filho q ela possivelmente esteja
            // não é folha então pega o filho da esquerda (que tem o mesmo indice atual i) e usa a função recursiva nele
            // lembrando de liberar o nó auxiliar para não ocuparmos memória desnecessariamente.
            Libera_no(aux);
            char *filho_esq =  pega_filho_arq(x, i);
            return Busca(filho_esq, ch);
        }
    }
    
    // não entrou em nenhum caso anterior então a chave buscada é maior que todas as chaves anteriores
    Libera_no(aux);
    char *filho_esq = pega_filho_arq(x, i);
    return Busca(filho_esq, ch);
}

void Imprime(char *nome, int andar){
    
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
    arv = recupera(nArq);
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
          Libera_no(arv);
          Libera_no(y);
          return;
        }
        
        if( (!arv->folha) && (y->nchaves >= t) ){ //CASO 2A
          printf("\nCASO 2A\n");
          while(!y->folha) y = pega_filho(y, y->nchaves); // Serve para preparar a função pro caso 3
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
        z = pega_filho(arv, i+1); //Encontrar o sucessor k' de k na árvore com raiz em z
        if(!z) {
          printf("Erro no remover: Erro ao ler filho z.\n");
          Libera_no(arv);
          Libera_no(z);
          return;
        }
        
        if( (!arv->folha) && (z->nchaves >= t) ){  //CASO 2B
          printf("\nCASO 2B\n");
          while(!z->folha) z = pega_filho(z, 0); // Serve para preparar a função pro caso 3
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

TAB *Insere(char *n_T, int k, int t, int *nome_atual){
  // eu gravo cada atualização no arquivo
  TAB *T = Busca(n_T, k);
  if(T) return T;
  if(!T){
    T=Cria_no(t);
    T->chave[0] = k;
    T->nchaves = 1;
    n_T=Cria(T,n_T);
    return T;//****
  }
  if(T->nchaves == (2*t) - 1){
    TAB *S = Cria_no(t);
    char n_S[90];
    *nome_atual++;      //*****************
    sprintf(&n_S,"%d", nome_atual);  ///ADD CODIGO QUE incrementa nome para novo arquivo criado 
    S->nchaves=0;
    S->folha = 0;
    S->filho[0] = n_T;
    S = Divisao(n_S,S,1,n_T,T,t);//Divisao(n_x, x, (i+1), x->filho[i], x_filho, t);//****
    S = Insere_Nao_Completo(&n_S,k,t);
    grava(n_s,S);
    return S;
  }
  T = Insere_Nao_Completo(n_T,k,t);
  grava(n_T,T);
  return T;
}

TAB *Divisao(char n_x,TAB *x, int i,char *n_y, TAB* y, int t,int *nome_atual){
  //eu gravo cada atualização no arquivo!
  TAB *z = Cria_no(t);
  char n_z[90]; //ADD CODIGO QUE incrementa nome para novo arquivo criado 
  *nome_atual++;      //*****************
  sprintf(&n_z,"%d",nome_atual);  
  z->nchaves= t - 1;
  z->folha = y->folha;
  int j;
  for(j=0;j<t-1;j++) z->chave[j] = y->chave[j+t];
  if(!y->folha){
    for(j=0;j<t;j++){
      z->filho[j] = y->filho[j+t];
      free(y->filho[j+t]);//!!!Achtung!!!! Possivel segmentation fault pode ocorrer aqui!
      printf("passei por, liberando ponteiros pros filhos\n")
      y->filho[j+t] = NULL;
    }
  }
  y->nchaves = t-1;
  for(j=x->nchaves; j>=i; j--) x->filho[j+1]=x->filho[j];
  x->filho[i] = Cria(z,&n_z);
  for(j=x->nchaves; j>=i; j--) x->chave[j] = x->chave[j-1];
  x->chave[i-1] = y->chave[t-1];
  x->nchaves++;
  grava(n_x,x);
  grava(n_y,y);
  Libera_no(z);//***********
  return x;
}

TAB *Insere_Nao_Completo(char *n_x, int k, int t){
  // eu gravo cada atualização no arquivo!
  TAB *x = recupera(n_x);
  int i = x -> nchaves - 1;
  // Se já achou uma folha, você sabe qual nó inserir, então vamos inserir :)
  if(x->folha == 1){
    while((i >= 0) && (k < x->chave[i])){       // itera ao contrário 
        x->chave[i + 1] = x->chave[i];          // "empurra" os elementos do nó folha pra direita
        i--;
    }
    // está na posição que devemos inserir a chave nova
    x->chave[i+1] = k;
    x->nchaves++;
    Cria(x, n_x);
    return x;
  }
  // Caso não seja folha, vamos procurar qual folha inserir
  while((i >= 0) && (k < x->chave[i])) i--;
  i++;
  TAB *x_filho = recupera(x->filho[i]);
  if(x_filho -> nchaves == ((2*t)-1)){ // filho está cheio
    x = Divisao(n_x, x, (i+1), x->filho[i], x_filho, t); //****
    if(k > x->chave[i]) i++;
  }
  x_filho = Insere_Nao_Completo(x->filho[i], k, t);
  Cria(x, n_x);
  Cria(x_filho, x->filho[i]);
  Libera_no(x_filho); //********
  return x;
}

int main (){
    printf("Arvore B\n");
    printf("Insira chave inicial: \n");
    int chave = 0;
    scanf("%d", &chave);
    printf("Chave inicial = %d", chave);
    return 0;
}


