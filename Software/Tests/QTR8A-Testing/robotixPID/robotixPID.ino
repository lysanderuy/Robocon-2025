#include <L298.h>

//QTR SENSOR PIN ASSIGNMENTS
#define SENSOR1 A0
#define SENSOR2 A1
#define SENSOR3 A2
#define SENSOR4 A3
#define SENSOR5 A4
#define SENSOR6 A5
#define SENSOR7 A6
#define SENSOR8 A7
#define LED_CTRL 6

//TB6612FNG DRIVER MOTOR PINS ASSIGNMENT
#define AIN1 8
#define AIN2 7
#define BIN1 9
#define BIN2 10
#define PWMA 3
#define PWMB 11

L298N motor1(PWMA, AIN1, AIN2);
L298N motor2(PWMB, BIN1, BIN2);

const int sensorArray[8] = {SENSOR8, SENSOR7, SENSOR6, SENSOR5, SENSOR4, SENSOR3, SENSOR2, SENSOR1};
int sensorValActive[8];

void setup() {
  Serial.begin(9600);

  for (int i = 0; i <8; i++)
  {
    pinMode(sensorArray[i], INPUT);
  }

  pinMode(LED_CTRL, OUTPUT);

}

void loop() {
  readSensorsActive(sensorValActive);
  displayReadings(sensorValActive);
  
  //initializing of sensors
  pinMode (SENSOR1, INPUT);
  pinMode (SENSOR2, INPUT);
  pinMode (SENSOR3, INPUT);
  pinMode (SENSOR4, INPUT);
  pinMode (SENSOR5, INPUT);
  pinMode (SENSOR6, INPUT);
  pinMode (SENSOR7, INPUT);
  pinMode (SENSOR8, INPUT);
  pinMode (LED_CTRL, OUTPUT);
  delay(3000);

}

void readSensorsActive (int* sensorValActive)
{
  digitalWrite(LED_CTRL, HIGH);
  delay(5);

  for (int i=0; i<8; i++)
  {
    sensorValActive[i] = analogRead(sensorArray[i]);
  }

  digitalWrite(LED_CTRL, LOW);
}

void displayReadings (int* sensorReadings) {
  int val_8 = sensorReadings[0];  
  int val_7 = sensorReadings[1];          
  int val_6 = sensorReadings[2];    
  int val_5 = sensorReadings[3];       
  int val_4 = sensorReadings[4];         
  int val_3 = sensorReadings[5];      
  int val_2 = sensorReadings[6];       
  int val_1 = sensorReadings[7];

  Serial.print("\t");
  Serial.print("8: ");
  Serial.print(val_8);
  Serial.print("\t");
  Serial.print("7: ");
  Serial.print(val_7);
  Serial.print("\t");
  Serial.print("6: ");
  Serial.print(val_6);
  Serial.print("\t");
  Serial.print("5: ");
  Serial.print(val_5);
  Serial.print("\t");
  Serial.print("4: ");
  Serial.print(val_4);
  Serial.print("\t");
  Serial.print("3: ");
  Serial.print(val_3);
  Serial.print("\t");
  Serial.print("2: ");
  Serial.print(val_2);
  Serial.print("\t");
  Serial.print("1: ");
  Serial.print(val_1);

  Serial.println();
}

