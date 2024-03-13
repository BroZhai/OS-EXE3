#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char suit;
  char val;
} Card;

int main() {
    char input[200];
    char* inputPtr;
    Card Stack[52];

    fgets(input, sizeof(input), stdin);

    inputPtr = strtok(input, " ");
    while (inputPtr != NULL) {
        for (int i = 0; i < strlen(inputPtr); i += 2) {
            char card[3];
            strncpy(card, &inputPtr[i], 2);
            card[2] = '\0';
            printf("第一个值：%c，第二个值：%c\n", card[0], card[1]);
        }

        inputPtr = strtok(NULL, " ");
    }

    return 0;
}