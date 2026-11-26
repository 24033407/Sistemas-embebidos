
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include <stdlib.h>
#include <string.h>

#define PIN_DS18B20 32
#define PIN_SERVO 13
#define PIN_RELE_VENTILADOR 23

const byte RELE_ON = LOW;
const byte RELE_OFF = HIGH;

LiquidCrystal_I2C lcd(0x27, 16, 2);

OneWire oneWire(PIN_DS18B20);
DallasTemperature sensor(&oneWire);

Servo servo;

const int SERVO_MIN_ANG = 20;
const int SERVO_MAX_ANG = 180;

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

float setpoint = 40.0;
float Kp = 0.50;

const float CONTROL_MIN = 0.0;
const float CONTROL_MAX = 5.0;

float temperatura = 0.0;
float error = 0.0;
float control = 0.0;
float anguloServo = SERVO_MIN_ANG;

bool sensorValido = false;

const byte NUM_LECTURAS = 5;

float lecturasTemperatura[NUM_LECTURAS] = {0};

byte indiceLectura = 0;
byte lecturasLlenas = 0;

float sumaTemperaturas = 0.0;

const float DIF_ENCENDIDO_VENT = 3.0;
const float DIF_APAGADO_VENT = 0.5;

bool ventiladorEncendido = false;

unsigned long tiempoSensor = 0;
unsigned long tiempoLCD = 0;

const unsigned long INTERVALO_SENSOR = 500;
const unsigned long INTERVALO_LCD = 300;

byte modoConfig = 0;

char entrada[10];
byte longitudEntrada = 0;

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

    return;
  }

  float temperaturaEncendido =
    setpoint + DIF_ENCENDIDO_VENT;

  float temperaturaApagado =
    setpoint + DIF_APAGADO_VENT;

  if (!ventiladorEncendido &&
      temperatura >= temperaturaEncendido) {

    establecerVentilador(true);

  }

  else if (ventiladorEncendido &&
           temperatura <= temperaturaApagado) {

    establecerVentilador(false);

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

  sumaTemperaturas = 0.0;
  indiceLectura = 0;
  lecturasLlenas = 0;

  for (byte i = 0; i < NUM_LECTURAS; i++) {

    lecturasTemperatura[i] = 0.0;

  }
}

void calcularControl() {

  error = setpoint - temperatura;

  control = Kp * error;

  control = constrain(
    control,
    CONTROL_MIN,
    CONTROL_MAX
  );

  anguloServo = SERVO_MIN_ANG +
    (SERVO_MAX_ANG - SERVO_MIN_ANG) *
    (control / CONTROL_MAX);

  anguloServo = constrain(
    anguloServo,
    SERVO_MIN_ANG,
    SERVO_MAX_ANG
  );

  servo.write((int)round(anguloServo));
}

void leerTemperatura() {

  float nuevaTemperatura = sensor.getTempCByIndex(0);

  if (
    nuevaTemperatura == DEVICE_DISCONNECTED_C ||
    nuevaTemperatura < -55 ||
    nuevaTemperatura > 125
  ) {

    sensorValido = false;

    control = 0;
    error = 0;

    anguloServo = SERVO_MIN_ANG;

    servo.write(SERVO_MIN_ANG);

    reiniciarPromedio();

    establecerVentilador(true);

    Serial.println("ERROR: SENSOR DESCONECTADO");

    return;
  }

  sensorValido = true;

  temperatura = calcularPromedio(nuevaTemperatura);

  calcularControl();

  controlarVentilador();

  Serial.print("Temperatura: ");
  Serial.print(temperatura, 2);

  Serial.print(" C | Setpoint: ");
  Serial.print(setpoint, 2);

  Serial.print(" C | Error: ");
  Serial.print(error, 2);

  Serial.print(" | Kp: ");
  Serial.print(Kp, 2);

  Serial.print(" | Control: ");
  Serial.print(control, 2);

  Serial.print(" | Servo: ");
  Serial.print(anguloServo, 1);

  Serial.print(" | Ventilador: ");

  if (ventiladorEncendido) {
    Serial.println("ON");
  }
  else {
    Serial.println("OFF");
  }
}

void actualizarLCD() {

  char buf1[17];
  char buf2[17];

  char linea1[17];
  char linea2[17];

  if (modoConfig != 0) {

    if (modoConfig == 1) {

      snprintf(buf1, sizeof(buf1), "NUEVO SETPOINT:");

    }
    else {

      snprintf(buf1, sizeof(buf1), "NUEVO VALOR Kp:");

    }

    snprintf(buf2, sizeof(buf2), "> %s", entrada);

    if (modoConfig == 1) {

      size_t longitud = strlen(buf2);

      if (longitud < 16) {

        snprintf(
          buf2 + longitud,
          sizeof(buf2) - longitud,
          " C"
        );

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

  if (modoConfig == 1) {

    if (valor >= 0) {

      setpoint = valor;

      Serial.print("Nuevo setpoint: ");
      Serial.println(setpoint);

    }
    else {

      entrada[0] = '\0';
      longitudEntrada = 0;

      return;
    }

  }

  else if (modoConfig == 2) {

    if (valor >= 0 && valor <= 10) {

      Kp = valor;

      Serial.print("Nuevo Kp: ");
      Serial.println(Kp);

    }
    else {

      entrada[0] = '\0';
      longitudEntrada = 0;

      return;
    }

  }

  modoConfig = 0;

  entrada[0] = '\0';
  longitudEntrada = 0;

  if (sensorValido) {

    calcularControl();

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

      longitudEntrada = 0;
      entrada[0] = '\0';

      lcd.clear();

    }

    else if (tecla == 'B') {

      modoConfig = 2;

      longitudEntrada = 0;
      entrada[0] = '\0';

      lcd.clear();

    }

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

      }
    }

    if (!tienePunto &&
        longitudEntrada > 0 &&
        longitudEntrada < 8) {

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
  lcd.print("CONTROL TEMP.");

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

  Serial.println("CONTROL PROPORCIONAL INICIADO");
  Serial.println("PROMEDIO MOVIL: 5 LECTURAS");
  Serial.println("VENTILADOR: HISTÉRESIS ACTIVADA");
}

void loop() {

  leerTeclado();

  if (millis() - tiempoSensor >= INTERVALO_SENSOR) {

    tiempoSensor = millis();

    leerTemperatura();

    sensor.requestTemperatures();

  }

  if (millis() - tiempoLCD >= INTERVALO_LCD) {

    tiempoLCD = millis();

    actualizarLCD();

  }
}
