#include <LiquidCrystal.h>
#include <Servo.h>

// Function prototyping
void servoControl(int *readings);
void setLEDColor(int tempC);
int fadeTo(int current, int target);

// Temp
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
int cel;

// RGB
int currentRed = 0, currentGreen = 0, currentBlue = 0;

// Servo
Servo myServo;
int LDRValue = 0, LDRAfter = 0, lightReadings = 20, lux = 0;

// Pointers
int *ptrLux = &lux;
int *ptrLightReading = &lightReadings;

// Pins used
#define THERMISTOR_PIN A0
#define LDR A4
#define BRIGHTNESS_PIN A5
#define RED_LIGHT_PIN 6
#define GREEN_LIGHT_PIN 5
#define BLUE_LIGHT_PIN 3
#define RS 12
#define EN 7
#define D4 8
#define D5 4
#define D6 11
#define D7 2

// Other defines
#define FADE_SPEED 100

// LCD
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup() {
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BLUE_LIGHT_PIN, OUTPUT);
  pinMode(THERMISTOR_PIN, INPUT);
  myServo.attach(9);
  pinMode(LDR, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
}

void loop() {

  // Temp beregninger
  Vo = analogRead(THERMISTOR_PIN);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  T = (T * 9.0)/ 5.0 + 32.0; 
  cel = (int)((T-30)/2);

  // Servo
  servoControl(&lightReadings);
  lightReadings++;
  
  // LCD
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Light: ");lcd.print(constrain(lux, 0, 880));lcd.print(" LUX");

  lcd.setCursor(0, 1);
  lcd.print("TEMP: ");lcd.print(cel);lcd.print((char)0b11011111);lcd.print("C");

  // LED RGB
  setLEDColor(cel);

  delay(300);
}

void servoControl(int *readings) {
  if (*readings == 20) {
    LDRValue = analogRead(LDR);
    LDRAfter = map(LDRValue, 0, 1023, 0, 180);
    LDRAfter = constrain(LDRAfter, 0, 180);

    *ptrLightReading = 0;

    *ptrLux = map(LDRValue, 0, 1023, 0, 880);

    myServo.write(LDRAfter);
  }
}

void setLEDColor(int tempC) {
  int targetRed, targetGreen, targetBlue;

  if (tempC < 25) {
    targetRed = 0;
    targetGreen = 0;
    targetBlue = 255;
  } else if (tempC > 28) {
    targetRed = 255;
    targetGreen = 0;
    targetBlue = 0;
  } else {
    targetRed = 0;
    targetGreen = 255;
    targetBlue = 0;
  }

  currentRed = fadeTo(currentRed, targetRed);
  currentGreen = fadeTo(currentGreen, targetGreen);
  currentBlue = fadeTo(currentBlue, targetBlue);

  int brightness = analogRead(BRIGHTNESS_PIN) / 4;

  analogWrite(RED_LIGHT_PIN, (currentRed * brightness) / 255);
  analogWrite(GREEN_LIGHT_PIN, (currentGreen * brightness) / 255);
  analogWrite(BLUE_LIGHT_PIN, (currentBlue * brightness) / 255);
}

int fadeTo(int current, int target) {
  if (current < target) {
    current += FADE_SPEED;
    if (current > target) current = target;
  } else if (current > target) {
    current -= FADE_SPEED;
    if (current < target) current = target;
  }
  return current;
}
