#include <ESP32Servo.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

// Configuramos la comunicación para el MP3 (Pines 16 y 17)
HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;

Servo miServo; 
Servo SERVO2; // Creamos el objeto servo
int pinServo = 18; // El pin donde conectaste el cable de señal
int pinServo2 = 21;
int led1 = 14; 
int led2 = 15;
int led3 = 13;
int an1 = 55;
int an2 = 135;

// Pines del sensor ultrasónico
int trigPin = 4; // Pin que envía el pulso
int echoPin = 5; // Pin que recibe el rebote
int distanciaActivacion = 150.00; // DISTANCIA EN CENTÍMETROS (Modifica este número a tu gusto)

void setup() {
  Serial.begin(115200);
  // Iniciamos comunicación con DFPlayer (RX=16, TX=17)
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  // Configuramos los servos y LEDs
  miServo.attach(pinServo);
  SERVO2.attach(pinServo2);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  
  // Configuramos los pines del ultrasónico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inicializamos el módulo de sonido
  Serial.println("Iniciando MP3...");
  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("Error de conexión con DFPlayer");
  } else {
    myDFPlayer.volume(30); // Ajusta el volumen (0 a 30)
  }
}

void loop() {
  // 1. Disparamos el pulso ultrasónico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 2. Leemos el tiempo que tarda en regresar el eco
  long duracion = pulseIn(echoPin, HIGH);
  
  // 3. Calculamos la distancia en centímetros
  long distancia = duracion * 0.034 / 2;

  // 4. Si la distancia es válida y menor a la distancia de activación, arranca la rutina
  if (distancia > 0 && distancia <= distanciaActivacion) {
    
    delay(700); // Pausa de 2 segundos de espera tras haber sensado

    // Mandamos reproducir el audio 0001.mp3 de la carpeta mp3
    myDFPlayer.playMp3Folder(1);

    // Inicia la rutina de movimientos de tu animatrónico
    miServo.write(0);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);
    SERVO2.write(an1);
    delay(200);
    SERVO2.write(an2);
    delay(200);

    // Gira a 0 grados
    miServo.write(0);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    delay(2000); // Espera 2 segundos

    // Gira a 90 grados (al centro)
    miServo.write(180);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    delay(6000); // Espera 6 segundos
    
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    miServo.write(0);
  }
  
  // Breve pausa para no saturar al sensor antes de volver a medir
  delay(50);
}