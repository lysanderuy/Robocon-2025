#include <QTRSensors.h>
#include <SparkFun_TB6612.h>

// QTR 8A pin assignments
//#define SENSOR8 A7
#define SENSOR7 A6
#define SENSOR6 A5
#define SENSOR5 A4
#define SENSOR4 A3
#define SENSOR3 A2
#define SENSOR2 A1
//#define SENSOR1 A0
#define LED_CTRL 6  

// TB6612FNG pin assignments
#define PWMA 3
#define PWMB 11
#define AIN2 7
#define AIN1 8
#define BIN1 9
#define BIN2 10

// Motor setup
Motor leftMotor = Motor(AIN2, AIN1, PWMA, 1, 255);
Motor rightMotor = Motor(BIN1, BIN2, PWMB, 1, 255);

// Calibration variables
unsigned long calibrationStartTime = 0; 
const int calibrationDuration = 5000;
const int ledPin = LED_BUILTIN;
int ledState = LOW;
bool calibrationPeriod = false;
int threshold = 0;

// Global sensor arrays
const uint8_t sensorArray[6] = {SENSOR7,  SENSOR6,  SENSOR5, SENSOR4, SENSOR3, SENSOR2};
int sensorValues[6];

// PID Variables
QTRSensors qtr;
int SensorCount = 6;
int position;
float Kp = 0.1;
float Ki = 0.01; 
float Kd = 0.07;
int P, D, I, previousError, PIDvalue, error;
int lsp, rsp;
int tempPosition;
int centerPosition= 2500;
int minSpeed = -145;
int maxSpeed = 145;
int baseSpeed = 145;

void setup() {
  Serial.begin(9600);

  qtr.setSensorPins(sensorArray, SensorCount);

  digitalWrite(LED_CTRL, HIGH);
  delay(5);

  noCalibration();
}

void loop() {
  readSensors(sensorValues);

  position = qtr.readLineBlack(sensorValues);
  error = centerPosition - position;

  P = error;
  I = constrain(I + error, minSpeed, maxSpeed);
  D = error - previousError;
  previousError = error;

  int motorSpeed = (Kp * P) + (Ki * I) + (Kd * D);

  // baseSpeed = 140 + 20*sin(2*M_PI*10*millis()/1000);

  int leftSpeed = constrain((baseSpeed - motorSpeed), minSpeed, maxSpeed);
  int rightSpeed = constrain((baseSpeed + motorSpeed), minSpeed, maxSpeed);

  leftMotor.drive(leftSpeed);
  rightMotor.drive(rightSpeed);
}

//======================================vvvCALIBRATIONS FUNCTIONSvvv======================================//
void noCalibration() {
  threshold = 200;

  slightForward(150);
  delay(500);
}

void calibrateSensors() {
  if (!calibrationPeriod) {
    stop();

    int arrayThreshold[6] = { SENSOR7,  SENSOR6,  SENSOR5, SENSOR4, SENSOR3, SENSOR2};
    int indexThreshold = 0;
    
    for (int i = 0 ; i < 6 ; i ++) {
      if (arrayThreshold[i] < arrayThreshold[indexThreshold]) 
        indexThreshold = i;
    }

    threshold = arrayThreshold[indexThreshold] + 250;
  }
  
  blinkLED();
}

void blinkLED() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);   
    delay(500);                  
    digitalWrite(ledPin, LOW);    
    delay(500);                   
  }
}
//======================================^^^CALIBRATIONS FUNCTIONS^^^======================================//

//======================================vvvSENSOR FUNCTIONSvvv======================================//
void readSensors (int* sensorVal) {
  for (int i=0 ; i<6 ; i++) {
    sensorVal[i] = analogRead(sensorArray[i]);
  }
}

void displayReadings (int* sensorReadings, int position, int error) {
  //int val_8 = sensorReadings[0];  
  int val_7 = sensorReadings[1];          
  int val_6 = sensorReadings[2];    
  int val_5 = sensorReadings[3];       
  int val_4 = sensorReadings[4];         
  int val_3 = sensorReadings[5];      
  int val_2 = sensorReadings[6];       
  //int val_1 = sensorReadings[7];

  Serial.print("\t");
  Serial.print("8: ");
  //Serial.print(val_8);
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
  //Serial.print(val_1);
  Serial.print("\t");
  Serial.print("POS: ");
  Serial.print(position);
  Serial.print("\t");
  Serial.print("ERR: ");
  Serial.print(error);

  Serial.println();
}
//======================================^^^SENSOR FUNCTIONS^^^======================================//

//======================================vvvCUSTOM MOTOR FUNCTIONSvvv======================================//
void stop() {
  leftMotor.drive(0);
  rightMotor.drive(0);
}

void slightForward(int speed) {
    leftMotor.drive(speed);
    rightMotor.drive(speed);
}

void spinRight() {
  leftMotor.drive(255);
  rightMotor.drive(-255);
}

void spinLeft() {
  leftMotor.drive(-255);
  rightMotor.drive(255);
}
//======================================^^^CUSTOM MOTOR FUNCTIONS^^^======================================//