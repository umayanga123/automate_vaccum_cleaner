#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#define TRIG_PIN A4
#define ECHO_PIN A5
#define MAX_DISTANCE 200
#define MAX_SPEED 75 // sets speed of DC  motors


NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(3, MOTOR12_1KHZ);

AF_DCMotor motor3(2, MOTOR12_1KHZ);

Servo myservo;

boolean goesForward = false;
int distance = 100;
int speedSet = 0;

int DisObstaclePin = 2; // This is our input pin
int DisObstacle = HIGH; // HIGH MEANS NO OBSTACLE


int receiver = 13; // Signal Pin of IR receiver to Arduino Digital Pin 11
//int power_on;
int fan_status,power_on;

IRrecv irrecv(receiver);           // create instance of 'irrecv'
decode_results results; // create instance of 'decode_results'


//----( SETUP: RUNS ONCE )----//
void setup()
{
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode");

  power_on = 1;
  fan_status = 0;

  pinMode(DisObstaclePin, INPUT);
  myservo.attach(9);
  myservo.write(115);
  delay(1000);

  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  DisObstacle = digitalRead(DisObstaclePin);
  delay(100);

  irrecv.enableIRIn(); // Start the receiver

}


void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{

  Serial.println("Begin loop");
  delay(1000);
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    Serial.print("Start reciving");
    translateIR();
    irrecv.resume(); // receive the next value
  }

  //delay(2000); 
  if (power_on == 1) {
    Serial.println("POWER ON");
    int distanceR = 0;
    int distanceL =  0;
    delay(40);
    if (distance <= 25 || DisObstacle == HIGH)
    {
      Serial.print("Turning");
      moveStop();
      delay(100);
      moveBackward();
      delay(300);
      moveStop();
      delay(200);
      distanceR = lookRight();
      delay(200);
      distanceL = lookLeft();
      delay(200);

      if (distanceR >= distanceL)
      {
        turnRight();
        moveStop();
      } else
      {
        turnLeft();
        moveStop();
      }
    } else
    {
      moveForward();
      Serial.print("Forword");
    }
    distance = readPing();
    delay(50);
    DisObstacle = digitalRead(DisObstaclePin);
    delay(50);
  }

  if (fan_status == 1) {
    startFan();
    delay(50);
  } else {
    stopFan();
    delay(50);
  }
  
 // irrecv.resume(); // receive the next value
}/* --(end main loop )-- */




/*-----( Functions )-----*/
void translateIR() // takes action based on IR code received
{
  switch (results.value)
  {
    case 0xFF30CF: Serial.println("Movement-ON- 1");
      //power_on = 1;
      break;
    case 0xFF18E7: Serial.println("Movement-OFF -2");
      //power_on = 0;
      break;
    case 0xFF10EF: Serial.println("Fan-on-3");
      fan_status = 1 ;
      break;
    case 0xFF38C7: Serial.println("Fan-off-4");
      fan_status = 0;
      break;
    default:
      Serial.println(" other button   ");
  }
  delay(500); // Do not get immediate repeat
} //END translateIR



int lookRight()
{
  myservo.write(50);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(115);
  return distance;
}

int lookLeft()
{
  myservo.write(170);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(115);
  return distance;
  delay(100);
}

int readPing() {
  delay(70);
  int cm = sonar.ping_cm();
  if (cm == 0)
  {
    cm = 250;
  }
  return cm;
}

void moveStop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  delay(100);
}

void moveForward() {
  if (!goesForward)
  {
    motor3.setSpeed(30);
    motor3.run(FORWARD);
    goesForward = true;
    motor1.run(FORWARD);
    motor2.run(FORWARD);

    motor1.setSpeed(100); //set the speed of the motors, between 0-255
    motor2.setSpeed(100);
    delay(100);
    motor1.setSpeed(55); //set the speed of the motors, between 0-255
    motor2.setSpeed(55);
  }
}

void moveBackward() {
  goesForward = false;
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);

  motor1.setSpeed(100); //set the speed of the motors, between 0-255
  motor2.setSpeed(100);
  delay(500);
}


void turnRight() {
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  delay(300);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}

void turnLeft() {
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  delay(300);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}


void startFan() {
  motor3.setSpeed(30);
  motor3.run(FORWARD);
  Serial.println("FAN start  ");
  delay(100);
}

void stopFan() {
  motor3.setSpeed(RELEASE);
  Serial.println("FAN  stop  ");
  delay(100);
}
