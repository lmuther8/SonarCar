//code to make car drive controlled by IR remote

#include <Servo.h>
Servo servo;                 // create object to refer to your servo
int servoPin = 3;            // create variable to store servo signal pin

#include <NewPing.h>
int triggerPin = 4;          // Arduino pin tied to trigger pin on the ultrasonic sensor.
int echoPin = 5;             // Arduino pin tied to echo pin on the ultrasonic sensor.
int maxDistance = 200;       // Maximum distance we want to ping for (in centimeters).
NewPing sonar(triggerPin, echoPin, maxDistance); // NewPing setup of pins and maximum distance.

//Motor Vars
//declare pin nubers for Left motor
int speedL = 11;  //enable L
int goL = 9;      //ctrl 1 L
int backL = 8;    //ctrl 2 L
//declare pin numbers for right motor
int speedR = 10;  //enable R
int goR = 7;      //ctrl 1 R
int backR = 6;    //ctrl 2 R


#include "IRLibAll.h"
// These are values are For the Adafruit Mini Remote
#define MY_PROTOCOL NEC
#define RIGHT_ARROW   0xfd50af //Move several clockwise
#define LEFT_ARROW    0xfd10ef //Move servo counterclockwise
#define SELECT_BUTTON 0xfd906f //Center the servo
#define UP_ARROW      0xfda05f //Increased number of degrees servo moves
#define DOWN_ARROW    0xfdb04f //Decrease number of degrees servo moves
#define MODE          0xfd609f

uint32_t Previous;//handles NEC repeat codes

//Create a receiver object to listen on pin 2
IRrecvPCI myReceiver(2);

//Create a decoder object 
IRdecode myDecoder; 

void setup() {
  //Servo Setup
  servo.attach(servoPin); // match servoPin with object servo
  
  //Motor Setup
  pinMode(speedL, OUTPUT); //set all Left motor pins as OUTPUT
  pinMode(goL, OUTPUT); 
  pinMode(backL, OUTPUT); 

  pinMode(speedR, OUTPUT); //set all Right motor pins as OUTPUT
  pinMode(goR, OUTPUT); 
  pinMode(backR, OUTPUT); 

  //IR setup
  myReceiver.enableIRIn(); // Start the receiver
}

//function to move straight forward
void go() {
  analogWrite(speedL, 255);
  digitalWrite(goL, HIGH);
  digitalWrite(backL, LOW);
  analogWrite(speedR, 255);
  digitalWrite(goR, HIGH);
  digitalWrite(backR, LOW);
}

//function to move straight backward
void back() {
  analogWrite(speedL, 255);
  digitalWrite(goL, LOW);
  digitalWrite(backL, HIGH);
  analogWrite(speedR, 255);
  digitalWrite(goR, LOW);
  digitalWrite(backR, HIGH);
}


//function to stop moving
void brake() {
  analogWrite(speedL, 0);
  analogWrite(speedR, 0);
}

//function to turn left
void turnLeft() {
  analogWrite(speedL, 255);
  digitalWrite(goL, LOW);
  digitalWrite(backL, HIGH);
  analogWrite(speedR, 255);
  digitalWrite(goR, HIGH);
  digitalWrite(backR, LOW);
}


//function to turn right
void turnRight() {
  analogWrite(speedL, 255);
  digitalWrite(goL, HIGH);
  digitalWrite(backL, LOW);
  analogWrite(speedR, 255);
  digitalWrite(goR, LOW);
  digitalWrite(backR, HIGH);
}

//location vars
int forward;
int right;
int left;
int wForward; 
int objCritDist = 70;
int del = 500;

void selfDrive() {
  forward = sonar.ping_cm();
  if (forward < objCritDist) {
    brake();
    servo.write(30);
    delay(del);
    right = sonar.ping_cm();
    delay(del);
    servo.write(150);
    delay(del);
    left = sonar.ping_cm();
    
    servo.write(90);
    wForward = 0;
    if (right > left) {
      while (wForward < objCritDist*2) {
        servo.write(90);
        wForward = sonar.ping_cm();
        turnRight();
      }
    }
    else {
      while (wForward < objCritDist*2) {
        servo.write(90);
        wForward = sonar.ping_cm();
        turnLeft();
      }
    }
  }
  else {
    servo.write(90);
    go();
  }
}


int res;
void loop() 
{ 
    if (myReceiver.getResults()) {
       myDecoder.decode();
       if(myDecoder.protocolNum==MY_PROTOCOL) {
         if(myDecoder.value==0xFFFFFFFF)
           myDecoder.value=Previous;
           if (myDecoder.value==LEFT_ARROW) turnLeft();
         switch(myDecoder.value) {
            case UP_ARROW:      res=1; break;
            case DOWN_ARROW:    res=2; break;
            case LEFT_ARROW:    res=3; break;
            case RIGHT_ARROW:   res=4; break;
            case SELECT_BUTTON: res=5; break;
            case MODE:          res=6; break;
         } 
         Previous=myDecoder.value;
       }
       myReceiver.enableIRIn();
    }
    if(res == 1) go();
    if(res == 2) back();
    if(res == 3) turnLeft();
    if(res == 4) turnRight();
    if(res == 5) brake();
    if(res == 6) selfDrive();
}
