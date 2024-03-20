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
int roundCounter=1;
int processID[4];
int playerID[4];
int playerReadpipes[4];
int playerWritepipes[4];
int playerScores[4];

void playRound(int playerReadpipes[], int playerWritepipes[], Card SortedHand[]){
  /*在整理好玩家的手牌SortedHand[]之后，现在我们要实现玩家打牌
  如果是第一轮roundCounter=1，那么先手玩家可以出除红心以外任和的牌，默认是value最小的那张牌
  当第一轮的先手玩家出完牌后，这个牌的对象信息会被传到父进程，父进程首先会print出这个玩家打了什么牌，再这个牌的信息传递给下一个子进程(玩家)，以此类推
  当玩家打出这张牌后，对应的，这个玩家自己的手牌数组SortedHand[]中的这张牌会被删除(suit和value都置为0)
  此时下一轮的玩家需要打出与上一轮相同花色但value最小的牌，如果没有相同花色的牌，那么可以打任意一张牌，对于剩下的玩家也是
  同样的规则，直到所有玩家打完牌，这一轮的牌局结束，然后父进程会根据这一轮的牌局情况来决定下一轮的先手玩家
  用一个Card数组来记录这一局打出的所有牌对象，然后根据这个数组来判断谁是RoundWinner，如果这个Card中有红心牌，那么对于这个RoundWinner来说，每个红心牌+1分，还有每个黑桃Q+13分。下一轮会以RoundWinner为先手玩家
  开始下一轮游戏，此时roundCounter!=1，对于之后的进程来说可以算是进入了一个循环，直到所有玩家的手牌都打完，游戏结束*/
  // Array to record the cards played in each round
  Card RoundCards[52]; //定义一种(总体的)卡组，记录每回合（4轮)出牌的情况，共13个回合(52轮)
  int roundCardCount = 0; //总出卡counter
  int i;
  int currentPlayer;

  // 用int[]记录和初始化每个玩家的初始分数
  int playerScores[4] = {0};

  // Determine the starting player for each round
  int roundWinner;
  if(roundCounter==1){
    roundWinner = 0;
  }
    int startingPlayer; //= roundCounter == 1 ? 0 : roundWinner;
    if(roundCounter==0){
      startingPlayer=1; //玩家"指代数"尚不清楚
    }else{
      startingPlayer=roundWinner;
    }

  // 使用for循环去实现：每轮四个玩家轮流出牌
  for (currentPlayer = startingPlayer; currentPlayer < 4; currentPlayer++) {
    // Check if the current player has any cards left
    if (SortedHand[currentPlayer].suit == 0 && SortedHand[currentPlayer].val == 0) {
      continue; // 判断玩家是否还有牌"剩余"，需要重写
    }

    // 查找当前玩家是否有"能出"的牌(花色一样的牌)，并进行定位
    int sameSuitIndex = -1;
    for (i = 0; i < roundCardCount; i++) {
      if (RoundCards[i].suit == SortedHand[currentPlayer].suit) {
        sameSuitIndex = i;
        break;
      }
    }

    // 创建一个玩家将要打出的Card对象
    Card playedCard;
    if (sameSuitIndex != -1) {
      //如果 有能出 同花色 的牌，"打出"玩家SortedHand[]中的牌(置suit和value为0)
      //这块逻辑不对，需要重写(想办法用到sameSuitIndex这个参数在SortedHand[]中定位)
      playedCard = SortedHand[currentPlayer];
      SortedHand[currentPlayer].suit = 0;
      SortedHand[currentPlayer].val = 0;
    } else {
      //如果 没得同花色 的牌，直接找到SortedHand中最小的出就好了
      //这块的逻辑也要重写
      playedCard = SortedHand[currentPlayer];
      SortedHand[currentPlayer].suit = 0;
      SortedHand[currentPlayer].val = 0;
    }

    //将每个玩家在当轮打出的卡记录到总回合数RoundCards[]中，同时roundCardCount++
    RoundCards[roundCardCount] = playedCard; 
    roundCardCount++;

    //打印出当前玩家的出牌信息
    printf("Child %d, pid %d: played %c%c\n", currentPlayer + 1, getpid(), playedCard.suit, playedCard.val);

    // 将玩家打出的卡传回到 父进程中 [传入playedCard的地址]
    write(playerWritepipes[(currentPlayer + 1) % 4], &playedCard, sizeof(Card));
  }

  // Determine the round winner based on the RoundCards array
  int roundScore = 0;
  for (i = 0; i < roundCardCount; i++) {
    if (RoundCards[i].suit == 'H') {
      roundScore += 1;
    } else if (RoundCards[i].suit == 'S' && RoundCards[i].val == 'Q') {
      roundScore += 13;
    }
    if (RoundCards[i].suit == 'H' || (RoundCards[i].suit == 'S' && RoundCards[i].val == 'Q')) {
      roundWinner = (startingPlayer + i) % 4;
    }
  }

  // Update the scores of the players
  playerScores[roundWinner] += roundScore;

  // Print the round winner and their score
  printf("Round Winner: Child %d, pid %d\n", roundWinner + 1, playerID[roundWinner]);
  printf("Round Score: %d\n", roundScore);

  // Update the round counter
  roundCounter++;

  // Check if the game is over
  if (roundCounter > 13) {
    // Print the final scores of the players
    printf("Final Scores:\n");
    for (i = 0; i < 4; i++) {
      printf("Child %d, pid %d: %d\n", i + 1, playerID[i], playerScores[i]);
    }
  }
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



/*Swap the cards according to their address*/
void CardSwap(Card* A,Card* B){
  Card* temp = malloc(sizeof(Card));
  *temp = *A;
  *A = *B;
  *B = *temp;
}

void InsertSort(Card* SelectStack,Card* targetStack,int size){
  int i;
  for(i=0;i<size;i++){
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
void SortCard(Card* HandStack,int playerIndex, int playerReadpipes[], int playerWritepipes[]){
  int i;
  printf("Child %d, pid %d: arranged ",playerIndex+1,getpid());

  //Create 4 arrays to store different suit of cards
  Card Sstack[13];
  Card Cstack[13];
  Card Dstack[13];
  Card Hstack[13];
  Card SortedHand[13];
  sCounter=0;
  
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
    }else if(processID[i]==0){  //What would be done to each player
      Card HandStack[13]; //Construct the hand stack for the player
      Distribute(Stack,HandStack,i); //Extract the specific card from the Stack to player's hand
      ShowCard(HandStack,i); //Initially print the player's hand
      SortCard(HandStack,i,playerReadpipes,playerWritepipes); //Group and Calculate the player's hand, then sort
      close(playerReadpipes[i]); //close the read pipe
      close(playerWritepipes[(i+1)%4]); //close the write pipe
      playerID[i]=getpid();
      playRound(playerReadpipes, playerWritepipes, HandStack);

      exit(0); //termination of a child process
    }
  }

  //receive exit status from the 4 players (parent)
  for(i=0;i<4;i++){
    wait(NULL);
  }
}

