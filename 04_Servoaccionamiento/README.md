# Control de Atenuación Domótica (Dimmer) mediante Servomotor y Arduino

Sistema de control analógico-mecánico para la regulación de intensidad lumínica mediante un servomotor acoplado a un potenciómetro de atenuación (*dimmer*), gestionado por microcontrolador a través de dos pulsadores.

---

## 📋 Descripción del Proyecto

Este proyecto implementa una interfaz de control manual para regular la luminosidad de una lámpara incandescente o regulable mediante un enfoque híbrido mecánico-electrónico. Un microcontrolador **ESP32** lee la entrada de dos pulsadores (*push buttons*) para controlar la posición angular de un **servomotor**, el cual gira mecánicamente la perilla de un *dimmer* comercial acoplado.

### Objetivos
* **Giro a la Derecha (CW):** Incrementa el ángulo del servomotor, aumentando la luminosidad de la lámpara.
* **Giro a la Izquierda (CCW):** Disminuye el ángulo del servomotor, reduciendo o apagando la luz.
* **Límites de Seguridad:** Control de software para evitar sobrepasos mecánicos fuera del rango de 0° a 180°.

---

## 🛠️ Componentes Utilizados

| Componente | Cantidad | Descripción |
| :--- | :---: | :--- |
| **Esp32** | 1 | Microcontrolador principal para la lógica de control. |
| **Servomotor (MG996R)** | 1 | Actuador mecánico para girar la perilla del dimmer. |
| **Pulsadores / Push Buttons** | 2 | Entradas digitales (Luz + / Luz -). |
| **Módulo Dimmer / Potenciómetro** | 1 | Regulador de potencia AC acoplado al servo. |
| **Foco / Lámpara Regulable** | 1 | Carga de prueba (AC). |
| **Fuente de Alimentación / Protoboard** | 1 | Distribución de energía y cableado. |


