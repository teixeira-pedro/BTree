//
//  main_arv_b_arq.c
//  
//
//  Created by Pedro Teixeira on 16/06/17.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debugger.h"
#include "arv_b_arq.h"

int v=2;

int narq=0;//nome "incremental" do arquivo

char s_narq[4];
int main(int argc,char **argv){
    narq++;
    TAB *test=Cria_no(v);
    passei("cria v");
    int i;
    passei("add chaves");
    for(i=0;i<(2*v)-1;i++){
    passei("add chaves");
        test->chave[i]=i;
        test->nchaves++;
        iI(test->chave[i]);
    }
    for(i=0;i<(2*v);i++) test->filho[i]=malloc(4*sizeof(char));
    for(i=0;i<(2*v);i++){
        narq++;
        sprintf(s_narq,"%d",narq);
        strncpy(test->filho[i],s_narq,4);
    }
    test->folha=0;
    
    FILE *fp=fopen("0","wb");
    if(fp){
        fwrite(test,sizeof(TAB),1,fp);
        fclose(fp);
        imprime_data(test);
    }
    return 0;
    Libera_no(test);
}


