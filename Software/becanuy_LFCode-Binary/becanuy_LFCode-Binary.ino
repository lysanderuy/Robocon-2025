#include <QTRSensors.h>
#include <SparkFun_TB6612.h>

// // DIP Switch pin assignments
// #define SW0 
// #define SW1
// #define SW2

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
bool onWhite = false;
static int sensorThreshold[sensorCount];

// Motor setup
Motor leftMotor = Motor(AIN1, AIN2, PWMA, 1, STBY);
Motor rightMotor = Motor(BIN1, BIN2, PWMB, 1, STBY);

// Speed Variables
const float speedMultiplierL = 1;
const float speedMultiplierR = 1;
const int baseSpeed = 80; // Straight
const int slightHSpeed = 80; // Slight High
const int slightLSpeed = 65; // Slight Low
const int moderateHSpeed = 90; // Moderate High
const int moderateLSpeed = 45; // Moderate Low
const int hardHSpeed = 255; // Hard High
const int hardLSpeed = -255; // Hard Low

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
  qtr.read(sensorValues);

  followLine1();

  // displayReadings(sensorValues, 'A');
  // displayReadings(sensorValues, 'D');
}

//======================================vvvLINE FOLLOWING FUNCTIONSvvv======================================//
void followLine1() {
  int position=0;
  int first, second, third, fourth, fifth, sixth, seventh, eighth;

  first = (sensorValues[0] > sensorThreshold[0]) ? 1 : 0;
  second = (sensorValues[1] > sensorThreshold[1]) ? 1 : 0;
  third = (sensorValues[2] > sensorThreshold[2]) ? 1 : 0;
  fourth = (sensorValues[3] > sensorThreshold[3]) ? 1 : 0;
  fifth = (sensorValues[4] > sensorThreshold[4]) ? 1 : 0;
  sixth = (sensorValues[5] > sensorThreshold[5]) ? 1 : 0;
  seventh = (sensorValues[6] > sensorThreshold[6]) ? 1 : 0;
  eighth = (sensorValues[7] > sensorThreshold[7]) ? 1 : 0;

  int sensorReadings[sensorCount] = { first, second, third, fourth, fifth, sixth, seventh, eighth };

  if ((sensorReadings[0] == 1 && sensorReadings[7] == 1 && (sensorReadings[3] == 0 || sensorReadings[4] == 0))) { 
    onWhite = true;
  }
  else if ((sensorReadings[0] == 0 && sensorReadings[7] == 0 && (sensorReadings[3] == 1 || sensorReadings[4] == 1))) {
    onWhite = false;
  }
 
  if (onWhite) {
    for (int i=0 ; i<sensorCount ; i++) {
      sensorReadings[i] = (sensorReadings[i] == 0) ? 1 : 0;
    }
  }
  
  for (int i = 0; i < sensorCount; i++) {
    position |= sensorReadings[i] << (7 - i);
  }
  
  switch (position) {
    //============ Straight Cases ============
    case 0b00111100:
    case 0b01111110:
    case 0b11111111:
    case 0b00000000:
      Straight();
      direction = "Straight";
      break;
    //=========== Slight Left Cases ===========
    case 0b01111000:
    case 0b01111100:
    case 0b00110000:
    case 0b00010000:
    case 0b01110000:
      slightLeft();
      direction = "Slight Left";
      break;
    //=========== Slight Right Cases ===========
    case 0b00011110:
    case 0b00111110:
    case 0b00001100:
    case 0b00001000:
    case 0b00011100:
      slightRight();
      direction = "Slight Right";
      break;
    //========== Moderate Left Cases ==========11110000
    case 0b11100000:
    case 0b11111000:
    case 0b00111000:
    case 0b01100000:
    case 0b11111100:
    case 0b11111110:
    case 0b00100000:
      moderateLeft();
      direction = "Moderate Left";
      break;
    //========= Moderate Right Cases =========
    case 0b00001111:
    case 0b00000111:
    case 0b00011111:
    case 0b00001110:
    case 0b00000110:
    case 0b00000100:
    case 0b00111111:
    case 0b01111111:
      moderateRight();
      direction = "Moderate Right";
      break;
    //============ Hard Left Cases ============
    case 0b11000000:
    case 0b10000000:
    case 0b01000000:
      hardLeft();
      direction = "Hard Left";
      break;
    //=========== Hard Right Cases ===========
    case 0b00000011:
    case 0b00000001:
    case 0b00000010:
      hardRight();
      direction = "Hard Right";
      break;
    //================= Default =================
    default:
      break;
  }
}
//======================================vvvLINE FOLLOWING FUNCTIONSvvv======================================//

//======================================vvvCALIBRATIONS FUNCTIONSvvv======================================//
void noCalibration() {
  stop();

  for (int i=0 ; i<sensorCount ; i++) {
    sensorThreshold[i] = 140;
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

  Serial.print("\t");
  Serial.print("D: ");
  Serial.print(direction);

  Serial.println();
}

void displayThresholds() {
  for (int i=0 ; i<sensorCount ; i++) {
    Serial.print("t");
    Serial.print(8 - i);
    Serial.print(": ");
    Serial.print(sensorThreshold[i]);
    Serial.print("\t");
  }

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

void slightLeft() {
  runMotors(slightLSpeed, slightHSpeed);
}

void slightRight() {
  runMotors(slightHSpeed, slightLSpeed);
}

void moderateLeft() {
  runMotors(moderateLSpeed, moderateHSpeed);
}

void moderateRight() {
  runMotors(moderateHSpeed, moderateLSpeed);
}

void hardLeft() {
  runMotors(hardLSpeed, hardHSpeed);
}

void hardRight() {
  runMotors(hardHSpeed, hardLSpeed);
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