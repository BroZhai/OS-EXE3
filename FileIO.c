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
      for(i=0;i<strlen(inputPtr);i+=2){
        char card[3];
        strncpy(card,&inputPtr[i],2*sizeof(char));
        card[2]='\0';
        printf("第一个值:%c 第二个值:%c\n",card[0],card[1]);
      }
      inputPtr=strtok(NULL," ");
    }
    


}