#include <Wire.h>
#include <Zumo32U4.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4LineSensors.h>
#include <Zumo32U4ProximitySensors.h>

Zumo32U4ProximitySensors proxSensors;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;

#define SERIAL_COM Serial
#define REVERSE_SPEED 50
#define ROTATION_SPEED 100
#define BACKWARD_SPEED 150
#define DEFAULT_SPEED 140
#define STOP_SPEED 0
#define PROX_COOLDOWN 1500
#define PROX_THRESHOLD 5
#define DARKTHRESHOLD 280

static uint8_t mode = 0;
static uint16_t lineSensorValues[5] = {0, 0, 0, 0, 0};

bool objectSeenInFront = false;
bool objectSeenLeft = false;
bool objectSeenRight = false;

static bool objectCollisionDetection = false;

static uint8_t semiRight = 0;
static uint8_t semiLeft = 0;
static uint8_t autoRight = 0;
static uint8_t autoLeft = 0;

static uint16_t semiSampleTime = 0;
static uint16_t autoSampleTime = 0;

#include "TurnSensor.h"

void setup()
{
  SERIAL_COM.begin(9600);
  proxSensors.initThreeSensors();
  lineSensors.initThreeSensors();
  turnSensorSetup();
}

void loop()
{
  String cmd = "0";
  if (SERIAL_COM.available() > 0)
  {
    cmd = SERIAL_COM.readStringUntil('\n');
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
  lineSensors.read(lineSensorValues);

  switch (mode)
  {
  case 1:
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    manualControl(cmd);
    break;
  case 2:
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    semiAuto(cmd);
    break;
  case 3:
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    fullAuto(cmd);
    break;
  }
}

void manualControl(String cmd)
{
  proxCheck();
  if (cmd == "w")
  {
    if (objectSeenInFront && objectCollisionDetection)
    {
      SERIAL_COM.println("Object in Front can't go forward!");
      return;
    }
    if (aboveLine(1))
    {
      SERIAL_COM.println("Wall in front can't go forward!");
      return;
    }
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
    delay(250);
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  }
  else if (cmd == "s")
  {
    motors.setSpeeds(-DEFAULT_SPEED, -DEFAULT_SPEED);
    delay(200);
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  }
  else if (cmd == "a")
  {
    if (objectSeenLeft && objectCollisionDetection)
    {
      SERIAL_COM.println("ERROR Object to the left can't go left!");
      return;
    }
    if (aboveLine(0))
    {
      SERIAL_COM.println("ERROR Wall to the left can't go left!");
      return;
    }
    motors.setSpeeds(-ROTATION_SPEED, ROTATION_SPEED);
    delay(200);
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  }
  else if (cmd == "d")
  {
    if (objectSeenRight && objectCollisionDetection)
    {
      SERIAL_COM.println("ERROR Object to the right can't go right!");
      return;
    }
    if (aboveLine(2))
    {
      SERIAL_COM.println("ERROR Wall to the right can't go right!");
      return;
    }
    motors.setSpeeds(ROTATION_SPEED, -ROTATION_SPEED);
    delay(200);
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  }
  else if (cmd == "x")
  {
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    mode = 0;
  }
  else if (cmd == "o")
  {
    objectCollisionDetection = !objectCollisionDetection;
  }
  else if (cmd == "l")
  {
    turnLeft();
  }
  else if (cmd == "r")
  {
    turnRight();
  }
}

void semiAuto(String cmd)
{
  proxCheck();
  if (cmd == "x")
  {
    emergencyStop();
    return;
  }
  if (semiRight >= 20 && semiLeft >= 20)
  {
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    SERIAL_COM.println("Left and Right threshold hit, manual mode taken over");
    semiRight = 0;
    semiLeft = 0;
    mode = 1;
  }
  semiNavigate();
}

void fullAuto(String cmd)
{
  proxCheck();
  if (cmd == "x")
  {
    emergencyStop();
    return;
  }
  if (autoRight >= 4 && autoLeft >= 4)
  {
    motors.setSpeeds(ROTATION_SPEED, -ROTATION_SPEED);
    delay(1250);
    SERIAL_COM.print("Zumo Stuck Turning Right for 1250ms");
    autoRight = 0;
    autoLeft = 0;
    return;
  }
  autoNavigate();
}

void semiNavigate()
{
  if (aboveLine(1))
  {
    SERIAL_COM.println("middle sensor was hit, manual mode activate");
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    mode = 1;
  }
  else if (aboveLine(0))
  {
    semiRight++;
    SERIAL_COM.print("Semi-Auto move forward for: ");
    SERIAL_COM.print(millis() - semiSampleTime);
    SERIAL_COM.println("ms");
    semiSampleTime = millis();
    motors.setSpeeds(50, 50);
    delay(150);
    lineSensors.read(lineSensorValues);
    SERIAL_COM.println("Turning Right fior 150ms");
    turn(ROTATION_SPEED, -ROTATION_SPEED);
  }
  else if (aboveLine(2))
  {
    semiLeft++;
    SERIAL_COM.print("Semi-Auto move forward for: ");
    SERIAL_COM.print(millis() - semiSampleTime);
    SERIAL_COM.println("ms");
    semiSampleTime = millis();
    motors.setSpeeds(50, 50);
    delay(100);
    lineSensors.read(lineSensorValues);
    SERIAL_COM.println("Turning Left for 150ms");
    turn(-ROTATION_SPEED, ROTATION_SPEED);
  }
  else
  {
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
}

void turn(int direction1, int direction2)
{
  if (aboveLine(0) && aboveLine(2))
  {
    motors.setSpeeds(50, 50);
    delay(150);
    SERIAL_COM.println("Corner / Junction HIT, manual mode activate");
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    mode = 1;
  }
  else
  {
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(250);
    motors.setSpeeds(direction1, direction2);
    delay(150);
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
}

void autoNavigate()
{
  proxCheck();
  if (aboveLine(1))
  {
    SERIAL_COM.println("middle sensor was hit, manual mode activate");
    motors.setSpeeds(STOP_SPEED, STOP_SPEED);
    mode = 1;
  }
  else if (aboveLine(0))
  {
    autoRight++;
    SERIAL_COM.print("Auto move forward for: ");
    SERIAL_COM.print(millis() - autoSampleTime);
    SERIAL_COM.println("ms");
    autoSampleTime = millis();
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(250);
    SERIAL_COM.println("Turning Right for 150ms");
    motors.setSpeeds(ROTATION_SPEED, -ROTATION_SPEED);
    delay(150);
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
  else if (aboveLine(2))
  {
    autoLeft++;
    SERIAL_COM.print("Auto move forward for: ");
    SERIAL_COM.print(millis() - autoSampleTime);
    SERIAL_COM.println("ms");
    autoSampleTime = millis();
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(250);
    SERIAL_COM.println("Turning Left for 150 ms");
    motors.setSpeeds(-ROTATION_SPEED, ROTATION_SPEED);
    delay(150);
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
  else
  {
    motors.setSpeeds(DEFAULT_SPEED, DEFAULT_SPEED);
  }
}

void proxCheck()
{
  static uint16_t lastSampleTime = 0;
  if ((uint16_t)(millis() - lastSampleTime) >= PROX_COOLDOWN)
  {
    lastSampleTime = millis();
    proxSensors.read();

    uint8_t frontLeftProxSensor = proxSensors.countsFrontWithLeftLeds();
    uint8_t frontRightProxSensor = proxSensors.countsFrontWithRightLeds();

    uint8_t leftLeftLedProxSensor = proxSensors.countsLeftWithLeftLeds();
    uint8_t leftRightLed = proxSensors.countsLeftWithRightLeds();

    uint8_t rightLeftLedProxSensor = proxSensors.countsRightWithLeftLeds();
    uint8_t rightRightLedProxSensor = proxSensors.countsRightWithRightLeds();

    // Determine if an object is visible or not.
    objectSeenInFront = frontLeftProxSensor >= PROX_THRESHOLD || frontRightProxSensor >= PROX_THRESHOLD;
    objectSeenLeft = leftLeftLedProxSensor >= PROX_THRESHOLD || leftRightLed >= PROX_THRESHOLD;
    objectSeenRight = rightLeftLedProxSensor >= PROX_THRESHOLD || rightRightLedProxSensor >= PROX_THRESHOLD;

    if (objectSeenInFront)
    {
      SERIAL_COM.println("Object in Front");
    }
    else if (objectSeenLeft)
    {
      SERIAL_COM.println("Object to Left");
    }
    else if (objectSeenRight)
    {
      SERIAL_COM.println("Object to Right");
    }
  }
}

void emergencyStop()
{
  motors.setSpeeds(STOP_SPEED, STOP_SPEED);
  mode = 0;
}

bool aboveLine(uint8_t sensorIndex)
{
  return lineSensorValues[sensorIndex] > DARKTHRESHOLD;
}

void turnLeft()
{
  turnSensorReset();
  motors.setSpeeds(-ROTATION_SPEED, ROTATION_SPEED);
  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    turnSensorUpdate();
  }
  turnSensorReset();
  motors.setSpeeds(STOP_SPEED, STOP_SPEED);
}
void turnRight()
{
  turnSensorReset();
  motors.setSpeeds(ROTATION_SPEED, -ROTATION_SPEED);
  while ((int32_t)turnAngle > -turnAngle45 * 2)
  {
    turnSensorUpdate();
  }
  turnSensorReset();
  motors.setSpeeds(STOP_SPEED, STOP_SPEED);
}