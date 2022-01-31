#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <math.h>

#define LOOPTIME 0.01 // in sec
				   
#define ENCODER_A 20
#define ENCODER_B 21
#define ENC2REDGEAR 216
#define PULSE 17

#define MOTOR1 23
#define MOTOR2 24

#define PGAIN 800
#define IGAIN 0.9
#define DGAIN 1

int enc_A;
int enc_B;
int pulse;
int encoderPosition = 0;
float redGearPosition = 0;

float referencePosition;
float e= 0;		// means errorposition
float ITAE =0;	// for ITAE

unsigned int startTime;
unsigned int checkTime;
unsigned int checkTimeBefore;

void funcEncoderA()
{
	enc_A=digitalRead(ENCODER_A);
	enc_B=digitalRead(ENCODER_B);
	
	if (enc_A == HIGH) // If A is rising
	{
		if (enc_B == LOW) 	//WHen B is LOW
			encoderPosition++;
		else                //When B is High
			encoderPosition--;
	}
	else // If A is falling
	{
		if (enc_B==LOW)
			encoderPosition--;
		else
			encoderPosition++;
	}
	redGearPosition = (float)encoderPosition / ENC2REDGEAR;
	e = referencePosition - redGearPosition;
}
			
void funcEncoderB()
{
	enc_A=digitalRead(ENCODER_A);
	enc_B=digitalRead(ENCODER_B);
	
	if (enc_B == HIGH) // If A is rising
	{
		if (enc_A == LOW) 	//When A is LOW
			encoderPosition--;
		else                //When A is High
			encoderPosition++;
	}
	else // If B is falling
	{
		if (enc_A==LOW)
			encoderPosition++;
		else
			encoderPosition--;
	}
	redGearPosition = (float)encoderPosition / ENC2REDGEAR;
	e = referencePosition - redGearPosition;
}
void PID_CONTROL()
{
	startTime=millis();
	checkTimeBefore = millis();
	e = referencePosition - redGearPosition;
	
    float m_1=0;		// last m
    float m;  			// present m
    float e_1=0;		// last time error
    float e_2=0;		// error before e_1
    float G1, G2, G3;	// constant values to get m
    
    G1 = PGAIN+IGAIN*LOOPTIME+DGAIN/LOOPTIME;
	G2 = -(PGAIN+2*DGAIN/LOOPTIME);
	G3 = DGAIN/LOOPTIME;
    
    
    while(1)
    {
        checkTime = millis();
        if (checkTime - startTime >= 5000){	// 5 seconds later after the start, stop the loop
            break;
		}
        m = m_1 + G1 * e + G2 * e_1 + G3 * e_2;		// renew m in every loop
        if (checkTime - checkTimeBefore > LOOPTIME) // in every loop after LOOPTIME,
        {
			if (checkTime-startTime)%50 ==0)	// every 50ms, get the position 
												// to get all the data 
												// couldn't get all of it by printing all positions
			{
				printf("%f\n",redGearPosition);
			}
            if (e > 0)	// if the magnitude of current position is lower than that of the objection,
            {
				//printf("Current Motor Position: %f\n", redGearPosition);
                //printf("m: %f\n",m); // to check magnitude of m
                softPwmWrite(MOTOR1, m);
                softPwmWrite(MOTOR2, 0);
            }
            else 	//otherwise,
            {
				//printf("Current Motor Position: %f\n", redGearPosition);
                //printf("m: %f\n",m); //to check magnitude of m
                softPwmWrite(MOTOR2, -m); 
                softPwmWrite(MOTOR1, 0);
            }
            checkTimeBefore = checkTime;	// renew every values for the next loop
            m_1 = m;						// except the constants
            e_1 = e;
            e_2 = e_1;
            ITAE = ITAE + LOOPTIME * (checkTime-startTime)/1000.0 * fabs(e);
			// Get ITAE with numerical integration ( Sigma t*abs(error)*(t-t1))
			// Set time in seconds
		}
	}
}

int main(void)
{
    int CW,CCW;
    printf("clockwise: ");			// get the target values of 
    scanf("%d",&CW);				// clockwise and counter-clockwise
    printf("counter-clockwise: ");
    scanf("%d",&CCW);
    CCW=-CCW; 						// if the starting position is 0, counter-clockwise becomes negative
									// so I set the CCW in negative
    wiringPiSetupGpio();

    pinMode(ENCODER_A, INPUT);
    pinMode(ENCODER_B, INPUT);
    pinMode(PULSE, INPUT);

    softPwmCreate(MOTOR1, 0, 100); // 3rd parameter of softPwmCreate assign pulse length
    softPwmCreate(MOTOR2, 0, 100); // if I set 3rd parameter more than 100, the motor
								   // rotates discontinuously, and if I set it too low, 
								   // the range of applicable gain values is reduced 
								   // since the motor gets highest value of 3rd parameter.
								   // Thus I thought the most appropriate value is 100

    wiringPiISR(ENCODER_A, INT_EDGE_BOTH, funcEncoderA);
    wiringPiISR(ENCODER_B, INT_EDGE_BOTH, funcEncoderB);
    
	//CW	
	referencePosition = CW;
	//pulse = HIGH; 		// For my private experimentation, I declared pulse value HIGH to rotate automatically
						// So the lowest ITAE value and combination of Gains might be different
	while(1)
	{
		pulse = digitalRead(PULSE);	// if the pulse comes in to 17pin,
										// read the pulse and begin pid control
										// erased this sentence for my private experimentation
		if (pulse == HIGH)
		{
			PID_CONTROL();
			//pulse = LOW;			// declare the pulse value LOW 
									// if not, the next loop will start automatically
			
			break;					// after the control ends, get out of the loop
		}
	}
	//CCW		
	referencePosition = CCW;
	//pulse = HIGH; 		// Same reason as above
	while(1)
	{
		pulse = digitalRead(PULSE);	// activate same as last loop
		if (pulse == HIGH)
		{
			PID_CONTROL();
			//pulse = LOW;
			break;
		}
	}
    
    printf("\nITAE: %.4f", ITAE);
    return 0; 
}
