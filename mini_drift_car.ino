#include <Ps3Controller.h>

// =====================================================
// ESP32 MINI DRIFT CAR
// PS3 + L298N
// =====================================================

// -------------------------
// Rear traction motor
// -------------------------
#define ENA 25
#define IN1 26
#define IN2 27

// -------------------------
// N20 steering motor
// -------------------------
#define ENB 32
#define IN3 33
#define IN4 14

// -------------------------
// Mode LEDs
// -------------------------
#define GREEN_LED 4
#define RED_LED 5

// -------------------------
// Speeds
// -------------------------
#define NORMAL_SPEED 180
#define DRIFT_SPEED 255
#define STEERING_SPEED 150

bool driftMode = false;

// =====================================================
// TRACTION MOTOR
// speed: -255 to +255
// =====================================================

void tractionMotor(int speed)
{
  speed = constrain(speed, -255, 255);

  if (speed > 0)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  }
  else if (speed < 0)
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -speed);
  }
  else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

// =====================================================
// STEERING MOTOR
// speed: -255 to +255
// =====================================================

void steeringMotor(int speed)
{
  speed = constrain(speed, -255, 255);

  if (speed > 0)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
  }
  else if (speed < 0)
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, -speed);
  }
  else
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
}

// =====================================================
// STOP
// =====================================================

void stopMotors()
{
  tractionMotor(0);
  steeringMotor(0);
}

// =====================================================
// PS3 CONNECT CALLBACK
// =====================================================

void onConnect()
{
  Serial.println("PS3 CONNECTED!");
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Start in Normal mode.
  driftMode = false;
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);

  stopMotors();

  Ps3.attachOnConnect(onConnect);

  // ESP32 Bluetooth MAC
  Ps3.begin("3C:71:BF:C5:D0:5C");

  Serial.println("Waiting for PS3 controller...");
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  // Safety: stop if controller is disconnected.
  if (!Ps3.isConnected())
  {
    stopMotors();

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);

    delay(50);
    return;
  }

  // -------------------------
  // X = NORMAL MODE
  // -------------------------
  if (Ps3.data.button.cross)
  {
    driftMode = false;

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);

    Serial.println("NORMAL MODE");
  }

  // -------------------------
  // TRIANGLE = DRIFT MODE
  // -------------------------
  if (Ps3.data.button.triangle)
  {
    driftMode = true;

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    Serial.println("DRIFT MODE");
  }

  // -------------------------
  // R2 = FORWARD
  // L2 = REVERSE
  // -------------------------

  int maxSpeed = driftMode ? DRIFT_SPEED : NORMAL_SPEED;
  int motorSpeed = 0;

  int r2 = Ps3.data.analog.button.r2;
  int l2 = Ps3.data.analog.button.l2;

  if (r2 > 5)
  {
    motorSpeed = map(r2, 0, 255, 0, maxSpeed);
  }
  else if (l2 > 5)
  {
    motorSpeed = -map(l2, 0, 255, 0, maxSpeed);
  }

  tractionMotor(motorSpeed);

  // -------------------------
  // RIGHT JOYSTICK X
  //
  // Inverted steering:
  // RIGHT -> LEFT
  // LEFT  -> RIGHT
  // -------------------------

  int steer = Ps3.data.analog.stick.rx;

  if (abs(steer) < 15)
  {
    steeringMotor(0);
  }
  else if (steer > 15)
  {
    int steeringSpeed = map(
      steer,
      15,
      127,
      0,
      STEERING_SPEED
    );

    steeringMotor(-steeringSpeed);
  }
  else if (steer < -15)
  {
    int steeringSpeed = map(
      steer,
      -127,
      -15,
      STEERING_SPEED,
      0
    );

    steeringMotor(steeringSpeed);
  }

  delay(10);
}
