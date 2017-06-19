//
//  arv_b_arq.h
//  
//
//  Created by Pedro Teixeira on 16/06/17.
//
//

const int t = 2;

#include <stdio.h>
#include <stdlib.h>
#include "debugger.h"
#include "arv_b_arq.h"



FILE *Cria(int t,char *nome){
    TAB* novo = (TAB*)malloc(sizeof(TAB));
    
    novo->nchaves = 0;
    novo->chave =(int*)malloc(sizeof(int*)*((t*2)-1));
    novo->folha=1;
    novo->filho = NULL;
    int i;
    return nome;
    
}