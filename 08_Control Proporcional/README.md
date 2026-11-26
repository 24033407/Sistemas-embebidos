# Sistemas Embebidos
Repositorio de prácticas de la materia **Sistemas Embebidos**, realizadas con ESP32, PlatformIO y Wokwi.
## Prácticas

### **08_Practica – Control Proporcional**

Control automático de temperatura mediante un controlador proporcional (P).  
ESP32 · PlatformIO · Arduino · Wokwi

### Descripción del proyecto

Este proyecto consiste en desarrollar un sistema de control de temperatura utilizando una tarjeta ESP32 y un controlador proporcional. Para medir la temperatura se emplea un sensor DS18B20, mientras que un servomotor y un módulo de relevador permiten controlar los actuadores del sistema.

El controlador proporcional calcula la acción de control a partir del error entre la temperatura medida y el valor de referencia establecido. Además, se utiliza un teclado matricial 4×4 para modificar los parámetros de funcionamiento y una pantalla LCD de 16×2 para visualizar las variables del sistema.

### Objetivo general

Implementar un sistema embebido basado en ESP32 capaz de regular la temperatura mediante un controlador proporcional, utilizando sensores, actuadores y una interfaz de usuario.

###  Objetivos específicos

🌡️ Configurar el sensor DS18B20 para medir la temperatura.

🧮 Implementar un algoritmo de control proporcional.

⚙️ Controlar un servomotor mediante la señal de control.

🔌 Implementar el accionamiento de un relevador.

⌨️ Utilizar un teclado matricial 4×4 para modificar los parámetros del sistema.

🖥️ Visualizar la temperatura y las variables de control en una pantalla LCD 16×2 mediante comunicación I²C.

🧪 Evaluar el funcionamiento del sistema mediante pruebas.

### Materiales y herramientas

- ESP32 DevKit V1
- Sensor de temperatura DS18B20
- Servomotor
- Módulo relevador
- Ventilador
- Pantalla LCD 16×2 con I²C
- Teclado matricial 4×4
- Visual Studio Code
- PlatformIO
- Wokwi

Integrantes: Jesús Ángel Bonilla Durán, Viridiana Santibañez Carrillo y Jocelyn Rodriguez Torres.

Profesor: Hugo Antonio Méndez Guzmán.