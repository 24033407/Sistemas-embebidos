# Sistemas Embebidos

Repositorio de prácticas de la materia **Sistemas Embebidos**, realizadas con ESP32, PlatformIO y Wokwi.

## Prácticas

### **09_Practica – Controlador PID básico**

🌡️ Control automático de temperatura mediante un controlador PID con ESP32.  
ESP32 · PlatformIO · Arduino · Wokwi

### Descripción del proyecto

Este proyecto consiste en desarrollar un sistema de control automático de temperatura utilizando una tarjeta ESP32 y un controlador proporcional, integral y derivativo (PID). Para medir la temperatura se utiliza un sensor digital DS18B20, mientras que un servomotor y un ventilador controlado mediante un relevador permiten realizar las acciones de control.

El programa calcula el error entre la temperatura medida y el valor de referencia (setpoint), y utiliza las ganancias Kp, Ki y Kd para determinar la salida del controlador. También incorpora el método de Tustin para la discretización de las acciones integral y derivativa, un filtro de promedio móvil de cinco lecturas y un mecanismo anti-windup para evitar la acumulación excesiva de la acción integral.

El sistema cuenta con una pantalla LCD de 16×2 con comunicación I²C y un teclado matricial 4×4, que permiten visualizar las variables principales y modificar el setpoint y las ganancias del controlador durante su funcionamiento.

### Objetivo general

Implementar un sistema embebido basado en ESP32 capaz de controlar la temperatura mediante un controlador PID digital, integrando un sensor de temperatura, actuadores y una interfaz de usuario para supervisar y modificar los parámetros del sistema.

### Objetivos específicos

🌡️ Configurar el sensor DS18B20 para realizar mediciones de temperatura.

🧮 Implementar las acciones proporcional, integral y derivativa de un controlador PID.

📐 Aplicar el método de Tustin para discretizar las acciones integral y derivativa.

⚙️ Controlar la posición angular de un servomotor mediante la salida del controlador.

🔌 Implementar el accionamiento de un ventilador mediante un módulo relevador.

📊 Aplicar un filtro de promedio móvil para suavizar las lecturas de temperatura.

⌨️ Configurar un teclado matricial 4×4 para modificar el setpoint y las ganancias Kp, Ki y Kd.

🖥️ Visualizar la temperatura, el error y la posición del servomotor en una pantalla LCD de 16×2.

🛡️ Implementar límites de salida y protección anti-windup para mejorar el funcionamiento del controlador.

### Materiales y herramientas

- ESP32 DevKit V1
- Sensor digital de temperatura DS18B20
- Resistencia pull-up de 4.7 kΩ
- Servomotor
- Módulo relevador de un canal
- Ventilador
- Pantalla LCD 16×2 con adaptador I²C
- Teclado matricial 4×4
- Protoboard y cables de conexión
- Visual Studio Code
- PlatformIO
- Wokwi

Integrantes: Jesús Ángel Bonilla Durán, Viridiana Santibañez Carrillo y Jocelyn Rodriguez Torres.

Profesor: Hugo Antonio Méndez Guzmán.