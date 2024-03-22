/*I've written my idea as anotation for most of the functions, but I don't know why there are strange bugs appeared...
  which I can not handled eventually... I have made my best pursuiting it T-T*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h> 

//Customized a Card Object using typedef, containing the suit and the value attributes of the card
typedef struct
{
  char suit;
  char val;
}Card;
//Created a Card set that holds the Card object 
typedef struct 
{
  Card CardStack[13];
}CardSet;

int sCounter;
int roundCounter=1;
int processID[4];
int playerIDSet[4]={1,2,3,4};
int playerReadpipes[4]; //Readpipes[Set] for players
int playerWritepipes[4]; //Writepipes[Set] for players
int playerScores[4] = {0}; // use a int[] to store every player's score
CardSet CS[4]; // Create A set containing four player's hand
Card RoundCards[52];   // Array to record the cards played in each round
int roundCardCount = 0; 
int currentPlayer;
int playerReady[4]={0};//Status for each player
int gamePid;

//Developed a function that deal the Cards from the Card Stack
void Distribute(Card* Stack, Card* HandStack,int playerIndex){
  int i;
  for(i=playerIndex;i<52;i+=4){
    HandStack[i/4]=Stack[i];
  }
}

//A funtion that print the cards on the player's hand.
void ShowCard(Card* HandStack,int playerIndex){
  printf("Child %d, pid %d: received ",playerIndex+1,processID[playerIndex]);
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
  printf("Child %d, pid %d: arranged ",playerIndex+1,processID[playerIndex]); 

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

  for(i=0;i<13;i++){
    CS[i].CardStack->suit=SortedHand[i].suit;
    CS[i].CardStack->val=SortedHand[i].val;
  }
  for(i=0;i<13;i++){
  printf("%c%c ",CS[i].CardStack->suit,CS[i].CardStack->val);
  }
  printf("\n");
  
}

//Define a function that execute the round process
void playRound(int playerReadpipes[], int playerWritepipes[]){
  int i; 
    for(i=0;i<4;i++){
      if(playerReady[i]==0){
        //When players are not all ready, the game will not start 
        printf("Player %d cannot discard now, since other players are not ready\n",playerIDSet[i]);
        return;
      }
    }

  // Determine the starting player for each round
  int roundWinner;
  int startingPlayer; 
  if(roundCounter==1){ //rounderCounter starts from "1"
      roundWinner=0;
    }
  startingPlayer=roundWinner;
  currentPlayer=startingPlayer;

  // create a Card object that receives player's discards
  Card playedCard;
  if(roundCounter==1){ //If it is the first round, assign player 1 to be the starter
    gamePid=getpid();
    printf("All players are ready, the game is about to start now!\n");
    printf("Parent pid %d: child players are %d, %d, %d, %d\n",gamePid,processID[0],processID[1],processID[2],processID[3]);
    playedCard.suit = CS[currentPlayer].CardStack[0].suit;
    playedCard.val=CS[currentPlayer].CardStack[0].val;
    printf("Parent pid %d: Round %d ,Child 1 to the lead\n",gamePid,roundCounter);
    printf("Child %d, pid %d: played %c%c\n", playerIDSet[currentPlayer%4],processID[currentPlayer+1], CS[0].CardStack->suit, CS[0].CardStack->val);
    CS[currentPlayer].CardStack[0].suit = 0;
    CS[currentPlayer].CardStack[0].val = 0;
    // write(playerWritepipes[(currentPlayer + 1) % 4], &playedCard, sizeof(Card)); //failed to write back
    RoundCards[0].suit=playedCard.suit;
    RoundCards[0].val=playedCard.val;
    roundCardCount++;
    currentPlayer++;
    //swtich to the next player, locate which card to discard
    int sameSuitIndex = -1;
    int randomSuitIndex= -1;
    for(i=1;i<=3;i++){
      int k;
      for(k=0;k<13;k++){
        // find the card with same suit
        if (RoundCards[0].suit == CS[currentPlayer].CardStack[k].suit) {
        sameSuitIndex = i;
        break;
        } 
      }   
      if (sameSuitIndex != -1) {
        //If the card of same suit is found, discard it.
        playedCard.suit = CS[i].CardStack[sameSuitIndex].suit;
        playedCard.val=CS[i].CardStack[sameSuitIndex].val;
        CS[i].CardStack[sameSuitIndex].suit = 0;
        CS[i].CardStack[sameSuitIndex].val = 0;
      } else {
        //If there are no card with same suit, locate the card with minimum value
       for (k = 0; k < 13; k++) {
          int sValue=CS[0].CardStack->val;
          if (CS[currentPlayer].CardStack->val<sValue) {
          randomSuitIndex = k;
        }
      }
      //discard the card with the minimum value.
      playedCard.suit=CS[i].CardStack[randomSuitIndex].suit;
      playedCard.val=CS[i].CardStack[randomSuitIndex].suit;
      CS[i].CardStack[randomSuitIndex].suit = 0;
      CS[i].CardStack[randomSuitIndex].val = 0;
    }
    //Store the dicarded card into the RoundCards array [related to score calculation]
    RoundCards[roundCardCount].suit=playedCard.suit;
    RoundCards[roundCardCount].val=playedCard.val;
    printf("Child %d, pid %d: played %c%c\n", playerIDSet[i],processID[currentPlayer], CS[i].CardStack[0].suit, CS[i].CardStack[0].val);
    currentPlayer++;
    roundCardCount++;
  }
  //calculate the round score added to the round winner
  int roundScore = 0;
  for (i = roundCardCount-4; i < roundCardCount; i++) {
    if (RoundCards[i].suit == 'H') {
      roundScore += 1;
    } else if (RoundCards[i].suit == 'S' &&RoundCards[i].val=='Q') {
      roundScore += 13;
    }
    if (RoundCards[i].suit == 'H'||(RoundCards[i].suit =='S'&& RoundCards[i].val =='Q')) {
      roundWinner = (startingPlayer + i) % 4;
    }
  }
  playerScores[roundWinner] += roundScore; //add the score to round winner

  //print the round winner message
  printf("Round Winner: Child %d, pid %d\n", roundWinner + 1, processID[roundWinner]);
  printf("Round Score: %d has been added to Player %d\n", roundScore,playerIDSet[roundWinner]);
  roundCounter++;
  }else{
    //not the first round(roundCounter!=1), print the round information
    printf("Parent pid %d: Round %d ,Child %d to the lead\n",gamePid,roundCounter,roundWinner);
    Card* readCard;
    int sameSuitIndex = -1;
    int randomSuitIndex= -1;

    /*failed to read from parent and write back*/ 
    // read(playerReadpipes[(currentPlayer+1)%4],readCard,sizeof(Card));
    // write(playerWritepipes[(currentPlayer + 2) % 4], readCard, sizeof(Card));

    // Same, locate the card with same suit or minimum value card
    for (i = 0; i < roundCardCount; i++) {
      if (RoundCards[i].suit == CS[currentPlayer].CardStack->suit) {
        sameSuitIndex = i;
        break;
      }
    }

    if(sameSuitIndex==-1){
    for (i = 0; i < roundCardCount; i++) {
    int sValue=CS[0].CardStack->val;
    if (CS[currentPlayer].CardStack->val<sValue) {
        randomSuitIndex = i;
        }
      }
    }
    printf("Child %d, pid %d: played %c%c\n", currentPlayer + 1, processID[currentPlayer], playedCard.suit, playedCard.val);

    //Store the dicarded card into the RoundCards array [related to score calculation]
    RoundCards[roundCardCount].suit=playedCard.suit;
    RoundCards[roundCardCount].val=playedCard.val;
    roundCardCount++;
    

    //Same round score calculation strategy
    int roundScore = 0;
    for (i = roundCardCount-4; i < roundCardCount; i++) {
      if (RoundCards[i].suit == 'H') {
        roundScore += 1;
      } else if (RoundCards[i].suit == 'S' && RoundCards[i].val == 'Q') {
        roundScore += 13;
      }
      if (RoundCards[i].suit == 'H' || (RoundCards[i].suit == 'S' && RoundCards[i].val == 'Q')) {
        roundWinner = (startingPlayer + i) % 4;
      }
    }
    playerScores[roundWinner] += roundScore; 

    //print the round winner message
    printf("Round Winner: Child %d, pid %d\n", roundWinner + 1, playerIDSet[roundWinner]);
    printf("Round Score: %d\n", roundScore);

  roundCounter++;
  }
}

int main(int argc, char *argv[]){

  printf("This program is attempting level 2, but failed on rest of the levels. \nPlease check the source code for further referencing...\n");
  printf("I've tried my best, sorry T-T\n");

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
    }else if(processID[i]==0){  //Child process
          // playRound(playerReadpipes,playerWritepipes);
      exit(0); //termination of a child process
    }else{ //Parent Process
      if(roundCounter==1){//Check whether it is the first round
      Card HandStack[13]; //Construct the hand stack for the player
      Distribute(Stack,HandStack,i); //Extract the specific card from the Stack to player's hand
      ShowCard(HandStack,i); //Initially print the player's hand
      SortCard(HandStack,i);
      playerReady[i]=1; //Player Ready
      playRound(playerReadpipes,playerWritepipes);
      }else{ 
        /*failed to read from parent and write back*/ 
        /*Idea: Create another card object that receive the card send from the child process
                Then send to another child process*/
        
        //Card readCard;
        // read(playerReadpipes[(currentPlayer+1)%4],readCard,sizeof(Card));
        // write(playerWritepipes[(currentPlayer + 2) % 4], readCard, sizeof(Card));

        //The recursive rounds going on, until the round reached 13
        if (roundCounter == 13) {
        printf("Final Scores:\n");
        for (i = 0; i < 4; i++) {
          printf("Child %d, pid %d: %d\n", i + 1, playerIDSet[i], playerScores[i]);
            }
         }
      }
    }
  }
  //receive exit status from the 4 players (parent)
  for(i=0;i<4;i++){
      wait(NULL);
  }
  printf("Program terminates here...\n");
}
