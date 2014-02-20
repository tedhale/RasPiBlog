#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <pthread.h>

#include <wiringPi.h>


const int NUM_STEPS = 6;		// number of steps including half steps
int currentStep = 1;			// just assume this starting point

// this maps which coils should be energized to position to each step
// using the "half-step" method
// step zero is power off.  The steps then go from 1 to 6
// this is only valid for a 3 coil motor like the one used in the demo
// and must be modified to match the layout of the motor you will be using
int StepMap[7][3] = {
	{0, 0, 0},
	{1, 0, 0},
	{1, 1, 0},
	{0, 1, 0},
	{0, 1, 1},
	{0, 0, 1},
	{1, 0, 1}};
	
// sleep specified number of milliseconds								
int Sleep(int millisecs)
{
	return usleep(1000*millisecs);
}

// moves motor to specified step using the table above
void MoveStepper(int pos)
{
	int i;
	// NOTE: I am just using GPIO 0, 1 and 2 to keep this simple
	for (i=0; i<3; i++)
		digitalWrite(i, StepMap[pos][i]);
}

// step the motor.  1 for clockwise,  -1 for counter-clockwise
void step(int dir)	
{
	currentStep += dir;
	if (currentStep>NUM_STEPS)
		currentStep = 1;
	else if (currentStep<=1)
		currentStep = NUM_STEPS;
	MoveStepper(currentStep);
}

// spin the motor briefly.  Start slow and speed up
void spin()
{
	int x, i;

	i = 0;
	x = 50;
	while (i<2000)
    {
		step(1);
		Sleep(x);
		if (x>=20) x -= x/20;
		else if (x>=10) x -= 1;
		i++;
	}
}

//************************************************************************
int main()
{
	int i, x, c;
	
	// initialize WiringPi library
	x = wiringPiSetup ();
	if (x == -1)
	{
		printf("Error on wiringPiSetup.  Program quitting.\n");
		return 0;
	}
	// set the first three GPIO pins to output and set to 0
	for (i=0; i<3; i++)
	{
		pinMode(i, OUTPUT);
		digitalWrite(i, 0);
	}

	// this ugly hack turns off line buffering on the console so that getchar()
	// returns immediately instead of waiting for the enter key
	system("stty raw");
	
	c = 0;
	while (c!='q')  // loop until q key is pressed
	{
		c = getchar();	// read keypress
		
		switch (c)		// select what action to take
		{
		case 's' :		// step clockwise
			printf("STEP\r\n");
			step(1);
			break;
			
		case 'r' :		// step counter-clockwise
			printf("BACK\r\n");
			step(-1);
			break;
			
		case 'z' :		// do the spinning demo
			printf("SPIN\r\n");
			spin();
			printf("DONE Spinning\r\n");
			break;
		}
	}

	// set power off on the motor
	MoveStepper(0);
	
	// restore line buffering on console
	system("stty cooked");

	printf("STOPPED\n");
	return 1;
}

