#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =====================================================
// PINES
// =====================================================

#define TRIG 5
#define ECHO 18

// Bombas / LEDs
#define BOMBA_VACIADO 19
#define BOMBA_LLENADO 23

// Botones
#define BOTON_SUBIR 25
#define BOTON_BAJAR 26
#define BOTON_SELECT 27


// =====================================================
// CALIBRACION DEL TANQUE
// =====================================================

// Distancia cuando el tanque está al 100%
#define DISTANCIA_100 3.0

// Distancia cuando el tanque está al 0%
#define DISTANCIA_0 17.5

// Capacidad total del tanque
#define CAPACIDAD_LITROS 1.96


// =====================================================
// LIMITES DEL TANQUE
// =====================================================

int LL = 10;
int HL = 90;


// =====================================================
// LCD
// =====================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);


// =====================================================
// ESTADO DE LAS BOMBAS
// =====================================================

bool llenando = false;
bool vaciando = false;


// =====================================================
// SELECCION DE PARAMETRO
// =====================================================

// true  = estamos modificando LL
// false = estamos modificando HL

bool seleccionLL = true;


// =====================================================
// ESTADOS ANTERIORES DE LOS BOTONES
// =====================================================

bool estadoAnteriorSubir = HIGH;
bool estadoAnteriorBajar = HIGH;
bool estadoAnteriorSelect = HIGH;


// =====================================================
// VARIABLES PARA PARPADEO
// =====================================================

unsigned long tiempoParpadeo = 0;

bool mostrarSeleccion = true;


// =====================================================
// SETUP
// =====================================================

void setup() {

  // ---------------------------------------------------
  // HC-SR04
  // ---------------------------------------------------

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);


  // ---------------------------------------------------
  // BOMBAS / LEDs
  // ---------------------------------------------------

  pinMode(BOMBA_VACIADO, OUTPUT);
  pinMode(BOMBA_LLENADO, OUTPUT);

  digitalWrite(BOMBA_VACIADO, LOW);
  digitalWrite(BOMBA_LLENADO, LOW);


  // ---------------------------------------------------
  // BOTONES
  // ---------------------------------------------------

  pinMode(BOTON_SUBIR, INPUT_PULLUP);
  pinMode(BOTON_BAJAR, INPUT_PULLUP);
  pinMode(BOTON_SELECT, INPUT_PULLUP);


  // ---------------------------------------------------
  // LCD
  // ---------------------------------------------------

  lcd.init();
  lcd.backlight();
  lcd.clear();

  delay(500);
}


// =====================================================
// LOOP
// =====================================================

void loop() {


  // ===================================================
  // 1. LEER BOTONES
  // ===================================================

  bool estadoSubir = digitalRead(BOTON_SUBIR);
  bool estadoBajar = digitalRead(BOTON_BAJAR);
  bool estadoSelect = digitalRead(BOTON_SELECT);


  // ===================================================
  // 2. BOTON SELECT
  // ===================================================

  if (estadoSelect == LOW && estadoAnteriorSelect == HIGH) {

    // Cambiar entre LL y HL
    seleccionLL = !seleccionLL;

    // Reiniciar parpadeo
    mostrarSeleccion = true;
    tiempoParpadeo = millis();

    delay(150);
  }


  // ===================================================
  // 3. BOTON SUBIR
  // ===================================================

  if (estadoSubir == LOW && estadoAnteriorSubir == HIGH) {

    // -----------------------------------------------
    // Estamos modificando LL
    // -----------------------------------------------

    if (seleccionLL) {

      // LL no puede alcanzar HL
      if (LL < HL - 1) {
        LL++;
      }
    }


    // -----------------------------------------------
    // Estamos modificando HL
    // -----------------------------------------------

    else {

      // HL máximo = 100%
      if (HL < 100) {
        HL++;
      }
    }

    delay(150);
  }


  // ===================================================
  // 4. BOTON BAJAR
  // ===================================================

  if (estadoBajar == LOW && estadoAnteriorBajar == HIGH) {

    // -----------------------------------------------
    // Estamos modificando LL
    // -----------------------------------------------

    if (seleccionLL) {

      // LL mínimo = 0%
      if (LL > 0) {
        LL--;
      }
    }


    // -----------------------------------------------
    // Estamos modificando HL
    // -----------------------------------------------

    else {

      // HL no puede llegar a LL
      if (HL > LL + 1) {
        HL--;
      }
    }

    delay(150);
  }


  // ===================================================
  // 5. GUARDAR ESTADOS ANTERIORES
  // ===================================================

  estadoAnteriorSubir = estadoSubir;
  estadoAnteriorBajar = estadoBajar;
  estadoAnteriorSelect = estadoSelect;


  // ===================================================
  // 6. PARPADEO DEL VALOR SELECCIONADO
  // ===================================================

  if (millis() - tiempoParpadeo >= 500) {

    tiempoParpadeo = millis();

    mostrarSeleccion = !mostrarSeleccion;
  }


  // ===================================================
  // 7. GENERAR PULSO DEL HC-SR04
  // ===================================================

  digitalWrite(TRIG, LOW);

  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);


  // ===================================================
  // 8. MEDIR ECO
  // ===================================================

  long tiempo = pulseIn(ECHO, HIGH);


  // Si no hay lectura válida
  if (tiempo == 0) {
    return;
  }


  // ===================================================
  // 9. CALCULAR DISTANCIA
  // ===================================================

  float distancia = tiempo * 0.0343 / 2;


  // ===================================================
  // 10. DISTANCIA → PORCENTAJE
  // ===================================================

  float porcentaje =
      (DISTANCIA_0 - distancia) *
      100.0 /
      (DISTANCIA_0 - DISTANCIA_100);


  // ===================================================
  // 11. LIMITAR PORCENTAJE ENTRE 0 Y 100
  // ===================================================

  if (porcentaje < 0) {
    porcentaje = 0;
  }

  if (porcentaje > 100) {
    porcentaje = 100;
  }


  // ===================================================
  // 12. PORCENTAJE → LITROS
  // ===================================================

  float litros =
      (porcentaje / 100.0) *
      CAPACIDAD_LITROS;


  // ===================================================
  // 13. CONTROL AUTOMATICO
  // ===================================================

  // ---------------------------------------------------
  // TANQUE LLENO
  // ---------------------------------------------------

  if (porcentaje >= HL) {

    llenando = false;

    vaciando = true;
  }


  // ---------------------------------------------------
  // TANQUE VACIO
  // ---------------------------------------------------

  if (porcentaje <= LL) {

    vaciando = false;

    llenando = true;
  }


  // ===================================================
  // 14. ACTIVAR BOMBA DE LLENADO
  // ===================================================

  if (llenando) {

    digitalWrite(BOMBA_LLENADO, HIGH);

    digitalWrite(BOMBA_VACIADO, LOW);
  }


  // ===================================================
  // 15. ACTIVAR BOMBA DE VACIADO
  // ===================================================

  if (vaciando) {

    digitalWrite(BOMBA_LLENADO, LOW);

    digitalWrite(BOMBA_VACIADO, HIGH);
  }


  // ===================================================
  // 16. MOSTRAR LL EN LCD
  // ===================================================

  lcd.setCursor(0, 0);

  if (seleccionLL && !mostrarSeleccion) {

    // Ocultar LL durante el parpadeo
    lcd.print("LL=     ");

  } else {

    lcd.print("LL=");
    lcd.print(LL);
    lcd.print("%");
  }


  // ===================================================
  // 17. MOSTRAR PORCENTAJE ACTUAL
  // ===================================================

  lcd.setCursor(8, 0);

  lcd.print("%=");

  lcd.print((int)porcentaje);

  lcd.print("%");


  // ===================================================
  // 18. MOSTRAR HL EN LCD
  // ===================================================

  lcd.setCursor(0, 1);

  if (!seleccionLL && !mostrarSeleccion) {

    // Ocultar HL durante el parpadeo
    lcd.print("HL=     ");

  } else {

    lcd.print("HL=");
    lcd.print(HL);
    lcd.print("%");
  }


  // ===================================================
  // 19. MOSTRAR LITROS
  // ===================================================

  lcd.setCursor(8, 1);

  lcd.print("L=");

  lcd.print(litros, 2);

  lcd.print("  ");


  // ===================================================
  // 20. PEQUEÑA ESPERA
  // ===================================================

  delay(200);
}