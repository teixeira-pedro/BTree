
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arv_b_arq.h"
#include "debugger.h"
int narq=0;//nome "incremental" do arquivo

char s_narq[90];

void main (){
    TAB *test=Cria_no(2);
    int i;
    for(i=0;i<test->nchaves+1;i++) test->filho[i] = malloc(90*sizeof(char));
    for(i=0;i<test->nchaves+1;i++) {
        narq++;
        sprintf(s_narq,"%d",narq);
        strncpy(test->filho[i],s_narq,90);
    }
    for(i=0;i<test->nchaves;i++) test->chave[i]=i;
    test->folha=0;
    imprime_data(test);

    printf("arquivo criado: %s \n",Cria(test,s_narq));
    
        
    return 0;
    

}