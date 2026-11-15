# 🎃 Animatrónico de Halloween con ESP32

**ESP32 | HC-SR04 | Servomotores | LED | DFPlayer Mini**

## Descripción
Animatrónico interactivo que detecta la proximidad de personas mediante un sensor ultrasónico y activa movimientos, luces y sonidos de terror.

## Material requerido
- ESP32 DevKit V1
- Sensor ultrasónico HC-SR04
- 2 servomotores
- 3 LED y resistencias
- DFPlayer Mini
- Tarjeta microSD y altavoz
- Cables y fuente de alimentación

## Funcionamiento
Cuando el sensor detecta un objeto a una distancia de hasta 150 cm, el ESP32 inicia la reproducción de audio, mueve los servomotores entre posiciones programadas y enciende los tres LED. Al terminar la secuencia, las luces se apagan y el primer servomotor regresa a 0°.

## Conexiones

| Componente | GPIO |
|---|---|
| Servomotores | 18 y 21 |
| LED | 14, 15 y 13 |
| HC-SR04 | TRIG 4 / ECHO 5 |
| DFPlayer Mini | RX 16 / TX 17 |

## Tecnologías
- Programación C++ con Arduino Framework.
- PlatformIO.
- Simulación en Wokwi.
- Comunicación serial UART.
