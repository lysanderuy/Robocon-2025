#include <QTRSensors.h>
#include <SparkFun_TB6612.h>

// QTR 8A pin assignments
#define SENSOR8 A7
#define SENSOR7 A6
#define SENSOR6 A5
#define SENSOR5 A4
#define SENSOR4 A3
#define SENSOR3 A2
#define SENSOR2 A1
#define SENSOR1 A0
#define LED_CTRL 6

// TB6612FNG pin assignments
#define PWMA 3
#define PWMB 11
#define AIN2 7
#define AIN1 8
#define BIN1 9
#define BIN2 10
#define STBY 12

//DIP Switch pin assignments
#define SW2 2
#define SW1 4
#define SW0 5

// Sensor Array setup
QTRSensors qtr;

// Global sensor arrays
int sensorCount = 8;
const uint8_t sensorArray[8] = {SENSOR8, SENSOR7,  SENSOR6,  SENSOR5, SENSOR4, SENSOR3, SENSOR2, SENSOR1};
uint16_t sensorValues[8] ;

// Motors setup
Motor leftMotor = Motor(AIN1, AIN2, PWMA, 1, STBY);
Motor rightMotor = Motor(BIN1, BIN2, PWMB, 1, STBY);

// S-Curve variables
float previousSpeed = 0.0;
float targetSpeed = 150.0;
float currentSpeed = 0.0;
float maxJerk = 5050.0;
float maxAccel = 8500.0;
float stepTime = 0.01;
unsigned long previousMillis = 0.0;
unsigned long interval = stepTime * 1000.0;

// Controller variables
float kp;
float kd;
int proportional = 0;
int derivative = 0;
int previousProportional;

// Calibration variables
const int ledPin = LED_BUILTIN;
int ledState = LOW;
static int sensorThreshold[8];
int legitMode;

//==============================================vvvSETUPvvv==============================================//
void setup() {
  Serial.begin(9600);
  
  qtr.setTypeAnalog();
  qtr.setSensorPins(sensorArray, sensorCount);

  pinMode(LED_CTRL, OUTPUT);
  digitalWrite(LED_CTRL, HIGH);
  delay(10);

  pinMode(SW0, INPUT);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  delay (10);

  legitMode = readMode();

  switch (legitMode) {
    case 0:
      targetSpeed = 150;
      kp = 0.0425;
      kd = 0.4;
      fastCalibrate();
      break;
    case 1:
      targetSpeed = 180;
      kp = 0.065;
      kd = 0.65;
      fastCalibrate();
      break;
    case 2:
      targetSpeed = 200;
      kp = 0.068;
      kd = 1; //0.7
      fastCalibrate();
      break;
    case 4:
      targetSpeed = 220;
      kp = 0.075;
      kd = 0.95;
      fastCalibrate();
      break;
    default:
      break;
  }
}
//==============================================^^^SETUP^^^==============================================//

//==============================================vvvMAIN LOOPvvv==============================================//
void loop() {
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    currentSpeed = updateSpeed(currentSpeed, targetSpeed, maxJerk, maxAccel, stepTime);
  }

  pidControl();
}
//==============================================^^^MAIN LOOP^^^==============================================//

//======================================vvvLINE FOLLOWING FUNCTIONSvvv======================================//
void pidControl() {
  uint16_t position = qtr.readLineBlack(sensorValues);

  proportional = position - 3500;
  derivative = proportional - previousProportional;
  previousProportional = proportional;

  int PID = (proportional * kp) + (derivative * kd);

  int leftSpeed = constrain((currentSpeed + PID), -targetSpeed, targetSpeed);
  int rightSpeed = constrain((currentSpeed - PID), -targetSpeed, targetSpeed);

  leftMotor.drive(leftSpeed);
  rightMotor.drive(rightSpeed);
}
//======================================^^^LINE FOLLOWING FUNCTIONS^^^======================================//

//======================================vvvCALIBRATIONS FUNCTIONSvvv======================================//
void noCalibration() {
  stop();

  for (int i = 0; i < sensorCount; i++) {
    sensorThreshold[i] = 350;
  }

  delay(500);
}

void fastCalibrate() {
  delay(500);
  for (int i = 0; i < 200; i++) {
    qtr.calibrate();
  }
}

void blinkLED() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}

int readMode () { 
  // for (int i=0 ; i<3 ; i++) {
    int modeRet = ((digitalRead(SW2) << 2) | (digitalRead(SW1) << 1) | (digitalRead(SW0)));
  // }

  return modeRet;
}
//======================================^^^CALIBRATIONS FUNCTIONS^^^======================================//

//======================================vvvSENSOR FUNCTIONSvvv======================================//
void displayReadings(int* sensorReadings, char type) {
  if (type == 'A') {
    int val_8 = sensorReadings[0];
    int val_7 = sensorReadings[1];
    int val_6 = sensorReadings[2];
    int val_5 = sensorReadings[3];
    int val_4 = sensorReadings[4];
    int val_3 = sensorReadings[5];
    int val_2 = sensorReadings[6];
    int val_1 = sensorReadings[7];

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
  } 
  else if (type == 'D') {
    int first = (sensorReadings[0] > sensorThreshold[0]) ? 1 : 0;
    int second = (sensorReadings[1] > sensorThreshold[1]) ? 1 : 0;
    int third = (sensorReadings[2] > sensorThreshold[2]) ? 1 : 0;
    int fourth = (sensorReadings[3] > sensorThreshold[3]) ? 1 : 0;
    int fifth = (sensorReadings[4] > sensorThreshold[4]) ? 1 : 0;
    int sixth = (sensorReadings[5] > sensorThreshold[5]) ? 1 : 0;
    int seventh = (sensorReadings[6] > sensorThreshold[6]) ? 1 : 0;
    int eighth = (sensorReadings[7] > sensorThreshold[7]) ? 1 : 0;

    Serial.print("8: ");
    Serial.print(first);
    Serial.print("\t");
    Serial.print("7: ");
    Serial.print(second);
    Serial.print("\t");
    Serial.print("6: ");
    Serial.print(third);
    Serial.print("\t");
    Serial.print("5: ");
    Serial.print(fourth);
    Serial.print("\t");
    Serial.print("4: ");
    Serial.print(fifth);
    Serial.print("\t");
    Serial.print("3: ");
    Serial.print(sixth);
    Serial.print("\t");
    Serial.print("2: ");
    Serial.print(seventh);
    Serial.print("\t");
    Serial.print("1: ");
    Serial.print(eighth);
  }

  Serial.println();
}

void displayThresholds() {
  for (int i = 0; i < sensorCount; i++) {
    Serial.print("t");
    Serial.print(8 - i);
    Serial.print(": ");
    Serial.print(sensorThreshold[i]);
    Serial.print("\t");
  }

  Serial.println();
}
//======================================^^^SENSOR FUNCTIONS^^^======================================//

//=========================================vvvMOTOR FUNCTIONSvvv=========================================//
void stop() {
  leftMotor.drive(0);
  rightMotor.drive(0);
}

void runMotors(int left, int right) {
  leftMotor.drive(left);
  rightMotor.drive(right);
}
//=========================================^^^MOTOR FUNCTIONS^^^=========================================//

//=========================================vvvS-CURVE FUNCTIONvvv=========================================//
float updateSpeed(float currentSpeed, float targetSpeed, float maxJerk, float maxAccel, float stepTime) {
  static float currentAccel = 0.0;
  float speedError = targetSpeed - currentSpeed;

  if (abs(speedError) < 0.1) {
    currentSpeed = targetSpeed;
    currentAccel = 0;
    return currentSpeed;
  }

  if (speedError > 0) {
    currentAccel += maxJerk * stepTime;
    if (currentAccel > maxAccel) currentAccel = maxAccel;
  } else {
    currentAccel = maxJerk * stepTime;
    if (currentAccel < -maxAccel) currentAccel = -maxAccel;
  }

  currentSpeed += currentAccel * stepTime;
  if ((speedError > 0 && currentSpeed > targetSpeed) || (speedError < 0 && currentSpeed < targetSpeed)) {
    currentSpeed = targetSpeed;
  }
  return currentSpeed;
}
//=========================================^^^S-CURVE FUNCTION^^^=========================================//
