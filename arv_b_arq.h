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
char *Cria(int t,char *nome);//TESTAR
TAB *Libera_no(TAB *a);//OK
void Libera(char *nome);//testar
void Imprime(char *nome, int andar);//testar
TAB *Busca(char* x, int ch);//testar
TAB *Inicializa();

TAB *Divisao(char *n_x, int i, char* n_y, int t,int m_atual);
void Insere_Nao_Completo(char *x, int k, int t);
char *Insere(char *n, int k, int t,int nm_atual);

void remover(char *arv, int ch, int t);
void retira(char *arv, int k, int t);


TAB *Busca_arq(char* x, int ch);//TESTAR //Retorna nome do arquivo onde o valor está contido ou n


void imprime_data(TAB *a);//TESTAR //para testes de campo