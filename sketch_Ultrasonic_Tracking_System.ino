#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP32Servo.h>

// --- Pin configuration ---
#define servoPin 13
#define trigPin 14
#define echoPin 12
#define SDAPin 4
#define SCLPin 2
#define buzzerPin 15

#define MAX_DISTANCE 700

// --- State machine ---
enum State { SCAN, TRACK, LOST };

// --- Globals ---
float timeOut = MAX_DISTANCE * 60;
int scanDirection = 1;
int currentAngle = 0;
State mode = SCAN;
unsigned long lastDetectionTime = 0;
unsigned long lastScanTime = 0;

// --- Constants ---
const int SCAN_STEP = 1;
const int SCAN_DELAY = 5;
const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;
const int DETECTION_THRESHOLD = 30;
const int LOST_TIMEOUT = 500;
const int BAUD_RATE = 115200;

LiquidCrystal_I2C lcd(0x27,16,2);
Servo myServo;

// =================================================================================
// ULTRASONIC READ FUNCTION
// =================================================================================
long getDistanceCM() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 25000); // timeout for stability
    long distance = duration * 0.034 / 2;

    return distance > 400 ? 400 : distance;  // cap to 4m
}

// =========================================================
// SERVO ROTATION
// =========================================================
void servoTrackSmooth(int targetAngle, int step = 2) {
  if (currentAngle < targetAngle) {
      currentAngle += step;
      if (currentAngle > targetAngle) currentAngle = targetAngle;
  } else if (currentAngle > targetAngle) {
      currentAngle -= step;
      if (currentAngle < targetAngle) currentAngle = targetAngle;
  }
  myServo.write(currentAngle);
}

// =========================================================
// CHECK I2C CONNECTION
// =========================================================
bool i2CAddrTest(uint8_t addr) {
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  Serial.begin(BAUD_RATE);
  Wire.begin(SDAPin, SCLPin);
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Scanning...");
  // myServo.setPeriodHertz(SCAN_DELAY);
  myServo.attach(servoPin);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (mode) {
    case SCAN: {
      if (millis() - lastScanTime > SCAN_DELAY) {
        lastScanTime = millis();
        currentAngle += SCAN_STEP * scanDirection;
        if (currentAngle >= MAX_ANGLE) {
          currentAngle = MAX_ANGLE;
          scanDirection = -1;
        }
        if (currentAngle <= MIN_ANGLE) {
          currentAngle = MIN_ANGLE;
          scanDirection = 1;
        }

        myServo.write(currentAngle);

        long distance = getDistanceCM();
        
        // LCD Update
        lcd.setCursor(0, 0);
        lcd.print("Scanning...");
        // lcd.print(currentAngle < 100 ? " " : "");
        // lcd.print(currentAngle);
        lcd.setCursor(0, 1);
        lcd.print("Dist:");
        lcd.print(distance);
        lcd.print("cm");

        // If something detected close
        if (distance < DETECTION_THRESHOLD) {
          lastDetectionTime = millis();
          mode = TRACK;
          lcd.init();
          lcd.backlight();
          lcd.clear();
          digitalWrite(buzzerPin, HIGH);
          break;
        }
        digitalWrite(buzzerPin, LOW);
      }
    } break;
    case TRACK: {
      long distance = getDistanceCM();
      if (distance >= DETECTION_THRESHOLD) {
        mode = LOST;
        lcd.init();
        lcd.backlight();
        lcd.clear();
        break;
      }

      lastDetectionTime = millis();

      // Tracking target angle = where we already are
      int targetAngle = currentAngle;
      servoTrackSmooth(targetAngle);
      // LCD
      lcd.setCursor(0, 0);
      lcd.print("Obstacle Detected!!!");
      lcd.setCursor(0, 1);
      lcd.print("Dist:");
      lcd.print(distance);
      lcd.print("cm");
    } break;
    case LOST: {
      lcd.setCursor(0, 0);
      lcd.print("Obstacle out of view");
      lcd.setCursor(0, 1);
      lcd.print("Re-scanning...");

      if (millis() - lastDetectionTime > LOST_TIMEOUT) {
        mode = SCAN;
        lcd.init();
        lcd.backlight();
        lcd.clear();
        digitalWrite(buzzerPin, LOW);
      }
    } break;
  }
}
