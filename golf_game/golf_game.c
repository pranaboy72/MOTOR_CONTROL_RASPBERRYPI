
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
#include <time.h>

#define RED 13
#define GREEN 26
#define YELLOW 19
#define BUTTON_1 21
#define BUTTON_2 20

int target;     // target distance
int bright;     // brightness
int b1;         // button 1 status
int b2;         // button 2 status
int d=0;       // driver status- 0: long 1: short
int LEDs;      // number of brightened LEDs
int distance=0; // current distance
int end=0;      // flag for ending the code
int button_push_time=0; // the time in milliseconds when you pushed a button. If its value is 0, no buttons have been pushed yet
int button_pushed_5s;   // the time in milliseconds when you pushed another button

void choose_driver(void)
{
    b2=digitalRead(BUTTON_2);
    button_pushed_5s=millis();  // get the time if you pushed the button before. if not, won't be used
    if ((b2 == HIGH) && ((button_push_time==0)||((button_pushed_5s-button_push_time)>500))){
        // to use this function, you have to push the button and get the digital read
        // also, it must be the first trial you pushed the button or after 0.5seconds after you pushed the last button
        button_push_time= millis(); // get the time you pushed this button
        if (d == 0){    // if the current driver is long driver,
            printf("\033c\n");
			printf("Target Distance: %dm // Your Distance: %dm // Short Driver selected!\n",target,distance);
            d=1;    // change it to the short one.
        }    
        else if (d == 1){   // if the current driver is short driver,
            printf("\033c\n");
            printf("Target Distance: %dm // Your Distance: %dm // Long Driver selected!\n",target,distance);
            d=0;    // change it to the long one.
        }
    }
}
void hit(void)
{
    b1=digitalRead(BUTTON_1);
    button_pushed_5s=millis();  // the mechanism is same as the one in choose_driver function.
    if ((b1 == HIGH) && ((button_push_time==0)||((button_pushed_5s-button_push_time)>500))){
        button_push_time=millis();
        if (d==0){
            distance+= 500*(LEDs+bright/99)/3;  // fully brightened LEDs' brightness is 100.
                                                // if I set a single full brightness as 1, total brightness is (fully brightened LEDs*1 + current brightness/max brightness)
                                                // I get the ratio of total brightness/maximum brightness(=3*100)
                                                // and multiply it with maximum distance of the current driver.
                                                // add the value with the last one.
            printf("\033c\n");
            printf("Target Distance: %dm // Your Distance: %dm // Remaining Distance: %d\n",target, distance, (target-distance));                                                  
        }
        else if (d==1){
            distance+= 50*((LEDs-1)*99+(bright))/(3*99);
            printf("\033c\n");
            printf("Target Distance: %dm // Your Distance: %dm // Remaining Distance: %d\n",target, distance, (target-distance));                                                  
        }
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

    softPwmCreate(RED, 0, 100);
    softPwmCreate(GREEN, 0, 100);
    softPwmCreate(YELLOW, 0, 100);

    srand((unsigned int)time(NULL));
    target= 2000 + rand()%501;  // get the target value

    printf("Game Start!\n");
    printf("You can choose driver\n");
    printf("Long Driver: 0 ~ 500m // Short Driver: 0 ~ 50m\n");
    printf("Target Distance: %dm // Your Distance: %dm",target,distance);

    while(1)
    {
        LEDs=1; // total LEDs that are brightened
		bright=0;   // it will be used for softpwm     
        while(1)
        {
            softPwmWrite(GREEN, bright);
            delay(100);
            choose_driver();    // choose the driver when you pushed the button 2         
            hit();              // get the distance when you pushed the button 1
            if (abs(target - distance)<10){     // if the distance is lower than 10m, you win
                printf("You win!");
                end=1;  // if you win or lose, end the code
                break;
            }
            if ((distance-target)>10){  // if the distance is higher than target more than 10m, you lose
				printf("You lose!");
				end=1;
			}
            bright+=5;      // bigger brightness for the next loop
            if (bright==100) break;     // if the brightness reach to 100, stop the loop
        }
        if (end==1) break;      // if you win or lose, end the whole loop
        
        LEDs=2;
        bright=0;
        while(1)
        {
            softPwmWrite(YELLOW, bright);
            delay(100);
            choose_driver();
            hit();
            if (abs(target - distance)<10){
                printf("You win!");
                end=1;
                break;
            }
            if ((distance-target)>10){
				printf("You lose!");
				end=1;
			}
            bright+=5;
            if (bright==100) break;
        }
        if (end==1) break;

        LEDs=3;
        bright=0;
        while(1)
        {
            softPwmWrite(RED, bright);
            delay(100);
            choose_driver();
            hit();
            if (abs(target - distance)<10){
                printf("You win!");
                end=1;
                break;
            }
            if ((distance-target)>10){
				printf("You lose!");
				end=1;
			}
            bright+=5;
            if (bright==100) break;
        }
        if (end==1) break;

        softPwmWrite(RED, 0);   // after a single loop, you need to adjust all brightness to 0 for the next loop
        softPwmWrite(YELLOW,0);
        softPwmWrite(GREEN, 0);
    }

    return 0;
}
