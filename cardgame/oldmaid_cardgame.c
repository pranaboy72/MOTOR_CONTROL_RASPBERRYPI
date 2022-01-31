#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void PrintCard(int* p, int length);
void PrintAllMemberCard(int* A, int* B, int* C, int* D, int* cardNum);
void CreateCardStack(int* cardStack);
void CardDistribute(int* A, int* B, int* C, int* D, int* cardStack, int* cardNum);
void ArrayPop(int* arr, int* arrLength, int popIndex);
void ArrayPush(int* arr, int* arrLength, int num);
void RemovePairedCard(int* holdingCard, int* cardNum);
void Game(int* A, int* B, int* C, int* D, int* cardNum, int* playerOrder);
void PrintPlayerOrder(int* playerOrder);

int main() {

    int cardStack[53] = { 0 };
    int A[14] = { 0 }, B[14] = { 0 }, C[14] = { 0 }, D[14] = { 0 };
    int cardHoldingNum[4] = { 0 };
    int playerOrder[4];

    CreateCardStack(cardStack);
    printf(">>>>> Card Distribution <<<<<\n\n");
    CardDistribute(A, B, C, D, cardStack, cardHoldingNum);
    PrintAllMemberCard(A, B, C, D, cardHoldingNum);

    printf("\n>>>>> Remove Paired Cards <<<<<\n\n");
    RemovePairedCard(A, &cardHoldingNum[0]);
    RemovePairedCard(B, &cardHoldingNum[1]);
    RemovePairedCard(C, &cardHoldingNum[2]);
    RemovePairedCard(D, &cardHoldingNum[3]);

    PrintAllMemberCard(A,B,C,D, cardHoldingNum);

    printf("\n>>>>> Game Start <<<<<\n\n");
    Game(A,B,C,D,cardHoldingNum,playerOrder);

    printf("\n>>>>> Game End <<<<<\n\n");
    PrintPlayerOrder(playerOrder);

    return 0;
}


void PrintCard(int* p, int length) {
    for (int i = 0; i < length; i++)     printf("%d\t", p[i]);
    printf("\n");
}

void PrintAllMemberCard(int* A, int* B, int* C, int* D, int* cardNum) {
    printf("A >>\t");
    PrintCard(A, cardNum[0]);
    printf("B >>\t");
    PrintCard(B, cardNum[1]);
    printf("C >>\t");
    PrintCard(C, cardNum[2]);
    printf("D >>\t");
    PrintCard(D, cardNum[3]);
}

void CreateCardStack(int* cardStack) {
    for (int i = 0; i < 52; i += 13) {
        for (int j = 0; j < 13; j++) {
            cardStack[i + j] = j + 1;
        }
    }
    cardStack[52] = 0;
}
void CardDistribute(int* A, int* B, int* C, int* D, int* cardStack, int* cardNum) {
    srand((unsigned int)time(NULL));
    int flag = 0;
    for (int i = 0; i < 53; i++) {
        int j = rand() % 4;
        if (flag == 1 && cardNum[j] == 13) {
            //after the flag is up and another person gets 13 cards, skip the person's turn
            i--;
            continue;
        }
        if (cardNum[j] == 14) {
            //when someone completes 14 cards, flag up
            i--;
            flag = 1;
            continue;
        }
        if (j == 0) {
            A[cardNum[j]] = cardStack[i];
            cardNum[j]++;
        }
        if (j == 1) {
            B[cardNum[j]] = cardStack[i];
            cardNum[j]++;
        }
        if (j == 2) {
            C[cardNum[j]] = cardStack[i];
            cardNum[j]++;
        }
        if (j == 3) {
            D[cardNum[j]] = cardStack[i];
            cardNum[j]++;
        }
        if (cardNum[j] == 14) {
            flag=1;
        }
    }
}

void ArrayPop(int* arr, int* arrLength, int popIndex) {
    for (int i = popIndex; i < *arrLength - 1; i++) {
        arr[i] = arr[i + 1];        //get rid of the next ones after the one that has to be popped 
    }
    arr[*arrLength-1] = 0;
    --*arrLength;
}

void ArrayPush(int* arr, int* arrLength, int num) {
    arr[*arrLength] = num;
    *arrLength += 1;
}

void RemovePairedCard(int* holdingCard, int* cardNum) {
    int index[14] = { 0 };                  //save the indexes that became pairs
    int n=0;                                //index of the array
    int m[7] = { 14,14,14,14,14,14,14 };    //don't let find other pairs of the found ones
    int y = 0;                              //index of m array
    int flag = 0;                           //when the flag's up, it means that i is in the m array
    for (int i = 0; i < *cardNum - 1; i++) {      
        for (int z = 0; z < 7; z++) {       //don't let find repeated index's other pair
            if (m[z] == i)
                flag = 1;
        }
        if (flag == 1) {                    //when the flag's up, start from the next i 
            flag = 0;
            continue;
        }
        for (int j = i + 1; j < *cardNum; j++) {
            if (holdingCard[i] == holdingCard[j]) {
                index[n] = i;               //save the paired indexes in the index array
                index[n + 1] = j;
                n += 2;
                m[y] = j;
                y++;
                break;
            }
        }
    }
    for (int z = n; z < 14; z++)    index[z] = 0;

    int a,b,tmp=0;
    for (a = 0; a < n -1; a++) {           //arrange the elements in the index in the ascending order
        for (b = 0; b < n - 1 - a; b++) {
            if (index[b] > index[b + 1]) {
                tmp = index[b];
                index[b] = index[b + 1];
                index[b + 1] = tmp;
            }
        }
    }

    int k;           //pop the index array elements in the order
    for (k = 0; k < n; k++) {
        ArrayPop(holdingCard, cardNum, index[k]-k);
    }
}
void Game(int* A, int* B, int* C, int* D, int* cardNum, int* playerOrder) {
    char ABCD[5] = "ABCD";
    int step = 0;                   //mark the current step
    int picked_card = 0;            //the value of the picked card
    int flag_a = 0;                 //when the flag's up, it means that the man doesn't have any more cards
    int flag_b = 0;
    int flag_c = 0;
    int flag_d = 0;
    int k = 0;                      //rank
    int skip;                       //it is used when the next man's card is depleted to skip the turn

    srand((unsigned int)time(NULL));
    int player = rand() % 4;         //choosing the first turn randomly
    
    while (1) {
        skip = 0;
        player = player % 4;        //current step player (0=A, 1=B, 2=C,3=D)
        
        if (cardNum[(player + 1)%4] == 0) {     //the number of extracted cards of next people
            skip++;                             //save how many times have to be skipped
            if (cardNum[(player + 2)%4] == 0)
                skip++;
        }
        //in the index type, select which card the player is going to pick from the next person
        int pick_idx = rand() % cardNum[(player + 1+skip) % 4];     //to assemble in 0, %4

        if (player == 0) {          //A's turn
            if (flag_a == 1) {      //when the player doesn't have any cards, skip
                player++;
                continue;
            }
            printf("\n[ Step %d ]\t", step + 1);
            if (flag_b == 0) {      //select the next player's card
                picked_card = B[pick_idx];
                ArrayPop(B, &cardNum[(player + 1) % 4], pick_idx);      //take the card 
                ArrayPush(A, &cardNum[player], picked_card);            //get the card in your hand
                RemovePairedCard(A, &cardNum[player]);                  //discard the cards which became a pair
                printf("A picked %d from B\n", picked_card);
                if (cardNum[1] == 0) {
                    flag_b = 1;                                         //when the next person's hand is depeleted, flag's up and he is ranked 
                    playerOrder[k] = (player + 1 + skip) % 4;           
                    k++;
                }
            }
            else if (flag_c == 0) {         //when the next person's won the game, pick from the other person's hand
                picked_card = C[pick_idx];
                ArrayPop(C, &cardNum[(player + 2) % 4], pick_idx);
                ArrayPush(A, &cardNum[player], picked_card);
                RemovePairedCard(A, &cardNum[player]);
                printf("A picked %d from C\n", picked_card);  
                if (cardNum[2] == 0) {
                    flag_c = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            else if (flag_d == 0) {         //if two players are left
                picked_card = D[pick_idx];
                ArrayPop(D, &cardNum[(player + 3) % 4], pick_idx);
                ArrayPush(A, &cardNum[player], picked_card);
                RemovePairedCard(A, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player],picked_card,ABCD[(player+1+skip)%4]);
                if (cardNum[3] == 0) {
                    flag_d = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            if (cardNum[player] == 0) {         //if current palyer's hand is depleted
                flag_a = 1;                     //flag's up 
                playerOrder[k] = player;        //rank
                k++;
            }
            
        }
        else if (player == 1) {                 //B's turn, repeat the A's code
            if (flag_b == 1) {
                player++;
                continue;
            }
            printf("\n[ Step %d ]\t", step + 1);
            
            if (flag_c == 0) {
                picked_card = C[pick_idx];
                ArrayPop(C, &cardNum[(player + 1) % 4], pick_idx);
                ArrayPush(B, &cardNum[player], picked_card);
                RemovePairedCard(B, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[2] == 0) {
                    flag_c = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            else if (flag_d == 0) { 
                picked_card = D[pick_idx];
                ArrayPop(D, &cardNum[(player + 2) % 4], pick_idx);
                ArrayPush(B, &cardNum[player], picked_card);
                RemovePairedCard(B, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[3] == 0) {
                    flag_d = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            else if (flag_a == 0) { 
                picked_card = A[pick_idx];
                ArrayPop(A, &cardNum[(player + 3) % 4], pick_idx);
                ArrayPush(B, &cardNum[player], picked_card);
                RemovePairedCard(B, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[0] == 0) {
                    flag_a = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            if (cardNum[player] == 0) {
                flag_b = 1;
                playerOrder[k] = player;
                k++;
            }
        }
        else if (player == 2) {         //C's turn
            if (flag_c==1) {
                player++;
                continue;
            }
            printf("\n[ Step %d ]\t", step + 1);
            
            if (flag_d == 0) {
                picked_card = D[pick_idx];
                ArrayPop(D, &cardNum[(player + 1) % 4], pick_idx);
                ArrayPush(C, &cardNum[player], picked_card);
                RemovePairedCard(C, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[3] == 0) {
                    flag_d = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            else if (flag_a == 0) {
                picked_card = A[pick_idx];
                ArrayPop(A, &cardNum[(player + 2) % 4], pick_idx);
                ArrayPush(C, &cardNum[player], picked_card);
                RemovePairedCard(C, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[0] == 0) {
                    flag_a = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            else if (flag_b == 0) {
                picked_card = B[pick_idx];
                ArrayPop(B, &cardNum[(player + 3) % 4], pick_idx);
                ArrayPush(C, &cardNum[player], picked_card);
                RemovePairedCard(C, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[1] == 0) {
                    flag_b = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            if (cardNum[player] == 0) {
                flag_c = 1;
                playerOrder[k] = player;
                k++;
            }
        }
        else if(player==3 ){            //D's turn
            if (flag_d == 1) {
                player++;
                continue;
            }
            printf("\n[ Step %d ]\t", step + 1);

            if (flag_a==0) {     
                picked_card = A[pick_idx];
                ArrayPop(A, &cardNum[(player + 1) % 4], pick_idx);
                ArrayPush(D, &cardNum[player], picked_card);
                RemovePairedCard(D, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[0] == 0) {
                    flag_a = 1;
                    playerOrder[k] = (player + 1 + skip)%4;
                    k++;
                }
            }
            else if (flag_b==0) {     
                picked_card = B[pick_idx];
                ArrayPop(B, &cardNum[(player + 2) % 4], pick_idx);
                ArrayPush(D, &cardNum[player], picked_card);
                RemovePairedCard(D, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[1] == 0) {
                    flag_b = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            else if (flag_c==0) { 
                picked_card = C[pick_idx];
                ArrayPop(C, &cardNum[(player + 3) % 4], pick_idx);
                ArrayPush(D, &cardNum[player], picked_card);
                RemovePairedCard(D, &cardNum[player]);
                printf("%c picked %d from %c\n", ABCD[player], picked_card, ABCD[(player + 1 + skip) % 4]);
                if (cardNum[2] == 0) {
                    flag_c = 1;
                    playerOrder[k] = (player + 1 + skip) % 4;
                    k++;
                }
            }
            if (cardNum[player] == 0) {
                flag_d = 1;
                playerOrder[k] = player;
                k++;
            }
        }
        PrintAllMemberCard(A, B, C, D, cardNum);        //when the turn's over, show the players' hand
        ++step;
        ++player;                                       //skip to the next player

        if (k >= 3) {                                   //end the game if 3 people's won
            playerOrder[3] = 6 - playerOrder[0] - playerOrder[1] - playerOrder[2];  //sum of the index has to be 6
            break;
        }
    }
}
void PrintPlayerOrder(int* playerOrder) {
    char ABCD[5] = "ABCD";

    printf("1st\t%c Player\n", ABCD[playerOrder[0]]);
    printf("2nd\t%c Player\n", ABCD[playerOrder[1]]);
    printf("3rd\t%c Player\n", ABCD[playerOrder[2]]);
    printf("4th\t%c Player\n", ABCD[playerOrder[3]]);
}
