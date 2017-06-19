//
//  arv_b_arq.h
//  
//
//  Created by Pedro Teixeira on 16/06/17.
//
//

#include <stdio.h>
#include <stdlib.h>
#include "debugger.h"

typedef struct arvore_b{
    int nchaves, folha, *chave;
    struct char **filho;
}TAB;

/*
    estrutura básica de árvore b, qtd de chaves, se é folha ou não, e ponteiro para o vetor de chaves
    ponteiro para o endereço do arquivo contendo os filhos
 
*/

char *Cria(int t,char *nome);//TESTAR
void Libera(char *nome); //TESTAR
void Imprime(char *nome, int andar);//TESTAR
TAB *Busca(char* x, int ch);
TAB *Inicializa();      //TESTAR
TAB *Divisao(char *x, int i, char* y, int t);
void Insere_Nao_Completo(char *x, int k, int t);
void Insere(char *t, int k, int t);
void remover(char *arv, int ch, int t);
void retira(char *arv, int k, int t)