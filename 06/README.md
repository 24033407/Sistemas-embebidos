# Sistemas Embebidos

Repositorio de prácticas de la materia **Sistemas Embebidos**, realizadas con ESP32, PlatformIO y Wokwi.

## Prácticas

* **05_Practica** *
Monitor de Voltaje y Temperatura con ESP32
⚡ Adquisición de señales analógicas y visualización en LCD 16×2 mediante I²C
ESP32 PlatformIO Arduino Wokwi License

📌 Descripción del proyecto
Este proyecto consiste en desarrollar un sistema de adquisición de señales analógicas utilizando una tarjeta ESP32, dos potenciómetros y un termistor NTC para medir variaciones de voltaje y estimar la temperatura.

Los datos adquiridos se muestran en una pantalla LCD de 16 columnas por 2 filas, utilizando el protocolo de comunicación I²C. Adicionalmente, las lecturas se envían al monitor serie para facilitar la supervisión y el análisis de las variables.

El sistema puede implementarse y probarse mediante Wokwi, integrándolo con Visual Studio Code y PlatformIO.

🎯 Objetivo general
Implementar un sistema embebido basado en ESP32 capaz de adquirir dos señales analógicas, estimar la temperatura mediante un termistor NTC y visualizar las variables medidas en una pantalla LCD con comunicación I²C.

🎓 Objetivos específicos
🔌 Configurar entradas analógicas de la ESP32 para adquirir señales eléctricas.
🎚️ Medir el voltaje de salida de dos potenciómetros.
🌡️ Estimar la temperatura a partir de la resistencia eléctrica de un termistor NTC.
🧮 Aplicar la ecuación Beta para convertir la resistencia del termistor en temperatura.
🖥️ Implementar la comunicación I²C para visualizar los datos en una pantalla LCD.
📡 Enviar las mediciones al monitor serie para su supervisión.
🧪 Validar el comportamiento del sistema mediante simulación.
🧰 Materiales y herramientas