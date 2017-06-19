#include <stdio.h>
#include <stdlib.h>

void passei(char * mensagem){
    printf("passei em %s \n",mensagem);
}
void ehNaoEh(int x){
    if(!x) printf(" não é ");return;
    printf(" é ");
}

void pp(char *msg){
    printf("%s\n",msg);
}
void ii(int x){
    printf(" %d ",x);
}

void pl(){
    printf("\n");
}
void s_n(int x){
    if(x) {
        printf(" sim");
    }else{
        printf(" nao");
    }
}

void pp_ch(char x){
	printf(" %c ",x);
}

void pp_nao_li(char *msg){
    printf(" %s ",msg);
}

void poww(){
	pp("pow!");
}
void ehNaoEhNull(void *obj,char *nome){
	if(!obj) printf(" o objeto %s é // .",nome);
	else printf(" o objeto %s NÃO é // .",nome);
}
void retirei(){
    printf("retirei o ");
}

void inseri(){
    printf("inseri o ");
}

void naFila(){
    printf("na fila ");
}

void naPilha(){
    printf("na pilha ");
}

void naPilhaVet(){
    printf("na pilhaVet ");}

void naFilaVet(){
    printf("na filaVet ");}
void na(){
    printf("Função ou método em construção, nenhum dado foi alterado\n");
    exit(1);
}

void iI(int x){
    ii(x);
}
void mensagemConstr(void){
    printf("Função ainda em construção. Nenhum dado foi alterado.\n");
    printf("Para mais informações verifique o arquivo \".h \" da biblioteca que está usando\n");
}
void aboutLC(){
    printf("\n===========================LISTAS CIRCULARES==============================\nUso e implementação igual ao de lista encadeada único cuidado que teremos\né que no ultimo elemento ao invés dele apontar pro nulo, ele apontará pro\n\"primeiro\" (token).\n\nl→\\/\\/\n\nl→[ ]→\n/\ ← ↓\n\nl→[ ]→[ ]→[ ]→[ ]→\n/\              ↓\n|              ↓\n+--------------+\n===========================LISTAS CIRCULARES==============================\n\n");
}

void aboutLDE(){
    printf("\n\n====================LISTAS DUPLAMENTE ENCADEADAS==========================\nAqui teremos que tomar cuidado com o encadeamento duplo, tanto para a\nesquerda para a direita, deve se ajustar o encadeamento para o anterior\ne para o próximo\n\n\nl→//\n\n\nl\n↓\n//←[ |  | ]→//\n\n\nl\n↓\n//←[ |  | ]←→[ |  | ]←→[ |  | ]→//\n====================LISTAS DUPLAMENTE ENCADEADAS==========================\n\n");
}
void impInt(int x){
    ii(x);
}
void passeiVaz(){
    passei("");
}


void igualA(){
    printf("igual a ");
}
void powww(){
    printf("**Poow!**\n");
}
void imp_vet_int(int *v,int n){
	if((!v)||(n<=0)) printf("[]");
	int i;
	printf("[");
	for(i=0;i<n;i++){
		if(i==n-1) printf(" %d ]\n",v[i]);
		else printf(" %d ,",v[i]);
	}
}
