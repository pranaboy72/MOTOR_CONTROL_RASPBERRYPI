#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <wiringPi.h>

#define RED 13      //red led
#define YELLOW 19   //yellow led
#define GREEN 26    //green led
#define BUTTON1 21  //left step button
#define BUTTON2 20  //right step button

struct winsize size;

void gamedesc(void);

int main(int argc, char **argv)
{
    //WiringPi setup
    wiringPiSetupGpio();
    pinMode(RED, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);

    //Variable define
    unsigned int gametime;
    int leftstep, gameresult;
    float lefttime;     // it is for the time left
    int currentstep;    // it tells if you pressed the button and used for counting step left
    int round=1, round_interval;   // round: when green light is on, value is 1, roundtime: interval time for this round 
    int state;  // tells which foot you are on

    gametime = 120000; // Game Time (milseconds) : default 2mins
    gameresult =1; // 1: Game Pass or 0: Game over
    leftstep = 100; // Left Number of steps 2 touch down: default 100 steps

    int state0, state1; // state0: left foot, state1: right foot
    int fall=0; // tells if you are fall down or not 1: fall, 2: not fall
    float falltime, standuptime;
    int fallcount=0;  // used to check the time when fell
    int redlight=0;   // becomes 1 when the redlight is on
    int beforetime;   // get the time of the last loop



    //Game discription
    ioctl(0, TIOCGWINSZ, &size);
    srand((unsigned int)time(NULL));

    gamedesc();

    //Initialize

    unsigned int startime = millis();
    unsigned int endtime = gametime+startime;
    
    unsigned int currenttime=millis();   // get current time
    unsigned int roundstart = millis();  // the time every round starts
    round_interval= ((rand()%9)+1)*1000; // get interval time for this round in milliseconds
    unsigned int roundend= roundstart+round_interval;   // get the time when the round ends

    // Loop start
    while(1)
    {   
        if (round == 1) // When green Light is on
        {
            // Turn on green Light
            digitalWrite(GREEN, HIGH);
            digitalWrite(RED, LOW);

            if (fall == 0) // if you are not fall down
            {
                digitalWrite(YELLOW, LOW);
                state0=digitalRead(BUTTON1);
                //if (state0 == HIGH)  printf("left\n");
                delay(100); // had to put this line because the code recoginzed the button pushed twice though it was once
                state1=digitalRead(BUTTON2);
                //if (state1 == HIGH)  printf("right\n");
                delay(100);
                if (state == 0 && state0 == HIGH){
                    fall =1;    // if last: left, current: left
                    fallcount=1;
                }
                if (state == 1 && state1 == HIGH){
                    fall =1;    // if last: right, current: right
                    fallcount=1;
                }
                if (state0 == HIGH && state1 == HIGH){
                    fall =1; // if stepped both
                    fallcount=1;
                }
                if (state0 == HIGH && state1 == LOW){   // if current : left step
                    state=0;
                    currentstep+=1;
                }
                else if (state0 == LOW && state1 ==HIGH){   // if current : right step
                    state =1;
                    currentstep+=1;
                }
            }
            else // if you fall down
            {
                digitalWrite(YELLOW, HIGH);
            }
        }

        else // when red light is on
        {
            // Turn on red light
            digitalWrite(GREEN, LOW);
            digitalWrite(RED, HIGH);
            state0= digitalRead(BUTTON1);
            state1= digitalRead(BUTTON2);
            
            if (state0 == HIGH || state1 == HIGH)
            {
                gameresult = 0;
                break;
            }
        }

        //Before loop again

        // Fall Check
        if (fall == 1 && fallcount==1)  // At the moment you fell down,
        {
            digitalWrite(YELLOW, HIGH);
            falltime=millis();  // get the time when you fell down
            standuptime=falltime+1000;  // get the time when you are gonna get up
            fallcount=0;    // if 'fallcount' isn't 0, it will get another fall down time.
                            // Only need the time at the moment you fell down.
        }
        
        // Status initialize
        printf("\033c");    // Reset terminal screen


        //Status print
        leftstep =100-currentstep;  // get the left step with the accumulated current step
        currenttime=millis();   // get the current time every loop
        lefttime = endtime - currenttime;   // get the left time

        if (fall == 1 && round == 0)
        {
            standuptime+=(currenttime-falltime); 
        }

        if ((currenttime >= roundend) && (round ==1)){  // if the round ends, start the red light
            round=0;  
            redlight=millis();  // get the time when redlight's on
        }

        if ((currenttime >= redlight+2000) && (round == 0)){    // if it is after 2 seconds when red light's on
            roundstart = millis();
            round_interval= ((rand()%9)+1)*1000;
            roundend= roundstart+round_interval;
            round=1; // new round starts after 2 seconds
        }

        if ((currenttime >= standuptime) && (fall == 1)){   // if it is after a second when you fell down
            digitalWrite(YELLOW, LOW);
            fall=0;   // stand up after a second
            state=2;    // initialize state 
        }

        for (int i=0; i<size.ws_col; i++)   putchar('-');
        printf("\n");
        printf("leftstep : %d steps, lefttime : %f seconds\n",leftstep,lefttime/1000.0);

        if (fall == 1) // if you are fall down, show left time until you stand up
        {
            printf("You fell down!! - wait %f seconds\n", (standuptime-currenttime)/1000.0);
        }

        //Loop escape check
        if (leftstep==0) break; // Left step check
        if (currenttime == endtime) gameresult = 0; // Left time check
        if (gameresult == 0) break; // Game result check
    }

    //Loop end
    printf("\n");
    for (int i=0;i<size.ws_col;i++) putchar('-');
    printf("\n");

    digitalWrite(GREEN, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);

    if (gameresult == 1) //Game win
    {
        printf("\n");
        printf("\n");
        printf("GAME END - YOU SURVIVED\n");
        printf("\n");
        for (int i=0; i < size.ws_col; i++) putchar('-');
        printf("\n");

        for (int i=0; i<7; i++)
        {
            digitalWrite(GREEN, HIGH);
            delay(100);
            digitalWrite(GREEN, LOW);
            digitalWrite(YELLOW, HIGH);
            delay(100);
            digitalWrite(YELLOW, LOW);
            digitalWrite(RED, HIGH);
            delay(100);
            digitalWrite(RED, LOW);
        }
    }
    else // game over
    {
        printf("\n");
        printf("\n");
        printf("GAME OVER - YOU'RE DEAD\n");
        printf("\n");
        for (int i=0; i< size.ws_col; i++) putchar('-');
        printf("\n");

        for (int i=0; i<5; i++)
        {
            digitalWrite(GREEN, HIGH);
            digitalWrite(YELLOW, HIGH);
            digitalWrite(RED, HIGH);
            delay(500);
            digitalWrite(GREEN, LOW);
            digitalWrite(YELLOW, LOW);
            digitalWrite(RED, LOW);
            delay(500);
        }
    }

    digitalWrite(GREEN, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, LOW);

    return 0;
}

void gamedesc(void)
{
    for (int i=0; i<size.ws_col; i++)   putchar('-');
    printf("Red light & Green light Game\n");
    delay(1000);
    printf("Rule 1 : When Green Light is on, you will move towards the finish line.\n");
    delay(1000);
    printf("Rule 2: When Red light is on, you must immediately stop. If not game is over.\n");
    delay(1000);
    for (int i=0; i<size.ws_col; i++) putchar('-');
    printf("Game is start in 3 seconds\n");
    delay(1000);
    printf("3\n");
    delay(1000);
    printf("2\n");
    delay(1000);
    printf("1\n");
    delay(1000);
    printf("GAME START\n");
    for (int i=0; i<size.ws_col; i++) putchar('-');
}
