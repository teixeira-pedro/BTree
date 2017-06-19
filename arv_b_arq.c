//
//  arv_b.h
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



void Imprime(char *nome, int andar){
    if(a){
        int i,j;
        for(i=0; i<=a->nchaves-1; i++){
            Imprime(a->filho[i],andar+1);
            for(j=0; j<=andar; j++) printf("   ");
            printf("%d\n", a->chave[i]);
        }
        Imprime(a->filho[i],andar+1);
    }
}