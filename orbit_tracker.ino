/* Juan Salazar 6/23/2019
 * Teensy 3.2 firmware for the Thing Pointer v1.
 * Receives commands via Serial/UART from w/ RPi.
 * Tracks the International Space Station (ISS).
 */

#include <Servo.h>
#define coil_A_1_pin  23
#define coil_A_2_pin  22
#define coil_B_1_pin  21
#define coil_B_2_pin  20


Servo arrow;
int servo_pin = 9;
int enable_pin = 18;
int stepDelay = 10;
int sprockRatio = 0;
float deg2step = 360/200;


// Ephem variables
float az; // reported azimuth
float alt; //reported altitude

void setup() {
  Serial.begin(9600);
  arrow.attach(servo_pin);  
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);

  // Stepper check 
  forward(50, 20);
  delay(500);
  backwards(50, 20);
  
}

void loop() {
//  while (Serial.available())  {
//    alt = Serial.parseFloat();
//    az = Serial.parseFloat();
//    Serial.flush();
//    Serial.print(alt);
//    Serial.print(' ');
//    Serial.println(az);
//    } 
  
}


/*
 * Stepper Motor Helper functions
 */
 
void setStep(boolean first, boolean sec, boolean third, boolean fourth){
  digitalWrite(coil_A_1_pin, first);
  digitalWrite(coil_A_2_pin, sec);
  digitalWrite(coil_B_1_pin, third);
  digitalWrite(coil_B_2_pin, fourth);
}

void forward(int dt, int steps){
  for (int i  = 0; i <= steps; i++){
    setStep(1, 0, 0, 0);
    delay(dt);
    setStep(0, 1, 0, 0);
    delay(dt);
    setStep(0, 0, 1, 0);
    delay(dt);
    setStep(0, 0, 0, 1);
    delay(dt);
  }
}

void backwards(int dt, int steps){
  for (int i = 0; i <= steps; i++){
    setStep(0, 0, 0, 1);
    delay(dt);
    setStep(0, 0, 1, 0);
    delay(dt);
    setStep(0, 1, 0, 0);
    delay(dt);
    setStep(1, 0, 0, 0);
    delay(dt);
  }
}
