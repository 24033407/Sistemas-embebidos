#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_DS18B20 4 

#define FOCO_CALENTAR 19  
#define ENFRIAMIENTO  23  

#define POT_LL 25
#define POT_HL 26

int HL = 35; 
float ema_LL = 0;
float ema_HL = 0;
const float alpha = 0.1;

LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);

bool enfriando = false; 
unsigned long tiempoLectura = 0;
unsigned long tiempoLCD     = 0;
float temperatura = 25.0; 

void setup() {
  pinMode(FOCO_CALENTAR, OUTPUT);
  pinMode(ENFRIAMIENTO, OUTPUT);

  digitalWrite(FOCO_CALENTAR, HIGH); 
  digitalWrite(ENFRIAMIENTO, HIGH);  

  sensors.begin();
  sensors.setResolution(11);            
  sensors.setWaitForConversion(false);  
  sensors.requestTemperatures();       

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  ema_LL = analogRead(POT_LL);
  ema_HL = analogRead(POT_HL);
}

void loop() {
  ema_LL = (alpha * analogRead(POT_LL)) + ((1.0 - alpha) * ema_LL);
  ema_HL = (alpha * analogRead(POT_HL)) + ((1.0 - alpha) * ema_HL);

  LL = map((int)ema_LL, 0, 4095, 0, 60);
  HL = map((int)ema_HL, 0, 4095, 0, 60);

  if (LL >= HL) {
    LL = HL - 1; 
    if (LL < 0) {
      LL = 0;
      HL = 1;
    }
  }

  if (millis() - tiempoLectura >= 375) {
    tiempoLectura = millis();

    float tempLeida = sensors.getTempCByIndex(0);
    if (tempLeida != DEVICE_DISCONNECTED_C && tempLeida > -50.0) {
      temperatura = tempLeida;
    }
    sensors.requestTemperatures();
    if (temperatura >= HL) {
      enfriando = true;  
    } else if (temperatura <= LL) {
      enfriando = false; 
    }
    if (enfriando) {
      digitalWrite(FOCO_CALENTAR, HIGH); 
      digitalWrite(ENFRIAMIENTO, LOW);   
    } else {
      digitalWrite(ENFRIAMIENTO, HIGH);  
      digitalWrite(FOCO_CALENTAR, LOW);  
    }
  }

  if (millis() - tiempoLCD >= 150) {
    tiempoLCD = millis();
    char buffer[10];
    lcd.setCursor(0, 0);
    sprintf(buffer, "LL=%-2dC ", LL);
    lcd.print(buffer);

    lcd.setCursor(8, 0);
    dtostrf(temperatura, 4, 1, buffer); 
    lcd.print("T=");
    lcd.print(buffer);
    lcd.print("C ");
    lcd.setCursor(0, 1);
    sprintf(buffer, "HL=%-2dC ", HL);
    lcd.print(buffer);

    lcd.setCursor(8, 1);
    if (enfriando) {
      lcd.print("[ENFR.] ");
    } else {
      lcd.print("[CALEN] ");
    }
  }
}
