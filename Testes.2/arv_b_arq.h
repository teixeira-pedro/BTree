//
//  arv_b_arq.h
//
//
//  Created by Pedro Teixeira on 16/06/17.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




typedef struct arvore_b{
    int nchaves, folha, *chave;
    char **filho;
}TAB;

/*
    estrutura básica de árvore b, qtd de chaves, se é folha ou não, e ponteiro para o vetor de chaves
    ponteiro para o endereço do arquivo contendo os filhos

*/

TAB *Cria_no(); //OK
char *Cria(TAB *no,char *nome); //OK - pergola
TAB *recupera (char *nome); //APOS O USO LIBERAR AS ESTRUTURAS RECUPERADAS - OK - pergola
void Libera_no(TAB *a); //OK
void Libera(char *nome); //OK
void Imprime(char *nome, int andar);//ALTERAR
TAB *Busca(char* x, int ch);//testar

TAB *Divisao(char *n_x,TAB *x, int i,char *n_y, TAB* y, int t,int *nome_atual);//TESTAR
TAB *Insere_Nao_Completo(char *n_x, int k,int *nome_atual);//testar
TAB *Insere(char *n_T, int k, int *nome_atual);//testar

char *Busca_arq(char* x, int ch);//TESTAR //Retorna nome do arquivo onde o valor está contido ou n

void imprime_data(TAB *a);//TESTAR //para testes de campo

char *Insere_MS(int *cont_arq,char *nome_a,int elem);
TAB *Divide_MS(char *nome);

char *guarda(void *end_v,TAB *elem,int tipo);
char *Divisao_TAB_MS(char *n_X,int i,char *n_Y,int *nome_atual);
char *Insere_TAB_MS(char *n_T,int k,int *nome_atual);
char *Insere_Nao_Completo_TAB_MS(char *n_X,int k,int *nome_atual);

void remover(char *nArq, int ch, int t);