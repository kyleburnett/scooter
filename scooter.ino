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
/*
index |  armValue  |  handValue
------+------------+-----------
  0   |  21        |  93
  1   |  26        |  98
  2   |  31        |  103
  3   |  36        |  108
  4   |  41        |  113
  5   |  46        |  118
  6   |  51        |  123
  7   |  56        |  128
-------------------------------
 */
int armPos = 3, handPos = 3;

void setup()
{
  // Initialize servos
  armServo.attach(armServoPin);
  delay(100);
  moveArm();
  handServo.attach(handServoPin);
  delay(100);
  moveHand();
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

void moveArm()
{
  int value = armPos * 5 + 21;
  armServo.write(value);
}

void moveHand()
{
  int value = handPos * 5 + 93;
  handServo.write(value);
}

/*
Loops through the list of actions for the current arm and hand positions
and returns the best action according to the results stored in the q-value
array.
 */
int getBestAction()
{
  int best = 0;
  float bestValue = 0;
  for (int i = 0; i < 4; ++i)
  {
    if (values[armPos][handPos][i] > bestValue)
    {
      bestValue = values[armPos][handPos][i];
      best = i;
    }
  }
  return best;
}

/*
Legal moves are defined as indexed values 0 through 3 as follows
index |  meaning
------+----------------------
  0   |  arm up; hand up
  1   |  arm up; hand down
  2   |  arm down; hand up
  3   |  arm down; hand down
-----------------------------
getMovesLegality() returns a 4-element boolean array where each element
corresponds to the legality of the i-th move
 */
boolean * getMovesLegality()
{
  boolean actions[4] = {true, true, true, true};
  return actions;
}

void performAction()
{
  // Get legal actions
  // With probability epsilon, choose action randomly
  // Otherwise, act according to best action
  moveArm();
  moveHand();
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
