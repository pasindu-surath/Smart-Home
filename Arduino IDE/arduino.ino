#include <Servo.h>

// -------------------------
// PIN DEFINITIONS
// -------------------------

#define RAIN_SENSOR_PIN 2

#define TRIG_PIN 7
#define ECHO_PIN 6

#define GREEN_LED 3
#define YELLOW_LED 4
#define RED_LED 5

#define SERVO_PIN 8

// -------------------------
// SERVO
// -------------------------

Servo clothesServo;

// Rack positions
int rackOutside = 0;
int rackInside = 180;

// Current rack position state
bool rackInsideState = false;

// -------------------------
// SETUP
// -------------------------

void setup() {

  Serial.begin(9600);

  // Rain sensor
  pinMode(RAIN_SENSOR_PIN, INPUT);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // LEDs
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Servo
  clothesServo.attach(SERVO_PIN);

  // Start with rack outside
  clothesServo.write(rackOutside);

  // Turn all LEDs off
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Serial.println("Smart Home System Started");
}


// -------------------------
// MAIN LOOP
// -------------------------

void loop() {

  // =========================
  // 1. RAIN SENSOR
  // =========================

  int rainState = digitalRead(RAIN_SENSOR_PIN);

  if (rainState == LOW) {

    // Rain detected
    Serial.println("Rain Detected!");

    if (rackInsideState == false) {

      // Move rack inside
      clothesServo.write(rackInside);

      rackInsideState = true;

      Serial.println("Clothes Rack Moving INSIDE");
    }

  } 
  else {

    // No rain
    Serial.println("No Rain");

    if (rackInsideState == true) {

      // Move rack outside
      clothesServo.write(rackOutside);

      rackInsideState = false;

      Serial.println("Clothes Rack Moving OUTSIDE");
    }
  }


  // =========================
  // 2. ULTRASONIC SENSOR
  // =========================

  long duration;
  float distance;

  // Send ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Read echo
  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Convert to centimeters
  if (duration == 0) {
    distance = 999;
  } 
  else {
    distance = duration * 0.0343 / 2;
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");


  // =========================
  // 3. PARKING ASSISTANT
  // =========================

  if (distance > 50) {

    // Vehicle far away
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);

  }

  else if (distance > 25 && distance <= 50) {

    // Vehicle medium distance
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);

  }

  else {

    // Vehicle very close
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);

  }

  delay(500);
}