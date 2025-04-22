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

// Global sensor arrays
const int sensorArray[8] = {SENSOR8, 
                            SENSOR7, 
                            SENSOR6,  
                            SENSOR5, 
                            SENSOR4, 
                            SENSOR3, 
                            SENSOR2, 
                            SENSOR1};
int sensorValActive[8];
int sensorValPassive[8];
int sensorValTrue[8];

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 8; i++) {
    pinMode(sensorArray[i], INPUT); // Setting pins sensor pins as input
  }

  pinMode(LED_CTRL, OUTPUT); // Setting LED_CTRL pin as output
  delay(3000);
}

void loop() {
  readSensorsActive(sensorValActive); 
  //readSensorsPassive(sensorValPassWive);
  //getTrueReading(sensorValActive, sensorValPassive, sensorValTrue);

  displayReadings(sensorValActive);
}

// Captures sensor readings with IR LEDs turned ON
void readSensorsActive (int* sensorValActive) {
  digitalWrite(LED_CTRL, HIGH);
  delay(5);

  for (int i=0 ; i<8 ; i++) {
    sensorValActive[i] = analogRead(sensorArray[i]);
  }

  digitalWrite(LED_CTRL, LOW);
}

// Captures sensor readings with IR LEDs turned OFF
void readSensorsPassive (int* sensorValPassive) {
  digitalWrite(LED_CTRL, LOW);

  for (int i=0 ; i<8 ; i++) {
    sensorValPassive[i] = analogRead(sensorArray[i]);
  }
}

// Computes true IR sensor readings by subtracting ambient light values
void getTrueReading (int* sensorValActive, int* sensorValPassive, int* sensorValTrue) {
  for (int i=0 ; i<8 ; i++) {
    sensorValTrue[i] = sensorValActive[i] - sensorValPassive[i];
  }
}

// Displays sensor readings in serial monitor
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