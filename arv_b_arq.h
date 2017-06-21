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

TAB *Cria_no(int t);//OK
char *Cria(TAB *no,char *nome);//TESTAR - PASSAR O NÓ COMO PARÂMETRO, ESTR. ALTERADA NA GRAVAÇÃO -PP
TAB *Libera_no(TAB *a);//OK
void Libera(char *nome);//testar
void Imprime(char *nome, int andar);//ALTERAR
TAB *Busca(char* x, int ch);//testar
TAB *Inicializa();

TAB *Divisao(char n_x,TAB *x, int i,char *n_y, TAB* y, int t,int *nome_atual);//TESTAR
TAB *Insere_Nao_Completo(char *n_x, int k, int t);//testar
TAB *Insere(char *n_T, int k, int t,int *nome_atual);//testar

void remover(char *nArq, int ch, int t);
void retira(char *arv, int k, int t);


char *Busca_arq(char* x, int ch);//TESTAR //Retorna nome do arquivo onde o valor está contido ou n


void imprime_data(TAB *a);//TESTAR //para testes de campo

TAB *recupera(char *nome);//APOS O USO LIBERAR AS ESTRUTURAS RECUPERADAS
int grava(char *nome,TAB *x);