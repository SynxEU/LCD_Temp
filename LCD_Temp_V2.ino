#include <LiquidCrystal.h>

int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
int cel;
int currentRed = 0;
int currentGreen = 0;
int currentBlue = 0;

#define FADE_SPEED 100
#define RED_LIGHT_PIN 6
#define GREEN_LIGHT_PIN 9
#define BLUE_LIGHT_PIN 10
#define THERMISTOR_PIN A0
#define BRIGHTNESS_PIN A5
#define RS 12
#define EN 11
#define D4 5
#define D5 4
#define D6 3
#define D7 2

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup() {
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BLUE_LIGHT_PIN, OUTPUT);
  pinMode(THERMISTOR_PIN, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
}

void loop() {

  Vo = analogRead(THERMISTOR_PIN);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  T = (T * 9.0)/ 5.0 + 32.0; 
  cel = (int)((T-30)/2);

  lcd.setCursor(0, 1);

  setLEDColor(cel);

  lcd.setCursor(0, 2);
  lcd.print("TEMP: ");lcd.print(cel);lcd.print((char)0b11011111);lcd.print("C");

  delay(300);
}


void setLEDColor(int tempC) {
  int targetRed, targetGreen, targetBlue;

  if (tempC < 25) {
    targetRed = 0;
    targetGreen = 0;
    targetBlue = 255;
    lcd.clear();
    lcd.print("Too Cold!");
  } else if (tempC > 28) {
    targetRed = 255;
    targetGreen = 0;
    targetBlue = 0;
    lcd.clear();
    lcd.print("Too Hot!");
  } else {
    targetRed = 0;
    targetGreen = 255;
    targetBlue = 0;
    lcd.clear();
    lcd.print("Perfect Temp");
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