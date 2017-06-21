#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main(){
    char string1 [90] ="minha string\n";
    char string2 [90] ="minha string2\n";
    
    FILE *fp = fopen("file.dat", "wb");
    
    fputs(string1, fp);
    fputs(string2, fp);
    fclose(fp);
    
    fp = fopen("file.dat", "rb");
    char lida [90];
    fgets(lida, 90, fp);
    printf("String lida: %s\n", lida);
    fgets(lida, 90, fp);
    printf("String lida2: %s\n", lida);
    fclose(fp);
    
    return 0;
}