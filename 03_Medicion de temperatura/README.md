# Práctica 3: Control de Temperatura 

Sistema de control automático de temperatura con ventana de histéresis configurable por el usuario mediante botones y pantalla LCD 16x2. El sistema regula la temperatura activando un sistema de enfriamiento (Celda Peltier + 3 ventiladores) o un sistema de calentamiento (Foco a 120V CA) en función de los límites configurados.

---

## 🛠️ Componentes y Hardware

| Componente | Cantidad | Función / Descripción |
| :--- | :---: | :--- |
| **Sensor DS18B20** | 1 | Sensor digital de temperatura sumergible/ambiental (OneWire). |
| **Pantalla LCD 16x2 con I2C** | 1 | Visualización de la temperatura actual y los límites $HL$ / $LL$. |
| **Push Buttons (Botones)** | 3 | Configuración del sistema (Modo/Selección, Incrementar, Decrementar). |
| **Módulo Relé (Canal 1)** | 1 | Control de encendido/apagado para la Celda Peltier y 3 ventiladores. |
| **Módulo Relé (Canal 2)** | 1 | Control de encendido/apagado para foco a 120V CA (Elemento calefactor). |
| **Microcontrolador** | 1 | Unidad de procesamiento principal. |
| **Fuente de Alimentación** | - | Alimentación para lógica, ventiladores, celda Peltier y foco. |

---

## ⚙️ Lógica de Funcionamiento (Control por Histéresis)

El sistema opera bajo una banda de histéresis definida por un **Límite Alto ($HL$)** y un **Límite Bajo ($LL$)**:

* **Modo Calentamiento ($Temp < LL$):**
  * Se activa el **Relé 2** (Foco 120V).
  * Se apaga el **Relé 1** (Peltier y ventiladores).
* **Modo Enfriamiento ($Temp > HL$):**
  * Se activa el **Relé 1** (Celda Peltier + 3 ventiladores).
  * Se apaga el **Relé 2** (Foco 120V).
* **Zona de Confort / Banda Muerta ($LL \le Temp \le HL$):**
  * Ambos relés permanecen apagados para evitar conmutaciones oscilatorias constantes y prolongar la vida útil de los actuadores.

---

## 🔘 Interfaz y Control de Usuario

El sistema cuenta con 3 botones táctiles para la configuración manual en tiempo real:

1. **Botón 1 (Selección / Modo):** Commuta entre la selección de configuración del **Límite Alto ($HL$)** y el **Límite Bajo ($LL$)**.
2. **Botón 2 ( + ):** Incrementa el valor del límite seleccionado en 1.
3. **Botón 3 ( - ):** Decrementa el valor del límite seleccionado en 1.

---

## 🖥️ Interfaz en Pantalla LCD 16x2

```text
+----------------+
| Temp: 24.5 C   |
| HL:30C  LL:20C |
+----------------+