# Sistemas Embebidos

Repositorio de prácticas y proyectos de la materia **Sistemas Embebidos**, desarrollados con ESP32, PlatformIO y Arduino.

## Proyecto Final

### **Proyecto Final – Control PID de temperatura con interfaz web**

🌡️ Sistema de monitoreo y control de temperatura mediante ESP32 y una interfaz HMI web.  
ESP32 · PlatformIO · Arduino · Wi-Fi · HTML · CSS · JavaScript

### Descripción del proyecto

Este proyecto consiste en desarrollar un sistema de control de temperatura basado en un microcontrolador ESP32 y un controlador proporcional, integral y derivativo (PID). La temperatura se mide mediante un sensor digital DS18B20 y se procesa utilizando un filtro de promedio móvil de cinco lecturas.

El sistema incorpora un servomotor como actuador de control y un ventilador accionado mediante un módulo relevador. El controlador PID calcula la acción de control a partir del error entre la temperatura medida y el valor de referencia (setpoint).

Para la supervisión y configuración se desarrolla una interfaz HMI web alojada directamente en el ESP32. La conexión se realiza mediante una red Wi-Fi creada por el microcontrolador, por lo que es posible acceder a la página desde un celular o computadora sin necesidad de un router externo.

La interfaz permite visualizar en tiempo real la temperatura actual, el setpoint, el error, la posición del servomotor y el estado del ventilador. También incluye una gráfica de evolución de la temperatura y controles para modificar las ganancias Kp, Ki y Kd, así como el setpoint, sin modificar directamente el programa.

### Objetivo general

Desarrollar un sistema de control de temperatura basado en ESP32 mediante un controlador PID y una interfaz HMI web, que permita supervisar las variables del proceso y modificar los parámetros del controlador de manera remota.

### Objetivos específicos

🌡️ Realizar la medición de temperatura mediante el sensor DS18B20.

🧮 Implementar un controlador PID discreto con acciones proporcional, integral y derivativa.

📊 Aplicar un filtro de promedio móvil para suavizar las mediciones de temperatura.

⚙️ Controlar la posición de un servomotor mediante la salida del controlador PID.

🔌 Implementar el accionamiento de un ventilador mediante un módulo relevador.

📡 Configurar el ESP32 como punto de acceso Wi-Fi para permitir la conexión de dispositivos externos.

🌐 Desarrollar una interfaz HMI web utilizando HTML, CSS y JavaScript.

📈 Visualizar la temperatura, el setpoint y la evolución del proceso mediante una gráfica.

🎛️ Permitir la modificación de Kp, Ki, Kd y SP desde la página web.

🔄 Establecer comunicación entre la interfaz web y el ESP32 mediante solicitudes HTTP.

### Materiales y herramientas

- ESP32 DevKit V1
- Sensor digital de temperatura DS18B20
- Resistencia pull-up de 4.7 kΩ
- Servomotor
- Módulo relevador de un canal
- Ventilador
- Pantalla o dispositivo para acceder a la interfaz web
- Protoboard y cables de conexión
- Computadora
- Visual Studio Code
- PlatformIO
- Framework Arduino
- HTML, CSS y JavaScript
- Librerías OneWire, DallasTemperature, ESP32Servo y WiFi

### Integrantes

- Jesús Ángel Bonilla Durán — Matrícula: 24033407
- Viridiana Santibañez Carrillo — Matrícula: 24033066
- Jocelyn Rodriguez Torres — Matrícula: 24033436

**Asignatura:** Sistemas Embebidos

**Profesor:** Hugo Antonio Méndez Guzmán