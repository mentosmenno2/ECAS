#include <Servo.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

//set reusable variables
boolean braking = false;

//set ultrasonic variables
unsigned long echo = 0;
int ultraSoundSignal = 9; // Ultrasound signal pin
unsigned long ultrasoundValue = 0;
int distanceThreshold = 100; //in cm

//set servo variables
int servoPort = 8;

//set mp3player variables
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

Servo myservo;

void setup() {
  //start the serials
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  //set pinmode for ultrasonic
  pinMode(ultraSoundSignal,OUTPUT);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  //wait for player to boot
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
}

void loop() {
  int currentDistance = ping(); //read distance
  Serial.println("distance: " + String(currentDistance));
  if(currentDistance < distanceThreshold && currentDistance != 0) { //if distance is valid, and an object is detected, brake.
    brake();
  } else {
    cancelBrake();
  }
  delay(1000);
}

void brake() {
  if(!braking) {
    // move the brakes to braking position
    myservo.attach(servoPort);
    myservo.write(180);
    delay(500);
    myservo.detach();
    braking = true;
    //play sound
    playBrakeSound();
  }
  Serial.println("Remmen!");
}

void cancelBrake() {
  if(braking) {
    // move the brakes to standard position
    myservo.attach(servoPort);
    myservo.write(0);
    delay(500);
    myservo.detach();
    braking = false;
  }
  Serial.println("Racen!");
}

void playBrakeSound() {
  myDFPlayer.play(1);  //Play the first mp3
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  Serial.println("Tring!");
}

unsigned long ping() { 
  pinMode(ultraSoundSignal, OUTPUT); // Switch signalpin to output
  digitalWrite(ultraSoundSignal, LOW); // Send low pulse 
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(ultraSoundSignal, HIGH); // Send high pulse
  delayMicroseconds(5); // Wait for 5 microseconds
  digitalWrite(ultraSoundSignal, LOW); // Holdoff
  pinMode(ultraSoundSignal, INPUT); // Switch signalpin to input
  digitalWrite(ultraSoundSignal, HIGH); // Turn on pullup resistor
  // please note that pulseIn has a 1sec timeout, which may
  // not be desirable. Depending on your sensor specs, you
  // can likely bound the time like this -- marcmerlin
  // echo = pulseIn(ultraSoundSignal, HIGH, 38000)
  echo = pulseIn(ultraSoundSignal, HIGH); //Listen for echo
  ultrasoundValue = echo / 58.138; //convert to CM
  return ultrasoundValue;
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
