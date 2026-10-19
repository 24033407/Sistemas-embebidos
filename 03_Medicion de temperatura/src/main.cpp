#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// =====================================================
// PINES DE ACTUADORES Y SENSOR
// =====================================================
#define PIN_DS18B20 4 // GPIO 4 (OneWire)

// Relés o Módulos de Control
#define FOCO_CALENTAR 19  // Relé para el Foco (Calentamiento)
#define ENFRIAMIENTO  23  // Relé para la Celda Peltier + Ventiladores

// Botones
#define BOTON_SUBIR  25
#define BOTON_BAJAR  26
#define BOTON_SELECT 27

// =====================================================
// LÍMITES INICIALES EN °C (Enteros 1 en 1)
// =====================================================
int LL = 20; // Límite Inferior
int HL = 35; // Límite Superior

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuración de bus OneWire y DallasTemperature
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);

// =====================================================
// ESTADOS Y TIEMPOS
// =====================================================
bool enfriando = false;  // false = Calentando, true = Enfriando
bool seleccionLL = true; // true = Modificando LL, false = Modificando HL

bool estadoAnteriorSubir  = HIGH;
bool estadoAnteriorBajar  = HIGH;
bool estadoAnteriorSelect = HIGH;

unsigned long tiempoParpadeo    = 0;
unsigned long tiempoLectura     = 0;
unsigned long tiempoLCD         = 0;
unsigned long ultimoTiempoBoton = 0;

bool mostrarSeleccion = true;
float temperatura = 25.0; // Valor inicial seguro

// =====================================================
// SETUP
// =====================================================
void setup() {
  // Salidas
  pinMode(FOCO_CALENTAR, OUTPUT);
  pinMode(ENFRIAMIENTO, OUTPUT);

  digitalWrite(FOCO_CALENTAR, HIGH); // HIGH = Relé APAGADO
  digitalWrite(ENFRIAMIENTO, HIGH);  // HIGH = Relé APAGADO

  // Botones
  pinMode(BOTON_SUBIR, INPUT_PULLUP);
  pinMode(BOTON_BAJAR, INPUT_PULLUP);
  pinMode(BOTON_SELECT, INPUT_PULLUP);

  // Inicializar DS18B20 en modo NO BLOQUEANTE a 11 bits (375ms de conversión)
  sensors.begin();
  sensors.setResolution(11);            // Sensibilidad de 0.125 °C
  sensors.setWaitForConversion(false);  // No congela el microcontrolador
  sensors.requestTemperatures();       // Primera petición en segundo plano

  // Inicializar bus I2C (SDA=21, SCL=22)
  Wire.begin(21, 22);

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

// =====================================================
// LOOP PRINCIPAL
// =====================================================
void loop() {

  // ---------------------------------------------------
  // 1. LECTURA Y LÓGICA DE BOTONES (De 1 en 1 °C)
  // ---------------------------------------------------
  bool estadoSubir  = digitalRead(BOTON_SUBIR);
  bool estadoBajar  = digitalRead(BOTON_BAJAR);
  bool estadoSelect = digitalRead(BOTON_SELECT);

  if (millis() - ultimoTiempoBoton >= 120) {
    // Botón SELECT
    if (estadoSelect == LOW && estadoAnteriorSelect == HIGH) {
      seleccionLL = !seleccionLL;
      mostrarSeleccion = true;
      tiempoParpadeo = millis();
      ultimoTiempoBoton = millis();
    }
    // Botón SUBIR
    else if (estadoSubir == LOW && estadoAnteriorSubir == HIGH) {
      if (seleccionLL) {
        if (LL < HL - 1) LL++;
      } else {
        if (HL < 100) HL++;
      }
      ultimoTiempoBoton = millis();
    }
    // Botón BAJAR
    else if (estadoBajar == LOW && estadoAnteriorBajar == HIGH) {
      if (seleccionLL) {
        if (LL > 0) LL--;
      } else {
        if (HL > LL + 1) HL--;
      }
      ultimoTiempoBoton = millis();
    }
  }

  estadoAnteriorSubir  = estadoSubir;
  estadoAnteriorBajar  = estadoBajar;
  estadoAnteriorSelect = estadoSelect;

  // ---------------------------------------------------
  // 2. TEMPORIZADOR PARA PARPADEO EN PANTALLA
  // ---------------------------------------------------
  if (millis() - tiempoParpadeo >= 400) {
    tiempoParpadeo = millis();
    mostrarSeleccion = !mostrarSeleccion;
  }

  // ---------------------------------------------------
  // 3. CONTROL DE HISTÉRESIS Y LECTURA DS18B20 (Cada 375 ms)
  // ---------------------------------------------------
  if (millis() - tiempoLectura >= 375) {
    tiempoLectura = millis();

    // Obtener lectura previa lista del sensor
    float tempLeida = sensors.getTempCByIndex(0);
    if (tempLeida != DEVICE_DISCONNECTED_C && tempLeida > -50.0) {
      temperatura = tempLeida;
    }

    // Pedir la siguiente conversión de 11 bits para los próximos 375 ms
    sensors.requestTemperatures();

    // Lógica de conmutación de histéresis
    if (temperatura >= HL) {
      enfriando = true;  
    } else if (temperatura <= LL) {
      enfriando = false; 
    }

    // Actuadores
    if (enfriando) {
      digitalWrite(FOCO_CALENTAR, HIGH); // APAGA el Foco
      digitalWrite(ENFRIAMIENTO, LOW);   // ENCIENDE Peltier y Ventiladores
    } else {
      digitalWrite(ENFRIAMIENTO, HIGH);  // APAGA Peltier y Ventiladores
      digitalWrite(FOCO_CALENTAR, LOW);  // ENCIENDE el Foco
    }
  }

  // ---------------------------------------------------
  // 4. IMPRESIÓN EN PANTALLA LCD (Cada 150 ms)
  // ---------------------------------------------------
  if (millis() - tiempoLCD >= 150) {
    tiempoLCD = millis();

    char buffer[10];

    // --- FILA 0: LL y Temperatura Actual ---
    lcd.setCursor(0, 0);
    if (seleccionLL && !mostrarSeleccion) {
      lcd.print("LL=--C ");
    } else {
      sprintf(buffer, "LL=%-2dC ", LL);
      lcd.print(buffer);
    }

    lcd.setCursor(8, 0);
    dtostrf(temperatura, 4, 1, buffer); // Muestra la temperatura con 1 decimal
    lcd.print("T=");
    lcd.print(buffer);
    lcd.print("C ");

    // --- FILA 1: HL y Estado ---
    lcd.setCursor(0, 1);
    if (!seleccionLL && !mostrarSeleccion) {
      lcd.print("HL=--C ");
    } else {
      sprintf(buffer, "HL=%-2dC ", HL);
      lcd.print(buffer);
    }

    lcd.setCursor(8, 1);
    if (enfriando) {
      lcd.print("[ENFR.] ");
    } else {
      lcd.print("[CALEN] ");
    }
  }
}