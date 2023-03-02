// importing all the necessary libraries

#include <Wire.h>
#include <Zumo32U4.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4LineSensors.h>
#include <Zumo32U4ProximitySensors.h>

// Allows for interaction of parts of the robot
Zumo32U4ProximitySensors proxSensors;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;

// defining all the necessary global variables and constants
#define SERIAL_COM Serial
#define REVERSE_SPEED 50
#define ROTATION_SPEED 100
#define BACKWARD_SPEED 150
#define DEFAULT_SPEED 140
#define STOP_SPEED 0
#define PROX_COOLDOWN 1500
#define PROX_THRESHOLD 5
#define DARKTHRESHOLD 280

static uint8_t mode = 0;                               // 0 = off, 1 = manual, 2 = semi-auto, 3 = auto
static uint16_t lineSensorValues[5] = {0, 0, 0, 0, 0}; // store line sensor values

bool objectSeenInFront = false;                        // store if object is seen in front
bool objectSeenLeft = false;                           // store if object is seen to the left
bool objectSeenRight = false;                          // store if object is seen to the right

static bool objectCollisionDetection = false;          // store if object collision detection is on or off

static uint8_t semiRight = 0;                          // store the number of times the right sensor has seen a line in semi-auto mode
static uint8_t semiLeft = 0;                           // store the number of times the left sensor has seen a line in semi-auto mode
static uint8_t autoRight = 0;                          // store the number of times the right sensor has seen a line in auto mode
static uint8_t autoLeft = 0;                           // store the number of times the left sensor has seen a line in auto mode


// These variables are used to calulate how long the robot moves forward for
static uint16_t semiSampleTime = 0;                    // store the time of the last sample in semi-auto mode
static uint16_t autoSampleTime = 0;                    // store the time of the last sample in auto mode

// Import necessary functions for turning 90 degrees
#include "TurnSensor.h"


void setup()
{
  SERIAL_COM.begin(9600);
  proxSensors.initThreeSensors();
  lineSensors.initThreeSensors();
  turnSensorSetup(); // setup the gyro sensor for turning
}

void loop()
{
  String cmd = "0"; // create cmd command to stop robot from crashing
  if (SERIAL_COM.available() > 0) // if there is a command in the serial monitor
  {
    cmd = SERIAL_COM.readStringUntil('\n'); // read the command

    // Check if the command is valid
    if (cmd == "manual")
    {
      // Manual
      mode = 1;
    }
    else if (cmd == "semi")
    {
      // Semi-Auto
      mode = 2;
    }
    else if (cmd == "auto")
    {
      // Full-Auto
      mode = 3;
    }
  }
  // Read the line sensors
  lineSensors.read(lineSensorValues);

  // Run the appropriate mode
  switch (mode)
  {
  case 1:
    // Manual
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    manualControl(cmd);
    break;
  case 2:
    // Semi-Auto
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    semiAuto(cmd);
    break;
  case 3:
    // Full-Auto
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    fullAuto(cmd);
    break;
  }
}

void manualControl(String cmd)
{
  proxCheck(); // check if there is an object in front, left or right of the robot
  if (cmd == "w")
  {
    if (objectSeenInFront && objectCollisionDetection) // if there is an object in front of the robot and object collision detection is on
    {
      SERIAL_COM.println("Object in Front can't go forward!");
      return;
    }
    if (aboveLine(1)) // if there is a wall in front of the robot
    {
      SERIAL_COM.println("Wall in front can't go forward!");
      return;
    }
    // move forward for 250ms
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
    delay(250);
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  }
  else if (cmd == "s")
  {
    // move backwards for 250ms
    motors.setSpeeds(-DEFAULT_SPEED, -DEFAULT_SPEED);
    delay(200);
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  }
  else if (cmd == "a")
  {
    if (objectSeenLeft && objectCollisionDetection) // if there is an object to the left of the robot and object collision detection is on
    {
      SERIAL_COM.println("ERROR Object to the left can't go left!");
      return;
    }
    if (aboveLine(0)) // if there is a wall to the left of the robot
    {
      SERIAL_COM.println("ERROR Wall to the left can't go left!");
      return;
    }
    // turn left for 200ms
    motors.setSpeeds(-ROTATION_SPEED, ROTATION_SPEED);
    delay(200);
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  }
  else if (cmd == "d")
  {
    if (objectSeenRight && objectCollisionDetection) // if there is an object to the right of the robot and object collision detection is on
    {
      SERIAL_COM.println("ERROR Object to the right can't go right!");
      return;
    }
    if (aboveLine(2)) // if there is a wall to the right of the robot
    {
      SERIAL_COM.println("ERROR Wall to the right can't go right!");
      return;
    }
    // turn right for 200ms
    motors.setSpeeds(ROTATION_SPEED, -ROTATION_SPEED);
    delay(200);
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  }
  else if (cmd == "x")
  {
    // stop the robot
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    mode = 0;
  }
  else if (cmd == "o")
  {
    // toggle object collision detection
    objectCollisionDetection = !objectCollisionDetection;
  }
  else if (cmd == "l")
  {
    // turn left 90 degrees
    turnLeft();
  }
  else if (cmd == "r")
  {
    // turn right 90 degrees
    turnRight();
  }
}

void semiAuto(String cmd)
{
  proxCheck(); // check if there is an object in front, left or right of the robot
  if (cmd == "x")
  {
    // stop the robot
    emergencyStop();
    return;
  }
  if (semiRight >= 20 && semiLeft >= 20) // if the robot has seen a wall on both sides for 20 times
  {
    // stop the robot convert to manual mode and take over
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    SERIAL_COM.println("Left and Right threshold hit, manual mode taken over");
    semiRight = 0;
    semiLeft = 0;
    mode = 1;
  }
  // navigate the robot
  semiNavigate();
}

void fullAuto(String cmd)
{
  proxCheck(); // check if there is an object in front, left or right of the robot
  if (cmd == "x")
  {
    // stop the robot
    emergencyStop();
    return;
  }
  if (autoRight >= 4 && autoLeft >= 4) // if the robot has seen a wall on both sides for 4 times
  {
    // Rotate the robot right for 1250ms
    motors.setSpeeds(ROTATION_SPEED, -ROTATION_SPEED);
    delay(1250);
    SERIAL_COM.print("Zumo Stuck Turning Right for 1250ms");
    autoRight = 0;
    autoLeft = 0;
    return;
  }
  // navigate the robot
  autoNavigate();
}

void semiNavigate()
{
  if (aboveLine(1)) // if there is a wall in front of the robot
  {
    // stop the robot convert to manual mode and take over
    SERIAL_COM.println("middle sensor was hit, manual mode activate");
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    mode = 1;
  }
  else if (aboveLine(0)) // if there is a wall on the left
  {
    semiRight++; // add 1 to the semiRight counter
    // print move forward time and move forward for 150ms
    SERIAL_COM.print("Semi-Auto move forward for: ");
    SERIAL_COM.print(millis() - semiSampleTime);
    SERIAL_COM.println("ms");
    semiSampleTime = millis();
    motors.setSpeeds(50, 50);
    delay(150);
    lineSensors.read(lineSensorValues); // read the line sensors
    // turn right for 150ms
    SERIAL_COM.println("Turning Right for 150ms");
    turn(ROTATION_SPEED, -ROTATION_SPEED);
  }
  else if (aboveLine(2)) // if there is a wall on the right
  {
    semiLeft++; // add 1 to the semiLeft counter
    // print move forward time and move forward for 150ms
    SERIAL_COM.print("Semi-Auto move forward for: ");
    SERIAL_COM.print(millis() - semiSampleTime);
    SERIAL_COM.println("ms");
    semiSampleTime = millis();
    motors.setSpeeds(50, 50);
    delay(100);
    lineSensors.read(lineSensorValues); // read the line sensors
    // turn left for 150ms
    SERIAL_COM.println("Turning Left for 150ms");
    turn(-ROTATION_SPEED, ROTATION_SPEED);
  }
  else // move forward
  {
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
}

void turn(int direction1, int direction2)
{
  if (aboveLine(0) && aboveLine(2)) // if there is a wall on both sides
  {
    // go forward for 150ms and convert to manual mode
    motors.setSpeeds(50, 50);
    delay(150);
    SERIAL_COM.println("Corner / Junction HIT, manual mode activate");
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    mode = 1;
  }
  else
  {
    // reverse for 250ms
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(250);
    // turn in specific direction for 150ms
    motors.setSpeeds(direction1, direction2);
    delay(150);
    // set speed to go forward
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
}

void autoNavigate()
{
  proxCheck(); // check if there is an object in front, left or right of the robot
  if (aboveLine(1)) // if there is a line in the middle
  {
    // stop the robot convert to manual mode and take over
    SERIAL_COM.println("middle sensor was hit, manual mode activate");
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    mode = 1;
  }
  else if (aboveLine(0)) // if there is a line on the left
  {
    autoRight++; // add 1 to the autoRight counter
    // print move forward time and move forward for 150ms
    SERIAL_COM.print("Auto move forward for: ");
    SERIAL_COM.print(millis() - autoSampleTime);
    SERIAL_COM.println("ms");
    autoSampleTime = millis();
    // reverse for 250ms
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(250);
    // turn right for 150ms and print to frontend
    SERIAL_COM.println("Turning Right for 150ms");
    motors.setSpeeds(ROTATION_SPEED, -ROTATION_SPEED);
    delay(150);
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
  else if (aboveLine(2)) // if there is a line on the right
  {
    autoLeft++; // add 1 to the autoLeft counter
    // print move forward time and move forward for 150ms
    SERIAL_COM.print("Auto move forward for: ");
    SERIAL_COM.print(millis() - autoSampleTime);
    SERIAL_COM.println("ms");
    autoSampleTime = millis();
    // reverse for 250ms
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(250);
    // turn left for 150ms and print to frontend
    SERIAL_COM.println("Turning Left for 150 ms");
    motors.setSpeeds(-ROTATION_SPEED, ROTATION_SPEED);
    delay(150);
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
  else
  {
    // move forward
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
}

void proxCheck()
{
  static uint16_t lastSampleTime = 0; // last time the proximity sensors were checked
  if ((uint16_t)(millis() - lastSampleTime) >= PROX_COOLDOWN) // if the cooldown time has passed
  {
    lastSampleTime = millis(); // set the last sample time to the current time
    proxSensors.read(); // read the proximity sensors

    // Get all the values from all the proximity sensors
    uint8_t frontLeftProxSensor = proxSensors.countsFrontWithLeftLeds();
    uint8_t frontRightProxSensor = proxSensors.countsFrontWithRightLeds();

    uint8_t leftLeftLedProxSensor = proxSensors.countsLeftWithLeftLeds();
    uint8_t leftRightLed = proxSensors.countsLeftWithRightLeds();

    uint8_t rightLeftLedProxSensor = proxSensors.countsRightWithLeftLeds();
    uint8_t rightRightLedProxSensor = proxSensors.countsRightWithRightLeds();

    // Determine if an object is visible or not by checking value against the threshold.
    objectSeenInFront = frontLeftProxSensor >= PROX_THRESHOLD || frontRightProxSensor >= PROX_THRESHOLD;
    objectSeenLeft = leftLeftLedProxSensor >= PROX_THRESHOLD || leftRightLed >= PROX_THRESHOLD;
    objectSeenRight = rightLeftLedProxSensor >= PROX_THRESHOLD || rightRightLedProxSensor >= PROX_THRESHOLD;

    if (objectSeenInFront) // if an object is in front of the robot
    {
      SERIAL_COM.println("Object in Front");
    }
    else if (objectSeenLeft) // if an object is to the left of the robot
    {
      SERIAL_COM.println("Object to Left");
    }
    else if (objectSeenRight) // if an object is to the right of the robot
    {
      SERIAL_COM.println("Object to Right");
    }
  }
}

void emergencyStop() // stop the robot entirely
{
  motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  mode = 0;
}

bool aboveLine(uint8_t sensorIndex) // check if the sensor at index position sensorIndex is above the line
{
  return lineSensorValues[sensorIndex] > DARKTHRESHOLD;
}

void turnLeft() // turn the robot left 90 degrees
{
  turnSensorReset(); // reset the turn sensor
  motors.setSpeeds(-ROTATION_SPEED, ROTATION_SPEED); // set the motors to turn left
  while ((int32_t)turnAngle < turnAngle45 * 2) // while the turn angle is less than 90 degrees
  {
    turnSensorUpdate(); // update the turn sensor
  }
  turnSensorReset(); // reset the turn sensor
  motors.setSpeeds(STOP_SPEED, STOP_SPEED); // stop the motors
}
void turnRight() // turn the robot right 90 degrees
{
  turnSensorReset(); // reset the turn sensor
  motors.setSpeeds(ROTATION_SPEED, -ROTATION_SPEED); // set the motors to turn right
  while ((int32_t)turnAngle > -turnAngle45 * 2) // while the turn angle is greater than -90 degrees
  {
    turnSensorUpdate(); // update the turn sensor
  }
  turnSensorReset(); // reset the turn sensor
  motors.setSpeeds(STOP_SPEED, STOP_SPEED); // stop the motors
}