/*
 * Scooter Robot
 * An Artificial Intelligence Project (CAP5636)
 * by Kyle Burnett and Brian Woods
 */

#include <Servo.h>

const int pingPin = 7;
const int armServoPin = 9;
const int handServoPin = 10;

Servo armServo, handServo;
const float epsilon = 0.05,
            alpha = 0.2,
            discount = 0.9;
float values[8][8][4];
long pos;

void setup()
{
  // Initialize servos
  armServo.attach(armServoPin);
  delay(100);
  handServo.attach(handServoPin);
  delay(100);
  // Initialize values array
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      for (int k = 0; k < 4; k++)
      {
        values[i][j][k] = 0;
      }
    }
  }
  // Initialize position
  pos = getPos();
}

void loop()
{
  performAction();
  float reward = getReward();
  updateValues();
}

void performAction()
{
}

float getReward()
{
  return 0.0;
}

void updateValues()
{
}

long getPos()
{
  long duration;
  
  // Trigger PING
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  
  // Read signal
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  
  return microsecondsToCentimeters(duration);
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
