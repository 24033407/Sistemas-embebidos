#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PIN_DS18B20 32
#define PIN_SERVO 13
#define PIN_RELE_VENTILADOR 23

const byte RELE_ON = LOW;
const byte RELE_OFF = HIGH;

LiquidCrystal_I2C lcd(0x27, 16, 2);

OneWire oneWire(PIN_DS18B20);
DallasTemperature sensor(&oneWire);

Servo servo;

const int SERVO_MIN_ANG = 0;
const int SERVO_MAX_ANG = 180;
const float CONTROL_MIN = 0.0f;
const float CONTROL_MAX = (float)(SERVO_MAX_ANG - SERVO_MIN_ANG);

const byte FILAS = 4;
const byte COLUMNAS = 4;

char keys[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[FILAS] = {18, 33, 25, 26};
byte colPins[COLUMNAS] = {27, 14, 15, 4};

Keypad teclado = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  FILAS,
  COLUMNAS
);

float setpoint = 40.0f;

float Kp = 1.0f;
float Ki = 0.0f;
float Kd = 0.0f;

float temperatura = 0.0f;
float error = 0.0f;

float control = 0.0f;
float anguloServo = SERVO_MIN_ANG;

float terminoIntegral = 0.0f;
float terminoDerivativo = 0.0f;
float errorAnterior = 0.0f;
float temperaturaAnterior = 0.0f;
bool pidInicializado = false;

const float TAU_DERIVATIVA = 1.0f;

bool sensorValido = false;

const byte NUM_LECTURAS = 5;

float lecturasTemperatura[NUM_LECTURAS] = {0};
byte indiceLectura = 0;
byte lecturasLlenas = 0;
float sumaTemperaturas = 0.0f;

const float DIF_ENCENDIDO_VENT = 1.0f;
const float DIF_APAGADO_VENT = 0.1f;

bool ventiladorEncendido = false;

unsigned long tiempoSensor = 0;
unsigned long tiempoLCD = 0;

const unsigned long INTERVALO_SENSOR = 500;
const unsigned long INTERVALO_LCD = 300;

byte modoConfig = 0;
char entrada[10];
byte longitudEntrada = 0;

void actualizarSalidaPID() {

  if (ventiladorEncendido) {
    control = CONTROL_MIN;
    anguloServo = SERVO_MIN_ANG;
    servo.write(SERVO_MIN_ANG);
    return;
  }

  float salidaSinSaturar = Kp * error
                         + terminoIntegral
                         + terminoDerivativo;

  control = constrain(salidaSinSaturar, CONTROL_MIN, CONTROL_MAX);

  anguloServo = SERVO_MIN_ANG + control;

  anguloServo = constrain(
    anguloServo,
    (float)SERVO_MIN_ANG,
    (float)SERVO_MAX_ANG
  );

  servo.write((int)roundf(anguloServo));
}

void reiniciarEstadosPID() {

  terminoIntegral = 0.0f;
  terminoDerivativo = 0.0f;
  errorAnterior = 0.0f;
  temperaturaAnterior = 0.0f;
  pidInicializado = false;
}

void establecerVentilador(bool encender) {

  if (ventiladorEncendido == encender) {
    return;
  }

  ventiladorEncendido = encender;

  if (ventiladorEncendido) {
    digitalWrite(PIN_RELE_VENTILADOR, RELE_ON);
    Serial.println("VENTILADOR: ENCENDIDO");
  }
  else {
    digitalWrite(PIN_RELE_VENTILADOR, RELE_OFF);
    Serial.println("VENTILADOR: APAGADO");
  }
}

void controlarVentilador() {

  if (!sensorValido) {
    establecerVentilador(true);
    reiniciarEstadosPID();
    control = CONTROL_MIN;
    anguloServo = SERVO_MIN_ANG;
    servo.write(SERVO_MIN_ANG);
    return;
  }

  float temperaturaEncendido = setpoint + DIF_ENCENDIDO_VENT;
  float temperaturaApagado = setpoint + DIF_APAGADO_VENT;

  bool estabaEncendido = ventiladorEncendido;

  if (!ventiladorEncendido && temperatura >= temperaturaEncendido) {
    establecerVentilador(true);
  }
  else if (ventiladorEncendido && temperatura <= temperaturaApagado) {
    establecerVentilador(false);
  }

  if (ventiladorEncendido) {
    terminoIntegral = 0.0f;
    terminoDerivativo = 0.0f;
    errorAnterior = setpoint - temperatura;
    temperaturaAnterior = temperatura;
    pidInicializado = true;
    error = setpoint - temperatura;
    actualizarSalidaPID();
  }
  else if (estabaEncendido && !ventiladorEncendido) {
    terminoIntegral = 0.0f;
    terminoDerivativo = 0.0f;
    error = setpoint - temperatura;
    errorAnterior = error;
    temperaturaAnterior = temperatura;
    pidInicializado = true;
    actualizarSalidaPID();
  }
}

float calcularPromedio(float nuevaLectura) {

  if (lecturasLlenas == NUM_LECTURAS) {
    sumaTemperaturas -= lecturasTemperatura[indiceLectura];
  }
  else {
    lecturasLlenas++;
  }

  lecturasTemperatura[indiceLectura] = nuevaLectura;
  sumaTemperaturas += nuevaLectura;

  indiceLectura++;
  if (indiceLectura >= NUM_LECTURAS) {
    indiceLectura = 0;
  }

  return sumaTemperaturas / lecturasLlenas;
}

void reiniciarPromedio() {

  sumaTemperaturas = 0.0f;
  indiceLectura = 0;
  lecturasLlenas = 0;

  for (byte i = 0; i < NUM_LECTURAS; i++) {
    lecturasTemperatura[i] = 0.0f;
  }
}

void calcularControlPID(float Ts) {

  error = setpoint - temperatura;

  if (ventiladorEncendido) {
    terminoIntegral = 0.0f;
    terminoDerivativo = 0.0f;
    errorAnterior = error;
    temperaturaAnterior = temperatura;
    pidInicializado = true;
    actualizarSalidaPID();
    return;
  }

  if (!pidInicializado) {
    errorAnterior = error;
    temperaturaAnterior = temperatura;
    terminoIntegral = 0.0f;
    terminoDerivativo = 0.0f;
    pidInicializado = true;
    actualizarSalidaPID();
    return;
  }

  if (Ts <= 0.0f || !isfinite(Ts)) {
    actualizarSalidaPID();
    return;
  }

  float P = Kp * error;

  float incrementoIntegral = Ki * (Ts / 2.0f) * (error + errorAnterior);
  float integralPropuesta = terminoIntegral + incrementoIntegral;

  float denominadorD = 2.0f * TAU_DERIVATIVA + Ts;
  float alphaD = (2.0f * TAU_DERIVATIVA - Ts) / denominadorD;
  float betaD = (2.0f * Kd) / denominadorD;

  float derivadaPropuesta = alphaD * terminoDerivativo
                          - betaD * (temperatura - temperaturaAnterior);

  float salidaPropuesta = P + integralPropuesta + derivadaPropuesta;

  bool integralEmpeoraSaturacion =
    (salidaPropuesta > CONTROL_MAX && incrementoIntegral > 0.0f) ||
    (salidaPropuesta < CONTROL_MIN && incrementoIntegral < 0.0f);

  if (!integralEmpeoraSaturacion) {
    terminoIntegral = integralPropuesta;
  }

  terminoDerivativo = derivadaPropuesta;
  errorAnterior = error;
  temperaturaAnterior = temperatura;

  actualizarSalidaPID();
}

void leerTemperatura(float Ts) {

  float nuevaTemperatura = sensor.getTempCByIndex(0);

  if (
    nuevaTemperatura == DEVICE_DISCONNECTED_C ||
    nuevaTemperatura < -55.0f ||
    nuevaTemperatura > 125.0f
  ) {
    sensorValido = false;

    control = CONTROL_MIN;
    error = 0.0f;
    anguloServo = SERVO_MIN_ANG;
    servo.write(SERVO_MIN_ANG);

    reiniciarPromedio();
    reiniciarEstadosPID();

    establecerVentilador(true);

    Serial.println("ERROR: SENSOR DESCONECTADO");
    return;
  }

  sensorValido = true;

  temperatura = calcularPromedio(nuevaTemperatura);

  calcularControlPID(Ts);

  controlarVentilador();

  Serial.print("T: ");
  Serial.print(temperatura, 2);
  Serial.print(" C | SP: ");
  Serial.print(setpoint, 2);
  Serial.print(" | E: ");
  Serial.print(error, 2);
  Serial.print(" | Kp: ");
  Serial.print(Kp, 3);
  Serial.print(" | Ki: ");
  Serial.print(Ki, 5);
  Serial.print(" | Kd: ");
  Serial.print(Kd, 5);
  Serial.print(" | I: ");
  Serial.print(terminoIntegral, 2);
  Serial.print(" | D: ");
  Serial.print(terminoDerivativo, 2);
  Serial.print(" | Control: ");
  Serial.print(control, 2);
  Serial.print(" | Servo: ");
  Serial.print(anguloServo, 1);
  Serial.print(" | Ventilador: ");
  Serial.println(ventiladorEncendido ? "ON" : "OFF");
}

void actualizarLCD() {

  char buf1[17];
  char buf2[17];
  char linea1[17];
  char linea2[17];

  if (modoConfig != 0) {

    switch (modoConfig) {
      case 1:
        snprintf(buf1, sizeof(buf1), "NUEVO SETPOINT:");
        break;
      case 2:
        snprintf(buf1, sizeof(buf1), "NUEVO VALOR Kp:");
        break;
      case 3:
        snprintf(buf1, sizeof(buf1), "NUEVO VALOR Ki:");
        break;
      case 4:
        snprintf(buf1, sizeof(buf1), "NUEVO VALOR Kd:");
        break;
      default:
        snprintf(buf1, sizeof(buf1), "CONFIGURACION:");
        break;
    }

    snprintf(buf2, sizeof(buf2), "> %s", entrada);

    if (modoConfig == 1) {
      size_t longitud = strlen(buf2);
      if (longitud < 16) {
        snprintf(buf2 + longitud, sizeof(buf2) - longitud, " C");
      }
    }

    snprintf(linea1, sizeof(linea1), "%-16s", buf1);
    snprintf(linea2, sizeof(linea2), "%-16s", buf2);

    lcd.setCursor(0, 0);
    lcd.print(linea1);
    lcd.setCursor(0, 1);
    lcd.print(linea2);
    return;
  }

  if (!sensorValido) {
    lcd.setCursor(0, 0);
    lcd.print("ERROR SENSOR    ");
    lcd.setCursor(0, 1);
    lcd.print("REVISAR DS18B20 ");
    return;
  }

  snprintf(
    buf1,
    sizeof(buf1),
    "T:%4.1f SP:%4.1f",
    temperatura,
    setpoint
  );

  snprintf(
    buf2,
    sizeof(buf2),
    "E:%+5.1f SV:%3.0f",
    error,
    anguloServo
  );

  snprintf(linea1, sizeof(linea1), "%-16s", buf1);
  snprintf(linea2, sizeof(linea2), "%-16s", buf2);

  lcd.setCursor(0, 0);
  lcd.print(linea1);
  lcd.setCursor(0, 1);
  lcd.print(linea2);
}

void guardarConfiguracion() {

  if (longitudEntrada == 0) {
    return;
  }

  float valor = atof(entrada);

  if (!isfinite(valor) || valor < 0.0f) {
    entrada[0] = '\0';
    longitudEntrada = 0;
    return;
  }

  switch (modoConfig) {

    case 1:
      setpoint = valor;
      Serial.print("Nuevo setpoint: ");
      Serial.println(setpoint);
      break;

    case 2:
      Kp = valor;
      Serial.print("Nuevo Kp: ");
      Serial.println(Kp, 6);
      break;

    case 3:
      Ki = valor;
      Serial.print("Nuevo Ki: ");
      Serial.println(Ki, 6);
      break;

    case 4:
      Kd = valor;
      Serial.print("Nuevo Kd: ");
      Serial.println(Kd, 6);
      break;

    default:
      return;
  }

  modoConfig = 0;
  entrada[0] = '\0';
  longitudEntrada = 0;

  if (sensorValido) {
    error = setpoint - temperatura;
    errorAnterior = error;
    temperaturaAnterior = temperatura;
    terminoDerivativo = 0.0f;
    pidInicializado = true;

    actualizarSalidaPID();
    controlarVentilador();
  }

  lcd.clear();
}

void leerTeclado() {

  char tecla = teclado.getKey();
  if (!tecla) {
    return;
  }

  if (modoConfig == 0) {

    if (tecla == '*') {
      modoConfig = 1;
    }
    else if (tecla == 'B') {
      modoConfig = 2;
    }
    else if (tecla == '1') {
      modoConfig = 3;
    }
    else if (tecla == 'C') {
      modoConfig = 4;
    }
    else {
      return;
    }

    longitudEntrada = 0;
    entrada[0] = '\0';
    lcd.clear();
    return;
  }

  if (tecla == 'C') {
    modoConfig = 0;
    longitudEntrada = 0;
    entrada[0] = '\0';
    lcd.clear();
    return;
  }

  if (tecla == 'D') {
    guardarConfiguracion();
    return;
  }

  if (tecla == '#') {
    bool tienePunto = false;

    for (byte i = 0; i < longitudEntrada; i++) {
      if (entrada[i] == '.') {
        tienePunto = true;
        break;
      }
    }

    if (!tienePunto && longitudEntrada > 0 && longitudEntrada < 8) {
      entrada[longitudEntrada] = '.';
      longitudEntrada++;
      entrada[longitudEntrada] = '\0';
    }
    return;
  }

  if (tecla >= '0' && tecla <= '9') {
    if (longitudEntrada < 8) {
      entrada[longitudEntrada] = tecla;
      longitudEntrada++;
      entrada[longitudEntrada] = '\0';
    }
  }
}

void setup() {

  Serial.begin(115200);

  digitalWrite(PIN_RELE_VENTILADOR, RELE_OFF);
  pinMode(PIN_RELE_VENTILADOR, OUTPUT);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("CONTROL PID");
  lcd.setCursor(0, 1);
  lcd.print("INICIANDO...");

  sensor.begin();
  sensor.setResolution(11);
  sensor.setWaitForConversion(false);
  sensor.requestTemperatures();

  tiempoSensor = millis();

  servo.setPeriodHertz(50);
  servo.attach(PIN_SERVO, 500, 2400);
  servo.write(SERVO_MIN_ANG);

  delay(1000);
  lcd.clear();

  Serial.println("CONTROL PID DISCRETO INICIADO");
  Serial.println("RANGO DEL SERVO: 0 A 180 GRADOS");
  Serial.println("INTEGRAL: TUSTIN / TRAPEZOIDAL");
  Serial.println("DERIVADA: TUSTIN FILTRADA SOBRE MEDICION");
  Serial.println("PROMEDIO MOVIL: 5 LECTURAS");
  Serial.println("TECLAS: *=SP, B=Kp, 1=Ki, C=Kd");
}

void loop() {

  leerTeclado();

  unsigned long ahora = millis();

  if (ahora - tiempoSensor >= INTERVALO_SENSOR) {

    float Ts = (ahora - tiempoSensor) / 1000.0f;
    tiempoSensor = ahora;

    leerTemperatura(Ts);

    sensor.requestTemperatures();
  }

  if (millis() - tiempoLCD >= INTERVALO_LCD) {
    tiempoLCD = millis();
    actualizarLCD();
  }
}