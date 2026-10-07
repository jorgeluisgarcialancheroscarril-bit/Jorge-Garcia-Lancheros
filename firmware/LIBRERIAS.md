# Librerías necesarias (Arduino IDE → Herramientas → Administrar bibliotecas)

| Librería | Autor | Uso |
|---|---|---|
| `LiquidCrystal I2C` | Frank de Brabander | Pantalla LCD 16x2 |
| `OneWire` | Paul Stoffregen | Bus del DS18B20 |
| `DallasTemperature` | Miles Burton | Sensor de temperatura DS18B20 |

## Conexiones por defecto (`ecoabono.ino`)

| Pin | Función |
|---|---|
| D2 | Pulsador INICIO (a GND) |
| D3 | Final de carrera de tapa (cerrada = a GND) |
| D4 | Relé trituración |
| D5 | Relé mezcla/volteo |
| D6 | Paro de emergencia (contacto NC a GND) |
| D7 | DS18B20 (con resistencia pull-up de 4.7 kΩ a 5 V) |
| D8 / D9 / D10 | LED verde / amarillo / rojo (con resistencia de 220 Ω) |
| D11 | Buzzer |
| A0 | Sensor capacitivo de humedad |
| A4 / A5 | LCD I2C (SDA / SCL) |

## Orden recomendado de pruebas
1. `pruebas/test_lcd`
2. `pruebas/test_seguridad`
3. `pruebas/test_sensores` (aquí calibras `RAW_SECO` y `RAW_MOJADO`)
4. `pruebas/test_reles` (**sin motores a potencia**)
5. `ecoabono/ecoabono.ino`
