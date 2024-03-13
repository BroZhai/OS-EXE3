#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
  char suit;
  char val;
}Card;

int main(){
    char input[200];
    char* inputPtr;
    Card Stack[52];
    fgets(input,sizeof(input),stdin);
    // int i;
    // for(i=0;i<1;i++){
    //     printf("%s",&input[i]);
    // }
    
    inputPtr=strtok(input," ");
    int i;
    while(inputPtr!=NULL){
        printf("第一个值:%c 第二个值:%c\n",inputPtr[0],inputPtr[1]);
      inputPtr=strtok(NULL," ");// 传入"NULL"，表示从上一次的位置继续分割字符串，并返回下一个分割出的子字符串的起始位置
    }
    


}