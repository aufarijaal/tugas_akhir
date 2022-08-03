#include <Servo.h>

// Pin definition
int pinServo = 0; /*  */

// Object definition
Servo servo;

void setup() {
  servo.attach(pinServo);
} 

void loop() {
  servoBuka();
  delay(1000);
  servoTutup();
}

void servoBuka() {
  servo.write(180);
}

void servoTutup() {
  servo.write(0);
}