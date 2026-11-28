# Práctica 6. Controlador de Temperatura ON-OFF

## Descripción
Desarrollo de un sistema de control automático de temperatura utilizando un ESP32. El sistema emplea un sensor LM35 para medir la temperatura, un teclado matricial 4×4 para configurar los límites de operación y una pantalla LCD 16×2 con comunicación I2C para visualizar los valores.

El control ON-OFF con histéresis permite activar automáticamente la calefacción o el enfriamiento dependiendo de la temperatura registrada.

## Material requerido
- ESP32 DevKit V1
- Sensor de temperatura LM35
- Teclado matricial 4×4
- Pantalla LCD 16×2 con módulo I2C
- LEDs o módulos de relevador
- PlatformIO
- Simulador Wokwi

## Funcionamiento
- Lectura de temperatura mediante el sensor LM35.
- Configuración de los límites inferior (LL) y superior (HL) mediante el teclado.
- Visualización de temperatura, límites y estado del sistema en la pantalla LCD.
- Activación automática de calefacción y enfriamiento mediante control ON-OFF con histéresis.
- Validación de los valores ingresados para evitar configuraciones incorrectas.

## Configuración del teclado
| Tecla | Función |
|---|---|
| `*` | Configurar límite inferior (LL) |
| `B` | Configurar límite superior (HL) |
| `D` | Guardar configuración |
| `C` | Cancelar configuración |
| `0–9` | Ingresar valores |

## Control de temperatura
- **HL:** Al alcanzar el límite superior, se activa el enfriamiento.
- **LL:** Al alcanzar el límite inferior, se activa la calefacción.
- **Histéresis:** Entre ambos límites se conserva el estado anterior del sistema.
