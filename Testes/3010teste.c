#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arv_b_arq.h"
#define T 2

TAB *Cria_no(){
    
    int i;
    TAB *n = (TAB *)malloc(sizeof(TAB));
    n->nchaves = 0;
    n->chave = (int*)malloc(sizeof(int)*((T*2)-1));
    n->folha = 1;
    
    n->filho = (char **)malloc((T*2) * (sizeof(char *)));
    for(i = 0; i < (T*2); i++) n->filho[i] = (char *) malloc(90*sizeof(char));
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
        aux[strlen(aux) + 1] = '\0';
        aux[strlen(aux)] = '\n';
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
    if(!a->folha){
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
        return NULL;                                                         //se não Tá ali e não tem filhos, acabou
    }
    //char *tmp=resp->filho[i];
    char filho_i[90];
    strcpy(filho_i, resp->filho[i]);                                         //Guardando nome do filho para liberar a MP
    //return Busca_arq(resp->filho[i], ch);
    Libera_no(resp);                                                         //mata da MP
    return Busca_arq(filho_i, ch);                                           //se tem, busca nos filhos
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
            char *filho_esq = pega_filho_arq(x, i);
            return Busca(filho_esq, ch);
        }
    }
    
    // não entrou em nenhum caso anterior então a chave buscada é maior que todas as chaves anteriores
    Libera_no(aux);
    char *filho_esq = pega_filho_arq(x, i);
    return Busca(filho_esq, ch);
}

char *guarda(void *end_v, TAB *elem, int tipo){
    if((!end_v) || (!elem) || (tipo > 1) || (tipo < 0)) return NULL;
    
    char * resp = (char *) malloc (sizeof(char) * 90);
    // isso já basta (além do outro retornar erro de compilação)
    
    if(tipo == 0){                                                      // é ponteiro pro contador de arquivo
        int *tmp = (int *) end_v;
        snprintf(resp, 89, "%d.dat", *tmp); 
    }else{                                                              // é string
        char *tmp=(char *) end_v;
        strncpy(resp, tmp, 90);
    }
    Cria(elem, resp);
    return resp;
}

char *Divisao_TAB_MS(char *n_X, int i, char *n_Y, int *nome_atual){
    TAB *Z = Cria_no(T);    // Novo nó Z (direita do nó antigo)
    TAB *Y = recupera(n_Y); // T na chamada, nó antigo antes da divisão (direita)
    TAB *X = recupera(n_X); // S na chamada, "nó de cima"
    
    Z->nchaves = T - 1;     // mínimo de chaves
    Z->folha = Y->folha;    // se o nó T que vai ser dividido é folha, então o novo Z (irmão dele) também vai ser
    
    int j;
    for(j = 0; j < T - 1; j++) Z->chave[j] = Y->chave[j + T]; // copia os valores da direita do nó antigo
    
    if(Y->folha == 0){                                            // se Y(T) for nó interno
        for(j=0; j < T; j++){
            strcpy(Z->filho[j], Y->filho[j + T]);             // copia os filhos
            free(Y->filho[j + T]);                            // libera o filho que acabou de copiar
            Y->filho[j + T] = NULL;                           // seta o ponteiro pra nulo pra n dar problema
        }
    }
    
    Y -> nchaves = T - 1;                                     // o nó antigo passou a ter um número mínimo de chaves assim como seu novo irmão
    n_Y = guarda(n_Y, Y, 1);                                  // atualiza nosso arquivo
    free(n_Y);
    
    int aux = *nome_atual;                                    // Variável "global" da main para fazer um track dos arquivos e eles terem "ids"
    aux++; 
    *nome_atual = aux;
    
    char *n_Z;
    n_Z = guarda(nome_atual, Z, 0);                             // faz o mesmo com Z
    /// 'i' é o número do filho que vamos inserir no nó de cima. Vai ser 1 se o nó for novo
    for(j = X->nchaves; j >= i; j--) X->filho[j + 1] = X->filho[j];
    X->filho[i] = n_Z;
    for(j = X->nchaves;j >= i; j--) X->chave[j] = X->chave[j-1];
    X->chave[i - 1] = Y->chave[T - 1];
    X->nchaves++;
    n_X = guarda(n_X, X, 1);
    Libera_no(X); Libera_no(Y); Libera_no(Z);
    X = recupera(n_X);
    Libera_no(X);
    free(n_Z);
    return n_X;
}

char *Insere_Nao_Completo_TAB_MS(char *n_X, int k, int *nome_atual){
    TAB *X = recupera(n_X);
    int i = X->nchaves - 1;
    if(X->folha == 1){
        while((i >= 0) && (k < X->chave[i])){
            X->chave[i+1] = X->chave[i];
            i--;
        }
        X->chave[i+1]=k;
        X->nchaves++;
        char *resp = guarda(n_X, X, 1);
        Libera_no(X);
        return resp;
    }
    while((i >= 0) && (k < X->chave[i])) i--;
    i++;
    FILE *fp = fopen(X->filho[i], "rb");
    int X_filho_i_nchaves;
    int u = fread(&X_filho_i_nchaves, sizeof(int), 1, fp);
    if(u == -1){
        Libera_no(X);
        return n_X;
    }
    fclose(fp);
    
    if(X_filho_i_nchaves == ((2*T)-1)){
        n_X = Divisao_TAB_MS(n_X, i+1, X->filho[i], nome_atual);
        X = recupera(n_X);
        if(k > X->chave[i]) i++;
    }
    
    X->filho[i] = Insere_Nao_Completo_TAB_MS(X->filho[i], k, nome_atual);
    char *resp = guarda(n_X, X, 1);
    Libera_no(X);
    //free(resp);
    return resp;
}

char *Insere_TAB_MS(char *n_A, int k, int *nome_atual){
    if((!n_A) || (!nome_atual)) return NULL;
    TAB *A = Busca(n_A, k);
    //printf("buscou");
    if(A){                                                                      // já existe, sai
        Libera_no(A);
        return n_A;
    }
    FILE *fp = fopen(n_A, "rb");
    int nchaves;
    if(!fp){                                                                    // ...a árvore Tá nula a.k.a. nome do primeiro arquivo nao consta, cria raiz
        A = Cria_no(T);
        A->chave[0] = k;
        A->nchaves = 1;
        int aux = *nome_atual;                                                  // Variável "global" da main para fazer um track dos arquivos e eles terem "ids"
        aux++; 
        *nome_atual=aux; 
        char *resp = guarda(nome_atual, A, 0);                                  // cria nó inicial e grava na MS
        free(resp);
        Libera_no(A);
        return n_A;
    }                                                                           // ... existe árvore
    int u = fread(&nchaves, sizeof(int), 1, fp);
    if(u == -1) {
        fclose(fp);
        return n_A;
    }
    fclose(fp);
    
    if(nchaves == ((2*T) - 1)){                                             //se T Tá cheio, prepara pra divisão
        TAB *S = Cria_no(T);                                                //criando um nó novo, e colocando o atual cheio
        S->nchaves = 0;                                                     //como seu primeiro filho
        S->folha = 0;
        S->filho[0] = n_A;
        
        int aux=*nome_atual;                                                  // Variável "global" da main para fazer um track dos arquivos e eles terem "ids"
        aux++; 
        *nome_atual=aux;
        
        char *n_S = guarda(nome_atual, S, 0);                                 // grava o nó preparatorio
        char nn_S[90];
        strncpy(nn_S, n_S, 90);                                               // copiamos para uma variável local pois "grava" aloca memória dinamicamente para n_S
        free(n_S);
        n_S = Divisao_TAB_MS(nn_S, 1, n_A, nome_atual);                    // manda dividi-lo
        n_S = Insere_Nao_Completo_TAB_MS(n_S, k, nome_atual);              // insere na estrutura **aqui já grava!!!
        Libera_no(S);
        return n_S;
    }
    n_A = Insere_Nao_Completo_TAB_MS(n_A, k, nome_atual);                 // retorna ok!
    Libera_no(A);
    return n_A;
}

void remove_filho(char *nArq, char *filho){
  if(!nArq || !filho){
    printf("Erro no remove_filho: Nome do arquivo ou do filho inválido.\n");
    return;
  }
  TAB *arv = recupera(nArq);
  int i, flag = 0;
  for(i = 0; i <= arv->nchaves; i++){ 
    if (strcmp(filho, filho[j]) == 0) flag = 1;
    if (flag) filho[j] == filho[j+1];
  }
  Cria(arv, nArq);
  Libera_no(arv);
}

int remover(char *nArq, int ch){
  /**
    Retornos:
    0 - OK!
    1 - Precisa tirar o nome do filho da lista
    2 - Erro
  **/
  
    if(!nArq){
        printf("Erro no remover: Nome do arquivo vazio.\n");
        return 2;
    }
    TAB *arv = NULL;
    arv = recupera(nArq);
    int i;
    for(i = 0; ( (i < arv->nchaves) && (arv->chave[i]) ) < ch; i++);
    if( (i < arv->nchaves) && (ch == arv->chave[i]) ){ //CASOS 1, 2A, 2B e 2C
        if(arv->folha){  //CASO 1
          printf("\nCASO 1\n");
          int j;
          for(j=i; j<arv->nchaves-1;j++) arv->chave[j] = arv->chave[j+1]; //Sobreescreve a chave a ser removido pelas próximas chaves
          arv->nchaves--;
          if(arv->nchaves == 0){
            Libera_no(arv);
            Libera(nArq);
            return 1;
          }
          Cria(arv, nArq);
          Libera_no(arv);
          return 0;
        }
        
        TAB *y = NULL;
        y = pega_filho(arv, i); //Encontrar o predecessor k' de ch na árvore com raiz em y
        if(!y) {
          printf("Erro no remover: Erro ao ler filho y.\n");
          Libera_no(arv);
          Libera_no(y);
          return 2;
        }
        
        if( (!arv->folha) && (y->nchaves >= T) ){ //CASO 2A
          printf("\nCASO 2A\n");
          while(!y->folha) y = pega_filho(y, y->nchaves); // Serve para preparar a função pro caso 3
          int temp = y->chave[y->nchaves-1]; //temp é k'
          char *filho = pega_filho_arq(nArq, i);
          if(!filho) {
            printf("Erro no remover: Filho inválido (Caso 2A).\n");
            Libera_no(arv);
            Libera_no(y);
            return 2;
          }
          arv->chave[i] = temp; //Substitua ch por k' em x
          Cria(arv, nArq);
          Libera_no(arv);
          Libera_no(y);
          int r = remover(filho, temp); //Eliminar recursivamente k'
          if(r == 1) remove_filho(nArq, filho);
          if(r == 2) return 2;
          return 0;
        }
        
        TAB *z = NULL;
        z = pega_filho(arv, i+1); //Encontrar o sucessor k' de k na árvore com raiz em z
        if(!z) {
          printf("Erro no remover: Erro ao ler filho z.\n");
          Libera_no(arv);
          Libera_no(z);
          return 2;
        }
        
        if( (!arv->folha) && (z->nchaves >= T) ){  //CASO 2B
          printf("\nCASO 2B\n");
          while(!z->folha) z = pega_filho(z, 0); // Serve para preparar a função pro caso 3
          int temp = z->chave[0]; //temp é k'
          char *filho = pega_filho_arq(nArq, i+1);
          if(!filho) {
            printf("Erro no remover: Filho inválido (Caso 2B).\n");
            Libera_no(arv);
            Libera_no(z);
            return 2;
          }
          arv->chave[i] = temp; // Substitua ch por k' em x
          Cria(arv, nArq);
          Libera_no(arv);
          Libera_no(z);
          int r = remover(filho, temp); //Eliminar recursivamente k'
          if(r == 1) remove_filho(nArq, filho);
          if(r == 2) return 2;
          return 0;
        }
        if( (!arv->folha) && (z->nchaves == T-1) && (y->nchaves == T-1) ){ //CASO 2C
          printf("\nCASO 2C\n");
          y->chave[y->nchaves] = ch;          //colocar ch ao final de filho[i]
          int j;
          for(j = 0; j < T-1; j++)            //juntar as chaves de y com as de z
            y->chave[T+j] = z->chave[j];
            
          if((y->folha) && (!z->folha)){
            y->folha = 0;
            y->filho = z->filho;
          }
          if((!y->folha) && (!z->folha)){
            
          }
          
          for(j = 0; j <= T; j++)             //juntar os filhos de y com os de z
            y->filho[T+j] = z->filho[j];
            
            
          y->nchaves = 2*T-1;
          for(j=i; j < arv->nchaves-1; j++)   //remover ch de arv
            arv->chave[j] = arv->chave[j+1];
          for(j=i+1; j <= arv->nchaves; j++)  //remover ponteiro para filho[i+1] - Sobrescrever o ponteiro que vai ser removido pelos ponteiros seguintes
            arv->filho[j] = arv->filho[j+1];
          arv->filho[j] = NULL;               //"remover" o último filho que seria de ninguém
          arv->nchaves--;
          arv->filho[i] = remover(arv->filho[i], ch, t);
          return 0;   
        }
        
/*
        for(i = 0;((i < x->nchaves)||(x->chave[i]<k)); i++){ 
          if((x->folha == 0)&&(x->chave[i]==k)) {//Se saiu pelo fato da chave K não estar presente em x e ele é nó interno (a.k.a., chegou aqui(n é raiz) e não é folha) CASO 3
              TAB *f=pega_filho(x,i);
              if(f->nchaves==t-1){
                int w;
                for(w=0;w<x->nchaves;w++){
                  TAB *f=pega_filho(x,i);
                  if(f->nchaves==t){//CASO 3A
                  }
                }
              }
          }
        }
*/

        for(i = 0;((i < x->nchaves)||(x->chave[i]<=k)); i++);
        if((k!=x->chave[i])&&(x->folha == 0)){ 
          int limite_busca=i-1;//O VALOR PODE ESTAR DAQUI PARA TRÁS
          for(i=0;i<=limite_busca;i++){
            TAB *f=pega_filho(x,i);//Se saiu pelo fato da chave K não estar presente em x e ele é nó interno (a.k.a., chegou aqui(n é raiz) e não é folha) e ....
            if(f->nchaves==t-1){///.... o filho onde pode estar tem t-1 chaves , então CASO 3
              int u;
              for(u=0;u<=limite_busca;u++){
                TAB irm=pega_filho(x,u);
                if(irm->nchaves>=t){// Se há algum irmão com t chaves, ele perderá uma chave pro pai //CASO 3A
                  
                }
              }
            }
          }
        }
        //verificar para filho
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
    TAB *raiz = Cria_no();
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
    TAB *filho1 = Cria_no();
    TAB *filho2 = Cria_no();
    
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
    TAB *neto1 = Cria_no();
    TAB *neto2 = Cria_no();
    TAB *neto3 = Cria_no();
    TAB *neto4 = Cria_no();
    TAB *neto5 = Cria_no();
    TAB *neto6 = Cria_no();
    TAB *neto7 = Cria_no();
    
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