#include <Servo.h>

unsigned long echo = 0;
int ultraSoundSignal = 9; // Ultrasound signal pin
int servoPort = 10;
unsigned long ultrasoundValue = 0;
int distanceThreshold = 100; //in cm
boolean braking = false;

Servo myservo;

void setup() {
  Serial.begin(9600);
  pinMode(ultraSoundSignal,OUTPUT);
}

void loop() {
  int currentDistance = ping();
  Serial.println("distance: " + String(currentDistance));
  if(currentDistance < distanceThreshold && currentDistance != 0) {
    brake();
  } else {
    cancelBrake();
  }
  delay(1000);
}

void brake() {
  if(!braking) {
    myservo.attach(servoPort);
    myservo.write(180);
    delay(500);
    myservo.detach();
    braking = true;
  }
  playBrakeSound();
  Serial.println("Remmen!");
}

void cancelBrake() {
  if(braking) {
    myservo.attach(servoPort);
    myservo.write(0);
    delay(500);
    myservo.detach();
    braking = false;
  }
  Serial.println("Racen!");
}

void playBrakeSound() {
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


