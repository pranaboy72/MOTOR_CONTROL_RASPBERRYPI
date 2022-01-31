#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>

#define RED 13
#define GREEN 26
#define YELLOW 19
#define BUTTON_1 21
#define BUTTON_2 20
#define BUTTON_3 16

int b1; // if you pushed the button 1
int b2; // if you pushed the button 2
int b3; // if you pushed the button 3
int answer[5];  // the array to save the answers
int times;  // how many times you pushed the buttons
int err=0;  // flag when you are wrong

void start_1(void)
{
    int a=0;
    while(a<5){     // start with blinking 3times
        digitalWrite(RED, HIGH);
        digitalWrite(GREEN, HIGH);
        digitalWrite(YELLOW, HIGH);
        delay(250);
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, LOW);
        digitalWrite(YELLOW, LOW);
        delay(250);
        a+=1;
    }
}

int main(void)
{
    wiringPiSetupGpio();
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(YELLOW, OUTPUT);

    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);
    pinMode(BUTTON_3, INPUT);
    

    int interval=500;   //starts interval with 500milliseconds

    while(1){           
        start_1();
        if (interval==0){   // if the interval becomes 0, code ends with green light
            digitalWrite(GREEN,HIGH);
            break;
        }

        srand((unsigned int)time(NULL));
        for(int a=0;a<5;a++)
        {
            int r= rand()%3;    // get which LED to be blinked in this loop
            answer[a]=r;    // store the blinked LED number

            if (r==0){	// r=0: green, r=1: yellow, r=2: red LED
                digitalWrite(GREEN, HIGH);
                delay(interval);    // the 'interval' changes every loop.
                digitalWrite(GREEN, LOW);
                delay(interval);
            }
            else if (r==1){
                digitalWrite(YELLOW, HIGH);
                delay(interval);
                digitalWrite(YELLOW, LOW);
                delay(interval);
            }
            else if (r==2){ 
                digitalWrite(RED, HIGH);
                delay(interval);
                digitalWrite(RED, LOW);
                delay(interval);
            }
        }
        //for (int k=0; k<5; k++)	printf("%d\n",answer[k]);

        times =0; // number of times of pushing buttons
        while(1){
			b1=digitalRead(BUTTON_1);   // if you pushed any one of those buttons, get the digital read
			b2=digitalRead(BUTTON_2);
			b3=digitalRead(BUTTON_3);			
			delay(200);     // need delay otherwise code gets more than one signal while pushing the button once
                            // therefore you need to push the buttons with quite wide intervals
			if (b1==HIGH){
				//printf("BUTTON: 1\n");
				if (answer[times]!=0){
					err=1;  // if you pushed the wrong one, flag err and end the code
					break;
				}
				times+=1;				
			}
			else if (b2==HIGH){
				//printf("BUTTON: 2\n");
				if (answer[times]!=1){
					err=1;
					break;
				}
				times+=1;
			}
			else if (b3==HIGH){
				//printf("BUTTON: 3\n");
				if (answer[times]!=2){
					err=1;
					break;
				}
				times+=1;
			}	
			if (times == 5)	break; // if you pushed buttons 5times, end the loop
		}
		if (err == 1)   // if you have pushed the wrong button,
		{
			digitalWrite(RED, HIGH);    // end the code with red light
			break;
		}
        interval-=50;   // shorten the interval with 0.1 second
                        // (LED HIGH for 0.05s, and LOW for 0.05s for 0.1s interval, so I reduced the value only 50)
	}   
    return 0;
}
