# Práctica 01 - GPIO

## Objetivo

Configurar y utilizar entradas y salidas digitales de un ESP32 para controlar un LED mediante dos botones.

## Funcionamiento

La práctica utiliza dos botones:

* **Botón 1:** enciende el LED.
* **Botón 2:** apaga el LED.

Los botones utilizan la resistencia `INPUT_PULLUP` interna del ESP32. Al presionar un botón, la entrada digital cambia a `LOW`.

## Pines utilizados

| Elemento |    GPIO |
| -------- | ------: |
| LED      |  GPIO 23 |
| Botón 1  | GPIO 21 |
| Botón 2  | GPIO 19 |

## Componentes

* ESP32
* LED
* Resistencia de 1 kΩ
* 2 botones
* Protoboard
* Cables

## Software utilizado

* Visual Studio Code
* PlatformIO
* Wokwi
* Arduino Framework

## Archivos

* `src/main.cpp` — Código de la práctica.
* `platformio.ini` — Configuración de PlatformIO.
* `wokwi.toml` — Configuración de la simulación.
* `diagram.json` — Diagrama del circuito en Wokwi.
