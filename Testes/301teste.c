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

/**
TAB *Insere(char *n_T, int k, int t, int *nome_atual){
  // eu gravo cada atualização no arquivo
  TAB *T = Busca(n_T, k);
  
  if(T) return T;
  if(!T){
    T=Cria_no(t);
    T->chave[0] = k;
    T->nchaves = 1;
    n_T=Cria(T,n_T);
    return T; //**** ????????????
  }
  
  if(T->nchaves == (2*t) - 1){
    TAB *S = Cria_no(t);
    char nn_S[90];
    char *n_S=&nn_S;
    *nome_atual++;      //*****************
    sprintf(n_S,"%d", *nome_atual);  ///ADD CODIGO QUE incrementa nome para novo arquivo criado 
    S->nchaves=0;
    S->folha = 0;
    S->filho[0] = n_T;
    S = Divisao(n_S,S,1,n_T,T,t,nome_atual);//Divisao(n_x, x, (i+1), x->filho[i], x_filho, t);//****
    S = Insere_Nao_Completo(n_S,k,t,nome_atual);
    Cria(S,n_S);
    return S;
  }
  T = Insere_Nao_Completo(n_T,k,t,nome_atual);
  Cria(T,n_T);
  return T;
}

TAB *Insere_Nao_Completo(char *n_x, int k, int t,int *nome_atual){
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
    x = Divisao(n_x, x, (i+1), x->filho[i], x_filho, t,nome_atual); //****
    if(k > x->chave[i]) i++;
  }
  x_filho = Insere_Nao_Completo(x->filho[i], k, t,nome_atual);
  Cria(x, n_x);
  Cria(x_filho, x->filho[i]);
  Libera_no(x_filho); //********
  return x;
}

void mostra_tmp(TAB *a){
    if(!a) {
        printf("→ //");
        return;
    }
    int i;
    printf("[");
    for(i=0;i<a->nchaves;i++) printf(" %d ",a->chave[i]);
    printf("]\n");
    if(!a->folha) {
        printf("[ ");
        for(i=0;i<=a->nchaves;i++) printf(" %s,",a->filho[i]);
        printf("]\n");
    }
}

TAB *Divisao(char *n_x,TAB *x, int i,char *n_y, TAB* y, int t,int *nome_atual){
  //eu gravo cada atualização no arquivo!
  TAB *z = Cria_no(t);
  char nn_z[90]; //ADD CODIGO QUE incrementa nome para novo arquivo criado
  char *n_z=&nn_z;
  *nome_atual++;      //*****************
  sprintf(n_z,"%d",*nome_atual);  
  z->nchaves= t - 1;
  z->folha = y->folha;
  int j;
  for(j=0;j<t-1;j++) z->chave[j] = y->chave[j+t];
  if(!y->folha){
    for(j=0;j<t;j++){
      z->filho[j] = y->filho[j+t];
      free(y->filho[j+t]);//!!!Achtung!!!! Possivel segmentation fault pode ocorrer aqui!
      printf("passei por, liberando ponteiros pros filhos\n");
      y->filho[j+t] = NULL;
    }
  }
  y->nchaves = t-1;
  for(j=x->nchaves; j>=i; j--) x->filho[j+1]=x->filho[j];
  x->filho[i] = Cria(z,n_z);
  for(j=x->nchaves; j>=i; j--) x->chave[j] = x->chave[j-1];
  x->chave[i-1] = y->chave[t-1];
  x->nchaves++;
  Cria(x,n_x);
  Cria(y,n_y);
  Libera_no(z);//***********
  return x;
}


//------------------INSERÇÃO----------------

char *Insere_MS(int *cont_arq,char *nome_a,int elem,int t){
    //Não existe nome
    if((!cont_arq)||(!nome_a)) return NULL;
    TAB *b,*a;
    int i,j;
    char Snome[90];
    char *nome=&Snome;
    sprintf(nome,"%d.dat",*cont_arq);//cont_arq é o ponteiro pro contador do nome do arquivo, que vem da main
    a=recupera(nome_a);
    //ÁRVORE NULA
    if(!a){
        a=Cria_no(t);
        a->chave[0]=x;
        a->nchaves++;
        *cont_arq++;
        return Cria(a,nome);
    }
    //ÁRVORE NÃO NULA E...
        //ELEMENTO JÁ ESTÁ
    b=Busca(nome,x);
    if(b) return nome;
        //ELEMENTO NÃO TÁ, E ELE ...
            //É FOLHA E...
        if((a->folha){
                // se ele têm menos de 2t-1 itens
            if(a->nchaves<(2*t)-1){
                for(i=0;( (i<a->nchaves) || (a->chave[i]<x) );i++); // acha posição de inserção
                if(i!=a->nchaves){
                    for(j=nchaves;j>i;j--) a->chave[j]=a->chave[j-1]; // chega todo mundo pra lá
                }
                a->chave[i]=x; //coloca x na sua devida posição
                char *resp=Cria(a,nome); //coloca de volta na MS
                Libera_no(a);//LIMPA MP
                return resp;
            }else{
                // C.C. e...
                int cont=*cont_arq;//salvando cont arq antes dele ser alterado por divide ms
                a=Divide_MS(nome,t,cont_arq); //A CONTEM APENAS 1 ELEMENTO
                if(!a) exit(1);
                    //É RAIZ
                if(cont<=1){//se nome do arquivo é 1.dat, é pq é o raiz
                    if(x>a->chave[0]){
                        return Insere_MS(cont_arq,a->filho[0],x,t);//INSERE NO FILHO ANTERIOR
                    }else{
                        return Insere_MS(cont_arq,a->filho[1],x,t);//IDEM PRO POSTERIOR
                    }
                }else{
                    //NÃO É RAIZ
                    // fazer o que aqui ????
                    // SERIA INSERIR ELEMENTOS ATÉ QUE DÊ T-1?
                }
            }
            Libera_no(b);
                
        }else{
            //NÃO É FOLHA
        }
}**/

char *guarda(void *end_v, TAB *elem, int tipo){
    if((!end_v) || (!elem) || (tipo > 1) || (tipo < 0)) return NULL;
    
    /**char Sresp[90];
    char *resp = &Sresp;**/
    char *resp; // isso já basta (além do outro retornar erro de compilação)
    
    if(!tipo){                                                          // é ponteiro pro contador de arquivo
        int *tmp=(int *) end_v;
        sprintf(resp,"%d.dat",*tmp);
    }else{                                                              // é string
        char *tmp=(char *) end_v;
        strncpy(resp,tmp,90);
    }
    Cria(elem,resp);
    return resp;
}

char *Divisao_TAB_MS(char *n_X,int i,char *n_Y,int t,int *nome_atual){
    TAB *Z=Cria_no(t),*Y=recupera(n_Y),*X=recupera(n_X);
    Z->nchaves=t-1;
    Z->folha=Y->folha;
    int j;
    for(j=0;j<t-1;j++) Z->chave[j]=Y->chave[j+t];
    if(!Y->folha){
        for(j=0;j<t;j++){
            Z->filho[j]=Y->filho[j+t];
            free(Y->filho[j+t]);
            Y->filho[j+t]=NULL;
        }
    }
    char *n_Z;
    Y->nchaves=t-1;
    n_Y=guarda(n_Y,Y,1);
    
    int aux = *nome_atual;                                                  // Variável "global" da main para fazer um track dos arquivos e eles terem "ids"
    aux++; 
    *nome_atual=aux;
    
    n_Z=guarda(nome_atual,Z,0);
    for(j=X->nchaves;j>=i;j--) X->filho[j+1]=X->filho[j];
    X->filho[i]=n_Z;
    for(j=X->nchaves;j>=i;j--) X->chave[j]=X->chave[j-1];
    X->chave[i-1]=Y->chave[t-1];
    X->nchaves++;
    n_X=guarda(n_X,X,1);
    Libera_no(X);Libera_no(Y);Libera_no(Z);
    return n_X;
}

char *Insere_Nao_Completo_TAB_MS(char *n_X,int k,int t,int *nome_atual){
    TAB *X=recupera(n_X);
    int i=X->nchaves-1;
    if(X->folha){
        while((i>=0)&&(k<X->chave[i])){
            X->chave[i+1]=X->chave[i];
            i--;
        }
        X->chave[i+1]=k;
        X->nchaves++;
        char *resp = guarda(n_X, X, 1);
        Libera_no(X);
        return n_X;
    }
    while((i>=0)&&(k<X->chave[i])) i--;
    i++;
    FILE *fp = fopen(X->filho[i], "rb");
    int X_filho_i_nchaves;
    int u = fread(&X_filho_i_nchaves, sizeof(int), 1, fp);
    if(u == -1){
        Libera_no(X);
        return n_X;
    }
    fclose(fp);
    if(X_filho_i_nchaves == ((2*t)-1)){
        n_X=Divisao_TAB_MS(n_X,i+1,X->filho[i],t,nome_atual);
        if(k>X->chave[i]) i++;
    }
    X->filho[i]=Insere_Nao_Completo_TAB_MS(X->filho[i],k,t,nome_atual);
    char *resp=guarda(n_X,X,1);
    Libera_no(X);
    return resp;
}

char *Insere_TAB_MS(char *n_T,int k,int t,int *nome_atual){
    printf("entrou em insere"); 
    if((!n_T) || (!nome_atual)) return NULL;
    TAB *T = Busca(n_T,k);
    //printf("buscou");
    if(T){                                                              //já existe, sai
        Libera_no(T);
        return n_T;
    }
    printf("Tentou abrir arquivo raiz");                                                                    //NÃO EXISTE e...
    FILE *fp = fopen(n_T, "rb");
    int nchaves;
    if(!fp){                                                            //...a árvore tá nula a.k.a. nome do primeiro arquivo nao consta
        printf("Entrou no if de não existir raiz ainda como esperado");
        T = Cria_no(t);
        T->chave[0] = k;
        T->nchaves = 1;
        int aux=*nome_atual;                                                  // Variável "global" da main para fazer um track dos arquivos e eles terem "ids"
        aux++; 
        *nome_atual=aux; 
        guarda(nome_atual, T, 0);                                       //cria nó inicial e grava na MS
        Libera_no(T);
        return n_T;
    }                                                                   //... existe árvore
    int u = fread(&nchaves, sizeof(int), 1, fp);
    if(u == -1) {
        fclose(fp);
        return n_T;
    }
    fclose(fp);
    
    if(nchaves == ((2*t) - 1)){                                             //se tá cheio, prepara pra divisão
        TAB *S = Cria_no(t);                                              //criando um nó novo, e colocando o atual cheio
        S->nchaves = 0;                                                   //como seu primeiro filho
        S->folha = 0;
        S->filho[0] = n_T;
        int aux=*nome_atual;                                                  // Variável "global" da main para fazer um track dos arquivos e eles terem "ids"
        aux++; 
        *nome_atual=aux;
        char *n_S = guarda(nome_atual, S, 0);                            //grava o nó preparatorio
        n_S = Divisao_TAB_MS(n_S, 1, n_T, t, nome_atual);                     //manda dividi-lo
        n_S = Insere_Nao_Completo_TAB_MS(n_S, k, t, nome_atual);             //insere na estrutura **aqui já grava!!!
        Libera_no(S);
        return n_S;
    }
                                                                        //caso não esteja cheio, apenas insira ajustando
    n_T = (char *) Insere_Nao_Completo_TAB_MS(n_T, k, t, nome_atual);
    Libera_no(T);
    return n_T;
}

int main (){
    
    int cont = 0;
    int resp = 3;
    char *nome;
    printf("Insira valores, inserir -99 causará o encerramento;\n");
    while(resp!=-99){
        printf(">>");
        scanf("%d", &resp);
        printf("\n");
        if(resp==-99) break;
        nome = Insere_TAB_MS("1.dat", resp, 2, &cont);
        printf("retornado : [ %s,cont : %d.dat] \n",nome,cont);
        Imprime_ms(nome,0);
    }
    
    return 0;
}


