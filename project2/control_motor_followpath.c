#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <math.h>


#define LOOPTIME 0.01 // in ms. The reference position changes a lot more than the first project,
                      // so I thought the loop time has to be much smaller than that of first project. 
				   
#define ENCODER_A 20
#define ENCODER_B 21
#define ENC2REDGEAR 217 // 216 is just a theoretical value. Empirically, 217 had more precise values
#define PULSE 17

#define MOTOR1 23
#define MOTOR2 24

#define PGAIN 500   // I used PID control to control the motor
#define IGAIN 0
#define DGAIN 10


int enc_A;
int enc_B;
int pulse;
int encoderPosition = 0;
float redGearPosition = 0;

float referencePosition[10000]={};  // array for the reference positions every 0.01 ms. so the array length would be 9999
float playbackPosition[10000]={};   // array for the playback positions. It will be used to get the rms
int reference_idx=0;    // it is index for the reference position array
int playback_idx=0;     // it is index for the playback position array

unsigned int startTime;     // it is the time when you start playback
unsigned int checkTime;     // checking time for every loop
unsigned int checkTimeBefore;   // checked time before

unsigned int teach_starttime;   // the three parameters are same as those of above. 
unsigned int teach_timebefore;  // the only difference is that these are used while teaching
unsigned int teach_time;

int playback_flag=0;    //
float e;
float e_rms=0.0;

float max_func=0;
int max_n=0;

int crush = 0;
float max_e=0;

int flag =1;
int T_0;


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
}

void teach()
{
    teach_starttime=millis();
    teach_timebefore=millis();
    while(1)
    {
        teach_time=millis();
        if (teach_time - teach_starttime > 10000)   break;
        if (teach_time - teach_timebefore >LOOPTIME)
        {
            referencePosition[reference_idx]=redGearPosition;
            if ((teach_time-teach_starttime)%50 ==0){
                printf("reference Pos(%d): %f\n",reference_idx,referencePosition[reference_idx]);
            }
            //printf("reference Pos(%d): %f\n",reference_idx,referencePosition[reference_idx]);
            reference_idx+=1;
            teach_timebefore=teach_time;
        }
    }
    
}

void playback()
{
    startTime=millis();
    checkTimeBefore=millis();

    float m_1=0;		// last m
    float m;  			// present m
    float e_1=0;		// last time error
    float e_2=0;		// error before e_1
    float G1, G2, G3;	// constant values to get m

    G1 = PGAIN+IGAIN*(LOOPTIME)+DGAIN/(LOOPTIME);
	G2 = -(PGAIN+2*DGAIN/(LOOPTIME));
	G3 = DGAIN/(LOOPTIME);

    while(1)
    {
        checkTime = millis();

        if (checkTime - startTime >= 10000){	// 10 seconds later after the start, stop the loop
            break;
		}
		
		
        if (checkTime - checkTimeBefore > LOOPTIME) // in every loop after LOOPTIME,
        {   
			playbackPosition[playback_idx]=redGearPosition;
			e = referencePosition[playback_idx] - playbackPosition[playback_idx];
			//printf("error(%d): %.4f\n",playback_idx,e);
			playback_idx+=1;
			m = m_1 + G1 * e + G2 * e_1 + G3 * e_2;		// renew m in every loop
            if ((checkTime-startTime)%50 == 0){
                printf("current pos(%d): %f\n",playback_idx-1,playbackPosition[playback_idx-1]);
            }
            if (max_e<fabs(e))	max_e=fabs(e);
            if (fabs(e) > 0.1)
            {
                crush=1;
                break;
            }
			
            if (e > 0)	// if the magnitude of current position is lower than that of the objection,
            {
                softPwmWrite(MOTOR1, m);
                softPwmWrite(MOTOR2, 0);
            }
            else if (e < 0)	//otherwise,
            {
                softPwmWrite(MOTOR2, -m); 
                softPwmWrite(MOTOR1, 0);
            }
            checkTimeBefore = checkTime;	// renew every values for the next loop
            m_1 = m;						// except the constants
            e_1 = e;
            e_2 = e_1;
		}
	}
}

void cross_correlation()
{   
    int n=0;
    while(n<playback_idx){
        float correlation_func=0;
        int i=0;
        while (i+n<playback_idx){
            correlation_func+=referencePosition[i] * playbackPosition[i+n];
            i++;
        }
        if (n==0)   max_func=correlation_func;
        if (max_func < correlation_func){
            max_func = correlation_func;
            max_n = n;
        }
        n++;
    }
}

int main(void)
{
    printf("Waiting for the first pulse...\n");
    
    wiringPiSetupGpio();

    pinMode(ENCODER_A, INPUT);
    pinMode(ENCODER_B, INPUT);
    pinMode(PULSE, INPUT);

    softPwmCreate(MOTOR1, 0, 100);
    softPwmCreate(MOTOR2, 0, 100);
    wiringPiISR(ENCODER_A, INT_EDGE_BOTH, funcEncoderA);
    wiringPiISR(ENCODER_B, INT_EDGE_BOTH, funcEncoderB);

    encoderPosition=0;
    redGearPosition=0;
    printf("First pulse detected!\n");
    printf("Start teaching for 10 seconds...\n");
    teach();
        
    printf("Waiting for the second pulse...\n");
    delay(2000);    // it is for term to start playback

    encoderPosition=0;
    redGearPosition=0;
    printf("Second pulse detected!\n");
    printf("Start playback...\n");
    playback();
    if (crush == 1) printf("Crush Detected!! Stop the motor\n");
    else {
        cross_correlation();
        printf("max n: %d\n",max_n);

        int i=0;
        while(flag){
            if (referencePosition[i] != 0){
                T_0 = i;
                flag=0;
            }  
            i++;
        }

        i=T_0;
        while (i<playback_idx-max_n){
            e_rms+=pow(fabs(referencePosition[i]-playbackPosition[i+max_n]),2);
            i++;
        }
        e_rms=sqrt(e_rms/(playback_idx-1-max_n));
        printf("reference index: %d\n",reference_idx);
        printf("playback index: %d\n",playback_idx);
        printf("e_rms: %.4f\n",e_rms);
        printf("max e: %f",max_e);
    }
    
    return 0;
}
