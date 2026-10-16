# Práctica 02 - Llenado y vaciado automático

## Objetivo

Implementar un sistema de control automático para el proceso de llenado y vaciado de un tanque utilizando un ESP32, sensores de nivel, módulos relé para la activación de bombas y una pantalla LCD con módulo I2C para monitorear el estado del sistema en tiempo real.

## Funcionamiento

El sistema gestiona el ciclo automático del tanque de la siguiente manera:

* **Pantalla LCD (16x2 I2C):** Muestra constantemente el estado del sistema (ej. "Llenando...", "Tanque Lleno", "Vaciando...", "Tanque Vacío").
* **Sensor Nivel Bajo:** Detecta cuando el nivel de agua está al mínimo e inicia el proceso de **llenado**, activando el relé de la bomba de llenado.
* **Sensor Nivel Alto:** Detecta cuando el tanque alcanza su capacidad máxima, apaga la bomba de llenado y activa el relé de la bomba/válvula de vaciado.
* **Módulos Relé:** Conmutan la alimentación de las bombas de agua de forma segura desde el ESP32.


* ESP32
* Pantalla LCD 16x2 con Módulo I2C
* Módulo Relé de 2 canales
* Bombas de agua (Llenado y Vaciado)
* 2 botones / pulsadores (simulando sensores de nivel)
* Protoboard
* Cables Dupont

## Software utilizado

* Visual Studio Code
* PlatformIO
* Wokwi
* Arduino Framework

## Archivos

* `src/main.cpp` — Código de la práctica.
* `platformio.ini` — Configuración de PlatformIO (incluyendo librería LiquidCrystal_I2C).
* `wokwi.toml` — Configuración de la simulación.
* `diagram.json` — Diagrama del circuito en Wokwi.