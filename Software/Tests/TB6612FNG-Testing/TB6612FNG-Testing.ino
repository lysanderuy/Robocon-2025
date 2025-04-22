// Motor PWMs
#define PWMA 3
#define PWMB 11

#define AI2 7
#define AI1 8

#define BI1 9
#define BI2 10

#define BASE_PWM 255

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  analogWrite(PWMB, 1*BASE_PWM); 
  analogWrite(PWMA, 1*BASE_PWM);

  digitalWrite(AI1, 1); //MotorA CCW
  digitalWrite(AI2, 0); //MotorA CW

  digitalWrite(BI1, 0); //MotorA CCW
  digitalWrite(BI2, 1); //MotorA CW
}
