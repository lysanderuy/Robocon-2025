#include <QTRSensors.h>
#include <SparkFun_TB6612.h>

// Calibration variables
const int ledPin = LED_BUILTIN;
int ledState = LOW;
int threshold = 0;
int thresholdAllowance = 250;
String direction = " ";
int BASE_PWM = 200;

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

// Motor setup
Motor leftMotor = Motor(AIN1, AIN2, PWMA, 1, STBY);
Motor rightMotor = Motor(BIN1, BIN2, PWMB, 1, STBY);

// Sensor setup
const uint8_t sensorArray[8] = { SENSOR8, SENSOR7, SENSOR6, SENSOR5, SENSOR4, SENSOR3, SENSOR2, SENSOR1 };
uint16_t sensorValues[8];
QTRSensors qtr;
int SensorCount = 8;

void setup() {
  Serial.begin(9600);

  qtr.setTypeAnalog();
  qtr.setSensorPins(sensorArray, SensorCount);

  pinMode(ledPin, OUTPUT);
  pinMode(LED_CTRL, OUTPUT);
  digitalWrite(LED_CTRL, HIGH);
  delay(5);

  noCalibration();
}

void loop() {
  qtr.read(sensorValues);

  followLine1();
}

//======================================vvvLINE FOLLOWING FUNCTIONSvvv======================================//
void followLine1() {
  int indexDarkest = 0;  // Initial index of sensor detecting darkest line

  // Keeping track of the sensor with darkest reading
  for (int i = 0; i < 8; i++) {
    if (sensorValues[i] > sensorValues[indexDarkest])
      indexDarkest = i;
  }

  if (sensorValues[indexDarkest] > threshold) {
    switch (indexDarkest) {
      case 0:
        runMotors(0, 1);
        direction = "LLLL";
        break;
      case 1:
        runMotors(.2, 1);
        direction = "LLL";
        break;
      case 2:
        runMotors(.65, 1);
        direction = "LL";
        break;
      case 3:
        runMotors(.88, 1);
        direction = "L";
        break;
      case 4:
        runMotors(1, .88);
        direction = "R";
        break;
      case 5:
        runMotors(1, .65);
        direction = "RR";
        break;
      case 6:
        runMotors(1, .2);
        direction = "RRR";
        break;
      case 7:
        runMotors(1, 0);
        direction = "RRRR";
        break;
      default:
        break;
    }
  }
}
//======================================vvvLINE FOLLOWING FUNCTIONSvvv======================================//

//======================================vvvCALIBRATIONS FUNCTIONSvvv======================================//
void noCalibration() {
  stop();

  threshold = 200;

  runMotors(180, 180);
  delay(500);
}

void calibrateSensors() {
  stop();

  int numSamples = 10;
  int arrayThreshold[8] = { 0 };

  for (int j = 0; j < numSamples; j++) {
    qtr.read(sensorValues);
    for (int i = 0; i < 8; i++) {
      arrayThreshold[i] += sensorValues[i];
    }
    delay(50);
  }

  for (int i = 0; i < 8; i++) {
    arrayThreshold[i] /= numSamples;
  }

  int indexWhitest = 0;
  for (int i = 1; i < 8; i++) {
    if (arrayThreshold[i] < arrayThreshold[indexWhitest])
      indexWhitest = i;
  }

  threshold = arrayThreshold[indexWhitest] + thresholdAllowance;

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
void displayReadings(int* sensorReadings) {
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
  Serial.print("\t");

  Serial.print("\t");
  Serial.print(direction);

  Serial.println();
}
//======================================^^^SENSOR FUNCTIONS^^^======================================//

//======================================vvvCUSTOM MOTOR FUNCTIONSvvv======================================//
void runMotors(int left, int right) {
  leftMotor.drive(BASE_PWM * left);
  rightMotor.drive(BASE_PWM * right);
}

void stop() {
  leftMotor.drive(0);
  rightMotor.drive(0);
}

void spinRight(int speed) {
  leftMotor.drive(speed);
  rightMotor.drive(-1 * speed);
}

void spinLeft(int speed) {
  leftMotor.drive(-1 * speed);
  rightMotor.drive(speed);
}
//======================================^^^CUSTOM MOTOR FUNCTIONS^^^======================================//

//======================================vvvEMOTE FUNCTIONSvvv======================================//
void emoteOne() {
  runMotors(255, 255);
  delay(500);
  stop();
  delay(300);
  spinLeft(255);
  delay(225);
  stop();
  delay(300);

  runMotors(255, 255);
  delay(500);
  stop();
  delay(300);
  spinLeft(255);
  delay(225);
  stop();
  delay(300);

  runMotors(255, 255);
  delay(500);
  stop();
  delay(300);
  spinLeft(255);
  delay(225);
  stop();
  delay(300);

  runMotors(255, 255);
  delay(500);
  stop();
  delay(300);
  spinLeft(255);
  delay(225);
  stop();
  delay(300);
}
//======================================^^^EMOTE FUNCTIONS^^^======================================//