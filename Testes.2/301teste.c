#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "arv_b_arq.h"
#define T 3

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

int main (){
    
    int cont = 0;
    int resp = -1;
    int limpou = 0;
    int i = 1;
    
    /** limpa arquivos antes de começar **/
    while (limpou == 0){
        char aux [90];
        sprintf(aux, "%d.dat", i);
        limpou = remove(aux);    
        i++;
    }
    
    char *raiz = "1.dat";
    do{
        printf("1 para inserir, 2 para remover, 3 para imprimir e -99 para sair.\n");
        printf(">>");
        scanf("%d", &resp);
        printf("\n");
        if(resp==-99){ break;
        }else if()
        
        raiz = Insere_TAB_MS(raiz, resp, &cont);
        printf("retornado : [ %s, cont : %d.dat] \n", raiz, cont);
        Imprime_ms(raiz, 0);    
    }while(resp!=-99);
        
    /** limpa arquivos antes de começar **/
    i = 1;
    while (i <= cont){
        char aux [90];
        sprintf(aux, "%d.dat", i);
        remove(aux);    
        i++;
    }
    
    return 0;
}