#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#define PIN_LM35 32
#define FOCO_CALENTAR 19
#define ENFRIAMIENTO 23

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {18, 33, 25, 26};
byte colPins[COLS] = {27, 14, 15, 4};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);
int HL = 35;
byte modoConfig = 0;
int valorTemporal = 0;
byte digitosIngresados = 0;

float temperatura = 25.0;
bool enfriando = false;

unsigned long tiempoLectura = 0;
unsigned long tiempoLCD = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
float leerLM35() {
  long sumaADC = 0;
  const int MUESTRAS = 15;

  for (int i = 0; i < MUESTRAS; i++) {
    sumaADC += analogRead(PIN_LM35);
    delayMicroseconds(50);
  }

  float adcPromedio = (float)sumaADC / MUESTRAS;
  float milivoltios = (adcPromedio / 4095.0) * 3300.0;

  return milivoltios / 10.0;
}

void iniciarConfiguracion(byte modo) {
  modoConfig = modo;
  valorTemporal = 0;
  digitosIngresados = 0;

  Serial.println(modo == 1 ?
    "Configurando LL" :
    "Configurando HL");
}
void guardarConfiguracion() {
  if (digitosIngresados == 0) {
    Serial.println("ERROR: No ingresaste ningun valor");
    return;
  }
  if (valorTemporal > 80) {
    Serial.println("ERROR: Maximo permitido 80 C");
    return;
  }
  if (modoConfig == 1) {
    if (valorTemporal < HL) {
      LL = valorTemporal;
      Serial.print("Nuevo LL: ");
      Serial.println(LL);
      modoConfig = 0;
    } 

    else {
      Serial.println("ERROR: LL debe ser menor que HL");
    }
  }

  else if (modoConfig == 2) {
    if (valorTemporal > LL) {
      HL = valorTemporal;
      Serial.print("Nuevo HL: ");
      Serial.println(HL);
      modoConfig = 0;
    } 
    else {
      Serial.println("ERROR: HL debe ser mayor que LL");
    }
  }
}
void setup() {

  Serial.begin(115200);

  pinMode(FOCO_CALENTAR, OUTPUT);
  pinMode(ENFRIAMIENTO, OUTPUT);

  digitalWrite(FOCO_CALENTAR, HIGH);
  digitalWrite(ENFRIAMIENTO, HIGH);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  analogReadResolution(12);

  Serial.println("Control de temperatura iniciado");
}
void loop() {

  char key = keypad.getKey();

  if (key != NO_KEY) {
    if (key == '*') {
      iniciarConfiguracion(1);
    }
    else if (key == 'B') {
      iniciarConfiguracion(2);
    }
    else if (key == 'C') {
      modoConfig = 0;
      Serial.println("Configuracion cancelada");
    }
    else if (key == 'D') {
      if (modoConfig != 0) {
        guardarConfiguracion();
      }
    }
    else if (key >= '0' && key <= '9' && modoConfig != 0) {
      if (digitosIngresados < 2) {
        int candidato = (valorTemporal * 10) + (key - '0');
        if (candidato <= 80) {
          valorTemporal = candidato;
          digitosIngresados++;

          Serial.print("Valor temporal: ");
          Serial.println(valorTemporal);

        }
        else {
          Serial.println("ERROR: Temperatura maxima 80 C");
        }
      }
    }
  }
  if (millis() - tiempoLectura >= 375) {
    tiempoLectura = millis();
    temperatura = leerLM35();
    if (temperatura >= HL) {
      enfriando = true;
    }
    else if (temperatura <= LL) {
      enfriando = false;
    }

    if (enfriando) {
      digitalWrite(FOCO_CALENTAR, HIGH);
      digitalWrite(ENFRIAMIENTO, LOW);
    }
    else {
      digitalWrite(ENFRIAMIENTO, HIGH);
      digitalWrite(FOCO_CALENTAR, LOW);
    }
  }
  if (millis() - tiempoLCD >= 150) {
    tiempoLCD = millis();
    char buffer[12];
    lcd.setCursor(0, 0);
    if (modoConfig == 1) {
      snprintf(buffer, sizeof(buffer), "LL=%02d_  ", valorTemporal);
    }
    else {
      snprintf(buffer, sizeof(buffer), "LL=%02dC  ", LL);
    }

    lcd.print(buffer);

    lcd.setCursor(8, 0);

    dtostrf(temperatura, 4, 1, buffer);

    lcd.print("T=");
    lcd.print(buffer);
    lcd.print("C ");

    lcd.setCursor(0, 1);

    if (modoConfig == 2) {
      snprintf(buffer, sizeof(buffer), "HL=%02d_  ", valorTemporal);
    }
    else {
      snprintf(buffer, sizeof(buffer), "HL=%02dC  ", HL);
    }

    lcd.print(buffer);

    lcd.setCursor(8, 1);

    if (enfriando) {
      lcd.print("[ENFR.] ");
    }
    else {
      lcd.print("[CALEN] ");
    }
  }
}
