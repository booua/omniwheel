#include "hFramework.h"
#include <cmath>
#include <stdio.h>
#include <stddef.h>
#include <DistanceSensor.h>
#include <math.h>


using namespace hModules;

bool obstacleFront = false;
bool obstacleBack = false;
bool obstacleRight = false;
bool obstacleLeft = false;

enum Direction
{
	left = 0,
	right = 1,
	forward = 2,
	backward = 3,
	fRight = 4,
	fLeft = 5,
	rRight = 6,
	rLeft = 7
};

void relativeMove(int positionX, int positionY)
{
	if (positionX > 0 && obstacleRight)
	{
			positionX = 0;
	}
	if (positionX < 0 && obstacleLeft)
	{
			positionX = 0;
	}
	if (positionY > 0 && obstacleFront)
	{
			positionY = 0;
	}
	if (positionY < 0 && obstacleBack)
	{
			positionY = 0;
	}

	hMot1.setPower(positionY);
	hMot2.setPower(-positionY);

	hMot3.setPower(positionX);
	hMot4.setPower(-positionX);
}

int readPosition(char received_data)
{
	char xposition[4];
	char yposition[4];
	int j = 0;
	int k = 0;
	int l = 0;
	int x = 0;
	int y = 0;
	int z = 0;

	// for(int i=0; received_data[i] == 'n'; i++)
	// {
		Serial.printf("received: %s\r\n", received_data);
	// }
	for(int i=0; received_data[i] == 'n'; i++)
	{
		if(received_data[i] == 'X')
		{
			j = i + 1;
			k = 0;
			while(received_data[j] != 'Y')
			{
				xposition[k] = received_data[j];
				k++;
				j++;
			}
		}
		if(received_data[i] == 'Y')
		{
			j = i + 1;
			l = 0;
			while(received_data[j] != 'n')
			{
				yposition[l] = received_data[j];
				l++;
				j++;
			}
		}
	}
	if(xposition[0] == '-')
	{
		z = 0;
		while(k > 0)
		{
			x += (int)xposition[k] * pow(10,z);
			k--;
			z++;
		}
	}else
	{
		z = 0;
		while(k == 0)
		{
			x += (int)xposition[k] *  pow(10,z);
			k--;
			z++;
		}
	}

	if(yposition[0] == '-')
	{
		z = 0;
		while(l > 0)
		{
			y += (int)yposition[l] *  pow(10,z);
			l--;
			z++;
		}
	}else
	{
		z = 0;
		while(l == 0)
		{
			y += (int)yposition[l] *  pow(10,z);
			l--;
			z++;
		}
	}
	Serial.printf("x =  %d\n", x);
	Serial.printf("y =  %d\n", y);

 return 1;
}
void getDistanceAndDetectObstacleTask()
{
	DistanceSensor sensorFront(hSens1.getBaseSens());
	DistanceSensor sensorBack(hSens5.getBaseSens());
	DistanceSensor sensorLeft(hSens4.getBaseSens());
	DistanceSensor sensorRight(hSens2.getBaseSens());
	
	while (1)
	{
		hLED1.off();
		int frontDist = sensorFront.getDistance();
		int backDist = sensorBack.getDistance();
		int leftDist = sensorLeft.getDistance();
		int rightDist = sensorRight.getDistance();
		

		// Serial.printf("%d\n",frontDist);
		// Serial.printf("%d\n",rightDist);
		// Serial.printf("%d\n",backDist);
		// Serial.printf("%d\n",leftDist);
		// hExt1.serial.printf("%d %d %d %d\n",frontDist,rightDist,backDist,leftDist);


		if (leftDist <= 10 && leftDist >= 0)
		{
			
			Serial.printf("OBSTACLE DETECTED: LEFT!\r\n");
			obstacleLeft = true;
			hLED1.on();
		}
		if (rightDist <= 10 && rightDist >= 0)
		{
			Serial.printf("OBSTACLE DETECTED: RIGHT!\r\n");
			obstacleRight = true;
			hLED1.on();
		}
		if (frontDist <= 10 && frontDist >= 0)
		{
			Serial.printf("OBSTACLE DETECTED: FRONT!\r\n");
			obstacleFront = true;
			hLED1.on();
		}
		if (backDist <= 10 && backDist >= 0)
		{
			Serial.printf("OBSTACLE DETECTED: BACK!\r\n");
			obstacleBack = true;
			hLED1.on();
		}
			obstacleLeft = false;
			obstacleBack = false;
			obstacleFront = false;
			obstacleRight = false;
	}
}

void switchElectromagnet(){
	printf("electromagnet toggled!\r\n");
}

int bluetoothReceiveCommandTask()
{
	char received_data[];
	while(1){
		if (hExt1.serial.available() > 0)
		{
			if (hExt1.serial.read(received_data, sizeof(received_data), 500))
			{
				
				// printf("received data: %s\r\n", received_data);
				readPosition(received_data);
				switch (received_data[0])
				{
				case 'm':
					switchElectromagnet();
					break;
				return 1;
				}
			}
			else
			{
				printf("no data received - check connections!\r\n");
				return 0;
			}
		}
	}
}

void hMain()
{
	hExt1.serial.init(9600, Parity::None, StopBits::One);
	sys.taskCreate(&bluetoothReceiveCommandTask);
	sys.taskCreate(&getDistanceAndDetectObstacleTask);
	
}