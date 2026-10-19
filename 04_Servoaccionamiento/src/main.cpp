#include <Arduino.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

Servo servo;
// Dirección I2C común: 0x27 (o 0x3F), 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SERVO_PIN 18
#define BTN_AUMENTAR 12  // Girar a la derecha (Aumentar brillo)
#define BTN_DISMINUIR 14 // Girar a la izquierda (Reducir brillo)

int angulo = 0;           // Posición inicial del servo (0 a 180)
int anguloAnterior = -1;  // Detección de cambios

void setup() {
    Serial.begin(115200);

    // Botones con resistencia PULLUP interna (no requieren resistencia externa)
    pinMode(BTN_AUMENTAR, INPUT_PULLUP);
    pinMode(BTN_DISMINUIR, INPUT_PULLUP);

    // Configuración del servo
    servo.attach(SERVO_PIN);
    servo.write(angulo);

    // Configuración de la pantalla LCD
    lcd.init();
    lcd.backlight();

    // Mensaje de bienvenida fija en la primera fila
    lcd.setCursor(0, 0);
    lcd.print(" REGULADOR DIMER ");
}

void loop() {
    bool huboCambio = false;

    // --- Botón Aumentar / Derecha ---
    if (digitalRead(BTN_AUMENTAR) == LOW) {
        if (angulo < 180) {
            angulo += 2; // Incrementar de 2 en 2 grados para un movimiento fluido
            if (angulo > 180) angulo = 180;
            huboCambio = true;
        }
        delay(35); // Controla la velocidad del giro al mantener presionado
    }

    // --- Botón Disminuir / Izquierda ---
    if (digitalRead(BTN_DISMINUIR) == LOW) {
        if (angulo > 0) {
            angulo -= 2; // Decrementar de 2 en 2 grados
            if (angulo < 0) angulo = 0;
            huboCambio = true;
        }
        delay(35); // Controla la velocidad del giro al mantener presionado
    }

    if (huboCambio || angulo != anguloAnterior) {
        servo.write(angulo);
        int porcentaje = map(angulo, 0, 180, 0, 100);

        lcd.setCursor(0, 1);
        lcd.print("Brillo:");
        if (porcentaje < 100) lcd.print(" "); 
        if (porcentaje < 10)  lcd.print(" ");
        lcd.print(porcentaje);
        lcd.print("% ");

        if (angulo < 100) lcd.print(" ");
        if (angulo < 10)  lcd.print(" ");
        lcd.print(angulo);
        lcd.print((char)223); // Muestra el símbolo °

        anguloAnterior = angulo;
    }
}