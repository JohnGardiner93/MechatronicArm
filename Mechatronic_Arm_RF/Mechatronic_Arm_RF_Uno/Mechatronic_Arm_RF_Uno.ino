//Version 1.1
//Date: 11/23/2014
//
// Description: For controlling mechatronic arm with joysticks. It features 6 servos controlled by PWM.
// Uses 2 Sainsmart Joysticks. Joysticks have 2 trimpots each and one button each. Each trimpot is used for
// one servo. The two buttons are used for one servo. Rotation of the base is handled by a fifth trimpot.
// This portion of the code handles the movement of the servos. The Mega portion of the code handles the 
// joystick input and the led feedback. This version uses pure I2C. However, wireless communication wlll
// not be possible using I2C and wireless serial communication will be needed in the future.
//
// Desired Future Updates: Optimization. Control Mode switch. Wireless serial communication. Comm mode switch.
//
// Equipment: Arduino Uno, 2 4.7k ohm resisitors (for pull-up on GND and +5V lines)

///////////////////////////////////////////////////////////////////////////////
//Libraries
#include <Servo.h>  //Servo library
#include <SPI.h>
#include <EEPROM.h>
//#include <ServoTimer2.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// Servos array in order from bottom of the arm up
// shoulderRotate : Servo 1 - motor[0]
// shoulderBend   : Servo 2 - motor[1]
// elbow          : Servo 3 - motor[2]
// wristBend      : Servo 4 - motor[3]
// wristRotate    : Servo 5 - motor[4]
// gripper        : Servo 6 - motor[5]

// Instantiate arrays (servo and position)
Servo motor[6];
long rd[6];
int p = 0;
long rec = 0;
//ServoTimer2 fmotor;

RF24 radio(2,10);

const uint64_t pipe = 0xE8E8F0F0E1LL;

typedef struct{
  long shoulderRotate;
  long shoulderBend;
  long elbow;
  long wristBend;
  long wristRotate;
  long grippr;
}
A_t;
A_t positions;
///////////////////////////////////////////////////////////////////////////////

void setup()
{
  
  Serial.begin(9600);
  
  radio.begin();
  radio.setDataRate(RF24_2MBPS); // Both endpoints must have this set the same
  radio.setAutoAck(false);
  //radio.printDetails();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  
  //Connect servos to pwm pins
  //motor[0].attach(13);
  //fmotor.attach(4);
  //motor[1].attach(11);
  motor[2].attach(9);
  motor[3].attach(6);
  motor[4].attach(5);
  motor[5].attach(3);
  
  rd[0] = 90;
  rd[1] = 90;
  rd[2] = 100;
  rd[3] = 40;
  rd[4] = 90;
  rd[5] = 45;
 //initiate array at resting position in case comm does
                           //not start right away 
                         
}

///////////////////////////////////////////////////////////////////////////////

void loop()
{
  
//Loops through each servo reading (except the gripper which is handled digitally)

Serial.println("hfhfhf");
  if ( radio.available() ) {
    radio.read(&positions, sizeof(positions));
    Serial.println("got it mang");
    Serial.println(positions.elbow);
    //motor[0].write(positions.shoulderRotate);
    //motor[0].write(0);
    //fmotor.write(positions.shoulderRotate);
    //motor[1].write(positions.shoulderBend);
    motor[2].write(positions.elbow);
    motor[3].write(positions.wristBend);
    motor[4].write(positions.wristRotate);
    motor[5].write(positions.grippr);
    
  }

}

///////////////////////////////////////////////////////////////////////////////
