#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "stdio.h"

char* stringDuplicator(char* string, int times){
    assert(string);
    assert(times>0);
    int len = strlen(string);
    char* out = malloc(len*times*sizeof(char)+1);
    if (out==NULL){
        return NULL;
    }
    char* out_return=out;
    for(int i=0; i<times;i++){
        strcpy(out,string);
        out+=len;
    }
    return out_return;
}

int main(){
    char* s="place now\n";
    char* d=stringDuplicator(s,3);
    printf("%s",d);
}