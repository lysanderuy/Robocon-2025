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

// Sensor setup
const int sensorCount = 8;
const uint8_t sensorArray[sensorCount] = { SENSOR8, SENSOR7, SENSOR6, SENSOR5, SENSOR4, SENSOR3, SENSOR2, SENSOR1 };
uint16_t sensorValues[sensorCount];
QTRSensors qtr;

// Calibration variables
const int ledPin = LED_BUILTIN;
int ledState = LOW;
String direction = " ";
int position, error;
const int centerValue = 3500;
const int manualThreshold = 170;
static int sensorThreshold[sensorCount];

// Motor setup
Motor leftMotor = Motor(AIN1, AIN2, PWMA, 1, STBY);
Motor rightMotor = Motor(BIN1, BIN2, PWMB, 1, STBY);
const float speedMultiplierL = 1;
const float speedMultiplierR = 1;
const int baseSpeed = 120; // Straight
const int slightHSpeed = 120; // Slight High
const int slightLSpeed = 100; // Slight Low
const int moderateHSpeed = 120; // Moderate High
const int moderateLSpeed = 85; // Moderate Low
const int hardHSpeed = 135; // Hard High
const int hardLSpeed = 0; // Hard Low

void setup() {
  Serial.begin(9600);

  qtr.setTypeAnalog();
  qtr.setSensorPins(sensorArray, sensorCount);

  pinMode(ledPin, OUTPUT);
  pinMode(LED_CTRL, OUTPUT);
  digitalWrite(LED_CTRL, HIGH);
  delay(10);

  noCalibration();
  delay(2000);
}

void loop() {
  // qtr.read(sensorValues);

  // followLine1();

  // displayReadings(sensorValues, 'A');
  // displayReadings(sensorValues, 'D');
}

//======================================vvvLINE FOLLOWING FUNCTIONSvvv======================================//
void followLine1() {
  position = qtr.readLineBlack(sensorValues);
  error = position - centerValue;

  if (abs(error) <= 150) {
    Straight();
  }
  else if (error < -150 && error >= -400) {
    slightLeft();
  }
  else if (error > 150 && error <= 400) {
    slightRight();
  }
  else if (error < -400 && error >= -900) {
    moderateLeft();
  }
  else if (error > 400 && error <= 900) {
    moderateRight();  
  }
  else if (error < -900) {
    hardLeft();
  }
  else if (error > 900) {
    hardRight();
  }
}
//======================================vvvLINE FOLLOWING FUNCTIONSvvv======================================//

//======================================vvvCALIBRATIONS FUNCTIONSvvv======================================//
void noCalibration() {
  stop();

  for (int i=0 ; i<sensorCount ; i++) {
    sensorThreshold[i] = manualThreshold;
  }

  delay(500);
}

void calibrateSensors() {
  Serial.println(">>> Calibrating QTR sensors...");
  stop();  

  for (int i = 0; i < 200; i++) {
    qtr.calibrate();  
    if (i < 100) {
      runMotors(80, -80);
    }
    else {
      runMotors(-80, 80);
    }              
    delay(10);
  }

  stop();
  Serial.println(">>> Calibration complete.");

  for (uint8_t i = 0; i < sensorCount; i++) {
    int mn = qtr.calibrationOn.minimum[i];
    int mx = qtr.calibrationOn.maximum[i];
    sensorThreshold[i] = (mn + mx) / 2;

    Serial.print("S"); Serial.print(i);
    Serial.print(": min="); Serial.print(mn);
    Serial.print(" max="); Serial.print(mx);
    Serial.print(" thr="); Serial.println(sensorThreshold[i]);
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

  for (int i=0 ; i<sensorCount ; i++) {
    Serial.print("\t");
    Serial.print("t");
    Serial.print(8 - i);
    Serial.print(": ");
    Serial.print(sensorThreshold[i]);
  }

  Serial.print("\t");
  Serial.print("D: ");
  Serial.print(direction);

  Serial.println();
}
//======================================^^^SENSOR FUNCTIONS^^^======================================//

//======================================vvvCUSTOM MOTOR FUNCTIONSvvv======================================//
void runMotors(int left, int right) {
  leftMotor.drive(speedMultiplierL * left);
  rightMotor.drive(speedMultiplierR * right);
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

void Straight() {
  runMotors(baseSpeed, baseSpeed);
}

void hardLeft() {
  runMotors(hardLSpeed, hardHSpeed);
}

void hardRight() {
  runMotors(hardHSpeed, hardLSpeed);
}

void moderateLeft() {
  runMotors(moderateLSpeed, moderateHSpeed);
}

void moderateRight() {
  runMotors(moderateHSpeed, moderateLSpeed);
}

void slightLeft() {
  runMotors(slightLSpeed, slightHSpeed);
}

void slightRight() {
  runMotors(slightHSpeed, slightLSpeed);
}
//======================================^^^CUSTOM MOTOR FUNCTIONS^^^======================================//