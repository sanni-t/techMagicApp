/** Sketch that listens to spells on SimbleeCOM and Opens/Closes the blinds accordingly 		**/
/** This sketch includes an Over The Air Update bootloader to update the code using BLE 		**/
/** Simblee switches to BLE when it receives 													**/
/** 'BlindsControllerUpdate' message from the central device.  									**/
/** For more on OTA updates, see: https://www.simblee.com/Simblee%20OTA%20Reference%20V1.0.pdf 	**/

#include "SimbleeCOM.h"
#include "OTA_Bootloader.h"

#define MOTOR1_CW_PIN 	5
#define MOTOR1_CCW_PIN	3
#define MOTOR2_CW_PIN 	15
#define MOTOR2_CCW_PIN	12
#define MOTOR3_CW_PIN 	11
#define MOTOR3_CCW_PIN	9
#define LED 			13
#define MOTOR_SPEED		250

typedef enum{
	CW,
	CCW
}motor_direction;

struct motor_entity{
	int num;
	int cw_pin;
	int ccw_pin;
};

motor_entity motors[] = {{1, MOTOR1_CW_PIN, MOTOR1_CCW_PIN},
						 {2, MOTOR2_CW_PIN, MOTOR2_CCW_PIN},
						 {3, MOTOR3_CW_PIN, MOTOR3_CCW_PIN}}; 
bool blindsNewState = 0;
bool blindsCurrentState = blindsNewState;
bool switchToBLE = false;

void setup() {
	SimbleeCOM.begin(); 
	//Serial.begin(9600);
	//pinMode(LED, OUTPUT);
	//Serial.println("Restarted");

	analogWrite(MOTOR1_CW_PIN, 0);
	analogWrite(MOTOR2_CW_PIN, 0);
	analogWrite(MOTOR3_CW_PIN, 0);
	analogWrite(MOTOR1_CCW_PIN, 0);
	analogWrite(MOTOR2_CCW_PIN, 0);
	analogWrite(MOTOR3_CCW_PIN, 0);

	//Initialize all blinds to a known state
	flipAllBlinds(0);
}

void loop() {
	analogWrite(LED, 100);
	delay(250);
	analogWrite(LED, 0);
	delay(250);

	if(blindsCurrentState != blindsNewState)
	{
		flipAllBlinds(blindsNewState);
		blindsCurrentState = blindsNewState;
	} 
	//delay(1000);
	// flipAllBlinds(0);
}

void flipAllBlinds(bool newState)
{
	if(newState == 0)
	{
		moveMotor(1, CW, 1350);
		// analogWrite(MOTOR1_CW_PIN, MOTOR_SPEED);
		// delay(1350);
		// analogWrite(MOTOR1_CW_PIN, 0);

		moveMotor(2, CW, 1500);
		// analogWrite(MOTOR2_CW_PIN, MOTOR_SPEED);
		// delay(1500);
		// analogWrite(MOTOR2_CW_PIN, 0);

		moveMotor(3, CW, 1200);
		// analogWrite(MOTOR3_CW_PIN, MOTOR_SPEED);
		// delay(1200);
		// analogWrite(MOTOR3_CW_PIN, 0);
	}
	else
	{
		moveMotor(1, CCW, 1300);
		// analogWrite(MOTOR1_CCW_PIN, MOTOR_SPEED);
		// delay(1300);
		// analogWrite(MOTOR1_CCW_PIN, 0);

		moveMotor(2, CCW, 1500);
		// analogWrite(MOTOR2_CCW_PIN, MOTOR_SPEED);
		// delay(1500);
		// analogWrite(MOTOR2_CCW_PIN, 0);
		
		moveMotor(3, CCW, 1500);
		// analogWrite(MOTOR3_CCW_PIN, MOTOR_SPEED);
		// delay(1500);
		// analogWrite(MOTOR3_CCW_PIN, 0);
	}
}

void moveMotor(int num, motor_direction dir, unsigned long delayms)
{
	if(dir == CW)
	{
		analogWrite(motors[num-1].cw_pin, MOTOR_SPEED);
		delay(delayms);
		analogWrite(motors[num-1].cw_pin, 0);
	}
	else
	{
		analogWrite(motors[num-1].ccw_pin, MOTOR_SPEED);
		delay(delayms);
		analogWrite(motors[num-1].ccw_pin, 0);
	}
}

void SimbleeCOM_onReceive(unsigned int esn, const char* payload, int len, int rssi)
{
  String input = "";
  for (int i = 0; i < len; i++)
  {
    input += payload[i];
  }
  Serial.println(input);
  if(input == "FLIP_BLINDS")
  {
    blindsNewState = !blindsCurrentState;
  }
  else if(input == "UpdateBoard")
  {
  	ota_bootloader_start(); //begins OTA enabled state
  }
  else if(input == "1")
  {
  	moveMotor(1, CW, 1000);
  }
  else if(input == "2")
  {
  	moveMotor(2, CW, 1000);
  }
  else if(input == "3")
  {
  	moveMotor(3, CW, 1000);
  }
  else if(input == "!")
  {
  	moveMotor(1, CCW, 1000);
  }
  else if(input == "@")
  {
  	moveMotor(2, CCW, 1000);
  }
  else if(input == "#")
  {
  	moveMotor(3, CCW, 1000);
  }
}

