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
int lastAction;
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
  updateValues(reward);
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
  float bestValue = -1.0;
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
  // If we are at the last arm postion, can't go up
  if (armPos == 7)
  {
    actions[0] = false;
    actions[1] = false;
  }
  // If we are at the last hand position, can't go up
  if (handPos == 7)
  {
    actions[0] = false;
    actions[2] = false;
  }
  // If we are at the first arm position, can't go down
  if (armPos == 0)
  {
    actions[2] = false;
    actions[3] = false;
  }
  // If we are at the last hand position, can't go up
  if (handPos == 0)
  {
    actions[1] = false;
    actions[3] = false;
  }
  // If we are at arm position 1, can't go down unless hand is at 1
  if (armPos == 1 && handPos != 1)
  {
    actions[2] = false;
    actions[3] = false;
  }
  return actions;
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
doAction() takes an action and changes the armPos and handPos indices
accordingly.
 */
void doAction(int action)
{
  if (action == 0 || action == 1)
  {
    armPos++;
  }
  else
  {
    armPos--;
  }
  if (action == 0 || action == 2)
  {
    handPos++;
  }
  else
  {
    handPos--;
  }
}

void performAction()
{
  // Get legal actions
  // With probability epsilon, choose action randomly
  // Otherwise, act according to best action
  randomSeed(analogRead(0));
  
  long coinflip = random(100);
  
  if (coinflip < (epsilon*100))
  {
    // Random Legal Action
    boolean check = false;
    boolean * legalActions = getMovesLegality();
    
    while(!check)
    {
      int randLegal = random(5);
      
      if ( legalActions[randLegal] )
      {
        check = true;
        doAction(randLegal);
        lastAction = randLegal;
      }
    }
    
  }
  else
  {
    // Use Best Action Policy to get best legal movement schemeand execute it
    int action = getBestAction();
    
    doAction(action);
    lastAction = action;
  }
  
  moveArm();
  moveHand();
}

float getReward()
{
  // Retrieve reward values depending on the change in
  // position to either closer or farther from the goal.
  long newPos = getPos();
  
  // Closer to goal.
  if (newPos < pos)
  {
    pos = newPos;
    return 1.0;
  }
  // Farther from goal
  else if (newPos > pos)
  {
    pos = newPos;
    return -1.0;
  }
  // No position change
  else
  {
    pos = newPos;
    return 0.0;
  }
}

float getQValue(int action)
{
  int nextArm, nextHand;
  if (action == 0 || action == 1)
  {
    nextArm = armPos + 1;
  }
  else
  {
    nextArm = armPos - 1;
  }
  if (action == 0 || action == 2)
  {
    nextHand = handPos + 1;
  }
  else
  {
    nextHand = handPos - 1;
  }
  return values[nextArm][nextHand][action];
}

void updateValues(float reward)
{
  boolean * legalActions = getMovesLegality();
  float maxValue = -1.0;
  for (int i = 0; i < 4; ++i)
  {
    if (legalActions[i])
    {
      float value = getQValue(i);
      if (value > maxValue)
      {
        maxValue = value;
      }
    }
  }
  
  // sample = R(s,a,s') + gamma * max_a'{Q(s',a')}
  float sample = reward + discount * maxValue;
  
  // Q(s,a) <- (1-alpha) * Q(s,a) + alpha * sample
  float prevValue = values[armPos][handPos][lastAction];
  values[armPos][handPos][lastAction] = (1 - alpha) * prevValue + alpha * sample;
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
