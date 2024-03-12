#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(){
    char input[100];
    while(fgets(input,100,stdin)!=NULL){
        printf("当前内容: %s",input);
    }


}