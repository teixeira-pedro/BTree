#include <stdlib.h>
#include <string.h>
#include "arv_b_arq.h"

int pega_T(){
  FILE *fp = fopen("T.dat", "rb");
  int t = 0;
  fread(&t, sizeof(int), 1, fp);
  fclose(fp);
  return t;
}


TAB *Cria_no(){
    int T = pega_T();
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
    int T = pega_T();
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
        for(i = 0; i < a->nchaves; i++) Libera(a->filho[i]);//mata filhos
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
    /*if (nchaves == 0){
        printf("Erro no pega_filho_arq: Arquivo de entrada vazio.\n");
        return NULL;
    }*/
    
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
    int T = pega_T();
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
    int T = pega_T();
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
    int T = pega_T();
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
    if (strcmp(filho, arv->filho[i]) == 0) flag = 1;
    if (flag) arv->filho[i] == arv->filho[i+1];
  }
  
  Cria(arv, nArq);
  Libera_no(arv);
}

int remover(char *nArq, int ch){
  
  int T = pega_T(); // <- pega o T do arquivo auxiliar, definida na main inicialmente.
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
  printf("\nRemovendo %d de %s...\n", ch, nArq);
  
  TAB *arv = NULL;
  arv = recupera(nArq);
  if (!arv){ printf("falha ao recuperar arvore\n"); return 2;}
  printf("Recuperou %s.\n", nArq);
  int i;
  printf("Pegando i...\n");
  printf("nchaves de arv = %d\n", arv->nchaves);
  printf("folha de arv = %d\n", arv->folha);
  printf("chave[0] de arv = %d\n", arv->chave[0]);
  printf("chave[1] de arv = %d\n", arv->chave[1]);
  printf("chave[2] de arv = %d\n", arv->chave[2]);
  for(i = 0; i <= arv->nchaves; i++){
    printf("filho %d de arv = %s\n", i, arv->filho[i]);
  }
  for(i = 0; (i < arv->nchaves) && (arv->chave[i] < ch); i++);
  printf("i = %d.\n", i);
  
  if( (i < arv->nchaves) && (ch == arv->chave[i]) ){ //CASOS 1, 2A, 2B e 2C
    if(arv->folha){  //CASO 1
      printf("\nCASO 1\n");
      int j;
      for(j=i; j<arv->nchaves-1;j++) arv->chave[j] = arv->chave[j+1]; //Sobreescreve a chave a ser removido pelas próximas chaves
      arv->nchaves--;
      if(arv->nchaves == 0){
        Libera_no(arv);
        
        printf("Removendo %s.\n", nArq);
        
        Libera(nArq);
        printf("\nentrou aqui5\n");
        
        return 1;
      }
      Cria(arv, nArq);
      Libera_no(arv);
      return 0;
    }
    printf("Não é Caso 1.\n");
    
    TAB *y = NULL;
    y = pega_filho(arv, i); //Encontrar o predecessor k' de ch na árvore com raiz em y
    if(!y) {
      printf("Erro no remover: Erro ao ler filho y.\n");
      Libera_no(arv);
      Libera_no(y);
      return 2;
    }
    printf("Pegou filho y.\n");
    
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
    printf("Não é caso 2A.\n");
    
    TAB *z = NULL;
    z = pega_filho(arv, i+1); //Encontrar o sucessor k' de k na árvore com raiz em z
    if(!z) {
      printf("Erro no remover: Erro ao ler filho z.\n");
      Libera_no(arv);
      Libera_no(y);
      Libera_no(z);
      return 2;
    }
    printf("Pegou filho z.\n");
    
    if( (!arv->folha) && (z->nchaves >= T) ){ //CASO 2B
      Libera_no(y); // Não precisa mais dele se estiver aqui
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
    printf("Não é caso 2B.\n");
    
    if( (!arv->folha) && (z->nchaves == T-1) && (y->nchaves == T-1) ){ //CASO 2C
      printf("\nCASO 2C\n");
      y->chave[y->nchaves] = ch;          //colocar ch ao final de y
      printf("Colocou ch no final de y.\n");
      int j;
      for(j = 0; j < T-1; j++)            //juntar as chaves de y com as de z
        y->chave[T+j] = z->chave[j];
      printf("Juntou as chaves de y com as de z.\n");

      //if((!y->folha) && (!z->folha)){
      for(j = 0; j < T; j++){             //juntar os filhos de y com os de z
        strcpy(y->filho[T+j], z->filho[j]);
      }
      printf("Juntou os filhos de y com os de z.\n");
      //}
        
      y->nchaves = 2*T-1;
      for(j=i; j < arv->nchaves-1; j++)   //remover ch de arv
        arv->chave[j] = arv->chave[j+1];
      printf("Removeu %d de %s.\n", ch, nArq);          

      char filho_a_remover[90];
      strcpy(filho_a_remover, arv->filho[i+1]);
      
      printf("\nÁrvore nchaves: %d\n", arv->nchaves);
      
      for(j=i+1; j < arv->nchaves; j++){  //sobrescrever o nome do filho que vai ser removido pelos nomes seguintes
        printf("Sobrescrevendo %s por %s\n", arv->filho[j], arv->filho[j+1]);
        strcpy(arv->filho[j], arv->filho[j+1]);
        printf("j = %d\n", j);
      }
      arv->filho[j+1] = NULL;               //"remover" o último filho que seria de ninguém
      printf("Removeu o filho z.\n");
      arv->nchaves--;
      
      Cria(arv, nArq);
      Libera_no(arv);
      printf("Liberou %s.\n", nArq);
      char *filhoY = pega_filho_arq(nArq, i);
      Cria(y, filhoY);
      Libera_no(y);
      printf("Liberou %s.\n", filhoY);
      Libera_no(z);
      printf("Nome do filho a ser removido: %s.\n", filho_a_remover);
      TAB* filho = recupera(filho_a_remover);
      Libera_no(filho);
      remove(filho_a_remover);
      //Libera(filho_a_remover);
      
      
      int r = remover(filhoY, ch); // remover recursivamente a ch de y (o z deixa de existir)
      if(r == 1) remove_filho(nArq, filhoY);
      if(r == 2) return 2;
      return 0;   
    }
  }
  
  TAB *y = NULL, *irmao_dir_y = NULL;
  y = pega_filho(arv, i); //Encontrar o predecessor k' de ch na árvore com raiz em y
  
  irmao_dir_y = pega_filho(arv, i + 1);  
  if(!y) {
    printf("Erro no remover: Erro ao ler filho y. >> nome do arquivo apontado : %s \n", arv->filho[i]);
    Libera_no(arv);
    Libera_no(irmao_dir_y);
    Libera_no(y);
    return 2;
  }
  printf("Pegou filho y e seu irmão da direita.\n >> primeiro e ultimo valores do f ");
  
  int z_nchaves;
  if (irmao_dir_y)
    z_nchaves = irmao_dir_y->nchaves;
    
  if (y->nchaves == T - 1){ //CASOS 3A e 3B
    
    if (irmao_dir_y){
      if((i < arv->nchaves) && (irmao_dir_y->nchaves >= T)){ //CASO 3A
        printf("\nCASO 3A: i menor que nchaves\n");
        
        char *n_Y = pega_filho_arq(nArq, i);
        char *n_dir_Y = pega_filho_arq(nArq, i+1);
        
        y->chave[T-1] = arv->chave[i];   //dar a y a chave i da arv
        y->nchaves++;
        arv->chave[i] = irmao_dir_y->chave[0];     //dar a arv uma chave de z
        int j;
        
        for(j=0; j < irmao_dir_y->nchaves-1; j++)  //ajustar chaves de z
          irmao_dir_y->chave[j] = irmao_dir_y->chave[j+1];
          
        strcpy(y->filho[y->nchaves], irmao_dir_y->filho[0]); //enviar ponteiro menor de z para o novo elemento em y
        printf("Tentou strcpy(y->filho[y->nchaves], irmao_dir_y->filho[0]);\n");
        strcpy(y->filho[y->nchaves], irmao_dir_y->filho[0]);   // TODO: TESTAR ISSO
        printf("Conseguiu copiar string\n");
        
        for(j=0; j < irmao_dir_y->nchaves; j++)       //ajustar filhos de z
          strcpy(irmao_dir_y->filho[j], irmao_dir_y->filho[j+1]);
          
        irmao_dir_y->nchaves--;
        char *resp = guarda(n_Y, y, 1); // grava  y
        free(resp);
        
        resp = guarda(n_dir_Y, irmao_dir_y, 1); // grava  irmao direito y
        free(resp);
        free(n_dir_Y);
        
        resp = guarda(nArq, arv, 1);  // grava arv
        free(resp);
        
        Libera_no(irmao_dir_y);
        Libera_no(y);
        Libera_no(arv);
        
        arv = recupera(nArq);
        Libera_no(arv);
        
        return remover(n_Y, ch);
      }
    }
    printf("Não é caso 3A: i menor que nchaves\n");
    
    TAB *irmao_esq_y = NULL;
    irmao_esq_y = recupera(arv->filho[i-1]);
    //if (!irmao_esq_y) {printf("Falha ao abrir o irmão da esquerda de Y\n"); return 2;}
    
    if (irmao_esq_y){
      if((i > 0)  && (irmao_esq_y->nchaves >=T)){ //CASO 3A
      char *n_Y = pega_filho_arq(nArq, i);
      char *n_esq_Y = pega_filho_arq(nArq, i-1);
      printf("\nCASO 3A: i igual a nchaves\n");
      int j;
      for(j = y->nchaves; j > 0; j--)               //encaixar lugar da nova chave
        y->chave[j] = y->chave[j-1];
      for(j = y->nchaves+1; j>0; j--)             //encaixar lugar dos filhos da nova chave
        strcpy(y->filho[j], y->filho[j-1]);
      y->chave[0] = arv->chave[i-1];              //dar a y a chave i da arv
      y->nchaves++;
      arv->chave[i-1] = irmao_esq_y->chave[irmao_esq_y->nchaves-1];   //dar a arv uma chave de irmao_esq_y
      strcpy (y->filho[0], irmao_esq_y->filho[irmao_esq_y->nchaves]);         //enviar ponteiro de irmao_esq_y para o novo elemento em y
      irmao_esq_y->nchaves--;
      
      char *resp = guarda(n_Y, y, 1); // grava  y
      free(resp);
      
      resp = guarda(n_esq_Y, irmao_esq_y, 1); // grava  irmao esq y
      free(resp);
      free(n_esq_Y);
      
      resp = guarda(nArq, arv, 1);  // grava arv
      free(resp);
      
      Libera_no(irmao_esq_y);
      Libera_no(y);
      Libera_no(arv);
      
      
      return remover(n_Y, ch);
      }
    }
    printf("Não é caso 3A: i igual a nchaves\n");
    
    TAB *z = NULL;
    if (irmao_dir_y) z = irmao_dir_y;
    
    if(z){ //CASO 3B
      if(i < arv->nchaves && z->nchaves == T - 1){
        Libera_no(irmao_esq_y);
        
        printf("\nCASO 3B: i menor que nchaves\n");
        
        printf("Pegou z (irmão a direita de y).\n");
        
        y->chave[T - 1] = arv->chave[i];     // pegar chave [i] e coloca ao final de filho[i]
        y->nchaves++;
        int j;
        
        for(j = 0; j < T - 1; j++){
          y->chave[T + j] = z -> chave[j];     // passar filho[i+1] para filho[i]
          y->nchaves++;
        }

        if(y->folha == 0)
          for(j = 0; j < T; j++)             // juntar os filhos de y com os de z
            strcpy(y->filho[T+j], z->filho[j]);

        for(j = i; j < arv->nchaves - 1; j++){ // limpar referências de i
          arv->chave[j] = arv->chave[j+1];
          strcpy(arv->filho[j+1], arv->filho[j+2]); // not working
        }
        
        arv->nchaves--;

        Libera_no(z);
        Cria(y, arv->filho[i]);
        Libera_no(y);
        Cria(arv, nArq);
        Libera_no(arv);
        
        int r = remover(nArq, ch);
        if(r == 2) return 2;
        return 0;
      }
      printf("Não é caso 3B: i menor que nchaves\n");
      
      /**z = pega_filho(arv, i - 1);  
      if(!z) {
        printf("Erro no remover: Erro ao ler filho z.\n");
        Libera_no(arv);
        Libera_no(z);
        Libera_no(y);
        return 2;
      }
      printf("Pegou z (irmão a esquerda de y).\n");**/
    }
    Libera_no(irmao_dir_y);
    if (irmao_esq_y) z = irmao_esq_y;
    else Libera_no(irmao_esq_y);
    
    if (z){
      if((i > 0) && (z->nchaves == T-1)){ 
        //printf("Z = %s\n", pega_filho_arq(nArq, i-1));
        printf("\nCASO 3B: i igual a nchaves\n");
        if(i == arv->nchaves)
          z->chave[T-1] = arv->chave[i-1]; //pegar chave[i-1] e poe ao final de filho[i-1]
        else
          z->chave[T-1] = arv->chave[i];   //pegar chave [i] e poe ao final de filho[i-1]
        z->nchaves++;
        
        int j;
        for(j=0; j < T-1; j++){
          z->chave[T+j] = y->chave[j];     //passar filho[i+1] para filho[i]
          z->nchaves++;
        }
        
        printf("nchaves z: %d\n", z->nchaves);
        for(j = 0; j < z->nchaves; j++){
          printf("Chave[%d] de Z = %d\n", j, z->chave[j]);
        }
        
        if(!z->folha)
          for(j = 0; j < T; j++)             //juntar os filhos de z com os de y
            strcpy(z->filho[T+j], y->filho[j]);
           
        arv->nchaves--;

        
        Cria(z, arv->filho[i-1]);
        Libera_no(z);
        Cria(y, arv->filho[i]);
        Libera_no(y);
        Cria(arv, nArq);
        Libera_no(arv);
        
        int r = remover(nArq, ch);
        if(r == 2) return 2;
        return 0;
      }
      printf("Não é caso 3B: i igual a nchaves\n");
    }else {
      printf("Falha ao ler filho caso 3b remover\n");
    }
  }  
  
  Libera_no(arv);
  if (irmao_dir_y)Libera_no(irmao_dir_y);
  Libera_no(y);
  
  int r = remover(pega_filho_arq(nArq, i), ch);
  if(r == 2) return 2;
  
  return 0;
  
}

void conserta_raiz(char* nArq){
  printf("Consertando raiz...\n");
  TAB *arv = recupera(nArq);
  printf("Recuperou %s.\n", nArq);
  char nome_filho[90];
  strcpy(nome_filho, arv->filho[0]);
  printf("Copiou nome do filho.\n");
  TAB *filho = recupera(nome_filho);
  arv = filho;
  printf("%s recebeu %s.\n", nArq, nome_filho);
  Cria(arv, nArq);
  printf("%s foi criado.\n", nArq);
  Libera_no(arv);
  printf("Liberou raiz.\n");
  /*Libera_no(filho);
  printf("Liberou filho.\n");*/
  remove(nome_filho);
  printf("Removeu filho.\n");
}

int main (){
    
    int cont = 0;
    int resp = -1;
    int limpou = 0;
    int i = 1;
    char *raiz = (char *) malloc(sizeof(char) * 90);
    char raiz_aux[90];
    
    // define t
    printf("Escolha seu t: (-99 se preferir carregar arquivos já existentes)\n");
    printf(">>");
    scanf("%d", &resp);
    if (resp == -99){
        
        printf("Nome do arquivo raiz já existente: \n");
        printf(">>");
        scanf("%s", raiz_aux);
        printf("%s\n", raiz_aux);
        strncpy(raiz, raiz_aux, 90);
        printf("Arquivo carregado!\n");
        
    }else{
        strcpy(raiz, "1.dat");
        /** limpa arquivos antes de começar para garantir integridade do sistema **/
        /** Opcional: não fazer isso e conseguir carregar dos arquivos. **/
    
        while (limpou == 0){
            char aux [90];
            sprintf(aux, "%d.dat", i);
            limpou = remove(aux);    
            i++;
        }
        // limpa arquivo auxiliar se existir
        remove("T.dat");    
        
        FILE *fp = fopen("T.dat", "wb");
        fwrite(&resp, sizeof(int), 1, fp);
        fclose(fp);
        resp = -1;
        printf("t definido! Estamos prontos para começar...\n");
    }
    
    do{
        printf("1 para inserir, 2 para remover, 3 para imprimir e -99 para sair.\n");
        printf(">>");
        scanf("%d", &resp);
        printf("\n");
        if(resp==-99){ break;
        }else if(resp == 1){
            while (resp != -99){
                printf("Insira o valor que deseja inserir. -99 para voltar ao menu principal.\n");
                printf(">>");
                scanf("%d", &resp);
                if(resp == -99) { resp = -1; break;}
                raiz = Insere_TAB_MS(raiz, resp, &cont);
            }
        }else if(resp == 2){
            while (resp != -99){
                printf("Insira o valor que deseja remover. -99 para voltar ao menu principal.\n");
                printf(">>");
                scanf("%d", &resp);
                if(resp == -99) { resp = -1; break;}
                remover(raiz, resp);
                TAB *arv = recupera(raiz);
                if (!arv){  printf("Retirou todos elementos da árvore! Comece outra vez...\n"); cont = 0; strcpy(raiz, "1.dat");}
                else if(arv->nchaves == 0) conserta_raiz(raiz);
                Libera_no(arv);
            }
        }else if(resp == 3){
            printf("\t\t Mostrando arquivo raiz '%s'!!\n\n", raiz);
            Imprime_ms(raiz, 0);    
        }else {
            printf("Valor inválido, tente novamente\n");
        }
    }while(resp!=-99);
    
    return 0;
}