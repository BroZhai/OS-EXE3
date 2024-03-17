#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h> 

//Custom a Card Object using typedef, containing the suit and the value attributes of the card
typedef struct
{
  char suit;
  char val;
}Card;

int sCounter;


void play_round(int player_id, int read_fd, int write_fd) {
    char cards[256];
    read(read_fd, cards, sizeof(cards));

    // 处理牌面信息
    // ...

    // 出牌
    char played_card[256];
    // 根据规则决定出牌

    // 发送出牌信息给父进程
    write(write_fd, played_card, strlen(played_card) + 1);
}

//Developed a function that deal the Cards from the Card Stack
void Distribute(Card* Stack, Card* HandStack,int playerIndex){
  int i;
  for(i=playerIndex;i<52;i+=4){
    HandStack[i/4]=Stack[i];
  }
}

//A funtion that print the cards on the player's hand.
void ShowCard(Card* HandStack,int playerIndex){
  printf("Child %d, pid %d: received ",playerIndex+1,getpid());
  int i;
  for(i=0;i<13;i++){ //assume that each player got 13 cards (52 cards in stack)
    printf("%c%c ",HandStack[i].suit,HandStack[i].val);
  }
  printf("\n");
}

//Another print function to show all the card being devied into different groups
void LimitShow(Card* SelectStack,int playerIndex,int size){
  if(size==0){
    return;
  }
  int i;
  for(i=0;i<size;i++){
    printf("%c%c ",SelectStack[i].suit,SelectStack[i].val);
  }
}

//get the calculated point in the SortCard() and print them out according to the format
void ShowPoints(int playerIndex,int points,int adjPoints){
  printf("\nChild %d, pid %d: ",playerIndex+1,getpid());
  printf("%d points, %d adjusted points",points,adjPoints);
}


/*Swap the cards according to their address*/
void CardSwap(Card* A,Card* B){
  Card* temp = malloc(sizeof(Card));
  *temp = *A;
  *A = *B;
  *B = *temp;
}

void InsertSort(Card* SelectStack,Card* targetStack,int size){
  for(int i=0;i<size;i++){
    targetStack[sCounter].suit=SelectStack[i].suit;
    targetStack[sCounter].val=SelectStack[i].val;
    sCounter++;
  }
}

/* J=74, Q=81,
   K=75 (+10) -->85, A=65 (+25) -->90, T=84(-20) -->64 [smallest]
   So that T<J<Q<K<A */

void DescendSort(Card* SelectStack,int size){
  int i,j;
  for(i=0;i<size-1;i++){
    for(j=i+1;j<size;j++){
      char pre=SelectStack[i].val;
      char next=SelectStack[j].val;
      if(pre=='K'){
        pre+=10;
      }if(next=='K'){
        next+=10;
      }if(pre=='A'){
        pre+=25;
      }if (next=='A'){
        next+=25;
      }if(pre=='T'){
        pre-=20;
      }if (next=='T'){
        next-=20;
      }
      if(pre<next){
        CardSwap(SelectStack + i,SelectStack+j);
      }
    }
  }
}

//A function that sort the cards in player's hand and calculate the value for the hand
void SortCard(Card* HandStack,int playerIndex){
  int i;
  printf("Child %d, pid %d: arranged ",playerIndex+1,getpid());

  //Create 4 arrays to store different suit of cards
  Card Sstack[13];
  Card Cstack[13];
  Card Dstack[13];
  Card Hstack[13];
  Card SortedHand[13];
  sCounter=0;
  

  int Valpoints=0;
  int Suitpoints=0;

  DescendSort(HandStack, 13);


  int Scount=0,Ccount=0,Dcount=0,Hcount=0;
  
  for(i=0;i<13;i++){
    if(HandStack[i].suit == 'S'){
      Sstack[Scount]=HandStack[i];
      Scount++;
    }else if(HandStack[i].suit == 'C'){
      Cstack[Ccount]=HandStack[i];
      Ccount++;
    }
    else if(HandStack[i].suit == 'D'){
      Dstack[Dcount]=HandStack[i];
      Dcount++;
    }
    else if(HandStack[i].suit == 'H'){
      Hstack[Hcount]=HandStack[i];
      Hcount++;
    }
  }

  InsertSort(Sstack,SortedHand,Scount);
  InsertSort(Hstack,SortedHand,Hcount);
  InsertSort(Cstack,SortedHand,Ccount);
  InsertSort(Dstack,SortedHand,Dcount);
  //SortedHand[]建立完成！

  for(i=0;i<13;i++){
    printf("%c%c ",SortedHand[i].suit,SortedHand[i].val);
  }



  int totalcards=13;


  printf("\n");
}

int main(int argc, char *argv[]){

  int i;
  //Known that a stack of 52 cards will be input in the command line
  Card Stack[52];
  //Store all the input cards.
  char input[200];
  char* inputPtr;
  fgets(input,200,stdin); //process the input .txt file
  inputPtr=strtok(input," ");
  while (inputPtr!=NULL)
  {
    Stack[i].suit=inputPtr[0];
    Stack[i].val=inputPtr[1];
    i++;
    inputPtr=strtok(NULL," ");
  }
  

  // for(i=0;i<argc-1;i++){
  //   Stack[i].suit=argv[i+1][0];
  //   Stack[i].val=argv[i+1][1];
  // }
  int processID[4];
  int playerID[4];
  int playerReadpipes[4];
  int playerWritepipes[4];
  int playerScores[4];

  //Create and Initialize Pipes for every player
  for(i=0;i<4;i++){
    int pipe_fd[2];
    if(pipe(pipe_fd)==-1){
      printf("Pipe creating exception! The program terminates now!");
      exit(1);
    }
    playerReadpipes[i]=pipe_fd[0];
    playerWritepipes[i]=pipe_fd[1];
  }


  //Using for-loop to create 4 players(childs)
  for(i=0;i<4;i++){
    processID[i]=fork();
    if(processID[i]<0){
      printf("Failed to create the player, the program will terminate now.");
      return 0;
    }else if(processID[i]==0){//What would be done to each player
      Card HandStack[13]; //Construct the hand stack for the player
      Distribute(Stack,HandStack,i); //Extract the specific card from the Stack to player's hand
      ShowCard(HandStack,i); //Initially print the player's hand
      SortCard(HandStack,i); //Group and Calculate the player's hand, then sort
      close(playerReadpipes[i]);
      close(playerWritepipes[(i+1)%4]);
      playerID[i]=getpid();
      // play_round(i+1,playerReadpipes[(i+4-1)%4],playerWritepipes[i]);


      exit(0); //termination of a child process
    }
  }

  //receive exit status from the 4 players (parent)
  for(i=0;i<4;i++){
    wait(NULL);
  }
}

