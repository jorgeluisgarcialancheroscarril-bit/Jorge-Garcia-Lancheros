/*
 * EcoAbono - Firmware principal (Arduino UNO / Nano)
 * Maquina semiautomatica para procesar residuos organicos.
 *
 * Maquina de estados:
 *   REPOSO -> VERIFICACION -> TRITURACION -> MEZCLA -> MONITOREO <-> AJUSTE
 *   MONITOREO -> DESCARGA -> REPOSO        (cualquier estado -> EMERGENCIA)
 *
 * Librerias: LiquidCrystal_I2C, OneWire, DallasTemperature (ver ../LIBRERIAS.md)
 *
 * IMPORTANTE: todos los tiempos y umbrales son valores iniciales de
 * referencia. Ajustalos con tus pruebas y registralos en /data.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ===================== PINES =====================
const uint8_t PIN_START      = 2;   // Pulsador de inicio (a GND)
const uint8_t PIN_TAPA       = 3;   // Final de carrera de tapa (cerrada = LOW)
const uint8_t PIN_RELE_TRIT  = 4;   // Rele motor de trituracion
const uint8_t PIN_RELE_VOLT  = 5;   // Rele motor de mezcla/volteo
const uint8_t PIN_PARO       = 6;   // Paro de emergencia NC (normal = LOW)
const uint8_t PIN_ONEWIRE    = 7;   // DS18B20
const uint8_t PIN_LED_VERDE  = 8;
const uint8_t PIN_LED_AMAR   = 9;
const uint8_t PIN_LED_ROJO   = 10;
const uint8_t PIN_BUZZER     = 11;
const uint8_t PIN_HUMEDAD    = A0;  // Sensor capacitivo de humedad

// Modulo rele tipico: se activa con LOW. Si el tuyo es activo en HIGH, invierte.
const uint8_t RELE_ON  = LOW;
const uint8_t RELE_OFF = HIGH;

// ===================== PARAMETROS (AJUSTAR) =====================
const unsigned long T_TRITURACION_S   = 120;   // s de trituracion
const unsigned long T_MEZCLA_INIC_S   = 60;    // s de mezcla inicial
const unsigned long T_CICLO_MIN       = 60;    // min de duracion del ciclo de monitoreo
const unsigned long INTERVALO_LECT_S  = 5;     // s entre lecturas de sensores
const unsigned long INTERVALO_VOLT_S  = 300;   // s entre volteos
const unsigned long DURACION_VOLT_S   = 30;    // s por volteo

const float HUM_MIN  = 50.0;   // % humedad minima de la mezcla
const float HUM_MAX  = 60.0;   // % humedad maxima de la mezcla
const float TEMP_MAX = 65.0;   // C: sobre este valor se aumenta la aireacion

// Calibracion del sensor capacitivo (leer valores en aire y en agua)
const int RAW_SECO   = 520;    // lectura en aire (0 %)
const int RAW_MOJADO = 260;    // lectura en agua (100 %)

// ===================== OBJETOS =====================
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Direccion tipica: 0x27 o 0x3F
OneWire oneWire(PIN_ONEWIRE);
DallasTemperature ds18b20(&oneWire);

// ===================== ESTADOS =====================
enum Estado { REPOSO, VERIFICACION, TRITURACION, MEZCLA, MONITOREO, AJUSTE, DESCARGA, EMERGENCIA };
Estado estado = REPOSO;

unsigned long tEstado = 0;       // inicio del estado actual
unsigned long tCicloInicio = 0;  // inicio del monitoreo (compensado por pausas)
unsigned long tUltLectura = 0;
unsigned long tUltVolteo = 0;
unsigned long tVolteoInicio = 0;
unsigned long tUltLCD = 0;
unsigned long tAjusteInicio = 0;
bool volteando = false;

float temperatura = 0.0;
float humedad = 0.0;
String mensajeAjuste = "";

// ===================== UTILIDADES =====================
void reles(bool trit, bool volt) {
  digitalWrite(PIN_RELE_TRIT, trit ? RELE_ON : RELE_OFF);
  digitalWrite(PIN_RELE_VOLT, volt ? RELE_ON : RELE_OFF);
}

void apagarMotores() { reles(false, false); volteando = false; }

void leds(bool v, bool a, bool r) {
  digitalWrite(PIN_LED_VERDE, v);
  digitalWrite(PIN_LED_AMAR, a);
  digitalWrite(PIN_LED_ROJO, r);
}

void pitar(unsigned int ms) { tone(PIN_BUZZER, 2000, ms); }

bool tapaCerrada()     { return digitalRead(PIN_TAPA) == LOW; }
bool paroActivo()      { return digitalRead(PIN_PARO) == HIGH; }   // NC: abierto = activado
bool startPresionado() { return digitalRead(PIN_START) == LOW; }

// Antirrebote simple para el pulsador de inicio (detecta el flanco de pulsacion)
bool startFlanco() {
  static bool previo = false;
  static unsigned long tUlt = 0;
  bool ahora = startPresionado();
  if (ahora != previo && millis() - tUlt > 40) {
    tUlt = millis();
    previo = ahora;
    if (ahora) return true;
  }
  return false;
}

float leerHumedad() {
  long suma = 0;
  for (int i = 0; i < 10; i++) { suma += analogRead(PIN_HUMEDAD); delay(2); }
  float raw = suma / 10.0;
  float pct = (RAW_SECO - raw) * 100.0 / (RAW_SECO - RAW_MOJADO);
  return constrain(pct, 0.0, 100.0);
}

float leerTemperatura() {
  ds18b20.requestTemperatures();
  float t = ds18b20.getTempCByIndex(0);
  if (t == DEVICE_DISCONNECTED_C) return -127.0;
  return t;
}

void cambiarEstado(Estado nuevo) {
  estado = nuevo;
  tEstado = millis();
  lcd.clear();
  Serial.print(F("# ESTADO -> "));
  Serial.println((int)nuevo);
}

void lcdLinea(uint8_t fila, const String &texto) {
  String s = texto;
  while (s.length() < 16) s += ' ';
  lcd.setCursor(0, fila);
  lcd.print(s.substring(0, 16));
}

String mmss(unsigned long ms) {
  unsigned long s = ms / 1000UL;
  char b[8];
  snprintf(b, sizeof(b), "%02lu:%02lu", s / 60, s % 60);
  return String(b);
}

// ===================== SEGURIDAD =====================
// Se evalua en cada iteracion del loop.
void vigilarSeguridad() {
  if (estado == REPOSO || estado == EMERGENCIA) return;

  if (paroActivo()) {
    apagarMotores();
    cambiarEstado(EMERGENCIA);
    return;
  }
  // Tapa abierta durante un ciclo: detener y abortar
  if (!tapaCerrada() && estado != DESCARGA) {
    apagarMotores();
    pitar(500);
    cambiarEstado(REPOSO);
    lcdLinea(0, "TAPA ABIERTA");
    lcdLinea(1, "Ciclo detenido");
    delay(1500);
  }
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(9600);

  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_TAPA, INPUT_PULLUP);
  pinMode(PIN_PARO, INPUT_PULLUP);
  pinMode(PIN_RELE_TRIT, OUTPUT);
  pinMode(PIN_RELE_VOLT, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_AMAR, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  apagarMotores();   // Reles apagados desde el arranque

  lcd.init();
  lcd.backlight();
  ds18b20.begin();

  lcdLinea(0, "EcoAbono v1.0");
  lcdLinea(1, "Iniciando...");
  delay(1500);
  lcd.clear();

  Serial.println(F("# EcoAbono listo"));
  Serial.println(F("t_s,estado,temp_C,humedad_pct,volteando"));
  cambiarEstado(REPOSO);
}

// ===================== LOOP =====================
void loop() {
  vigilarSeguridad();
  unsigned long ahora = millis();

  switch (estado) {

    case REPOSO:
      apagarMotores();
      leds(true, false, false);
      if (ahora - tUltLCD > 500) {
        tUltLCD = ahora;
        lcdLinea(0, "EcoAbono: LISTO");
        lcdLinea(1, tapaCerrada() ? "Pulse INICIO" : "Cierre la tapa");
      }
      if (startFlanco()) cambiarEstado(VERIFICACION);
      break;

    case VERIFICACION:
      leds(false, true, false);
      if (paroActivo()) {
        lcdLinea(0, "PARO ACTIVO");
        lcdLinea(1, "Rearme el paro");
        pitar(300); delay(1200);
        cambiarEstado(REPOSO);
      } else if (!tapaCerrada()) {
        lcdLinea(0, "ALERTA: TAPA");
        lcdLinea(1, "Cierre la tapa");
        pitar(300); delay(1200);
        cambiarEstado(REPOSO);
      } else {
        lcdLinea(0, "Seguridad OK");
        delay(600);
        reles(true, false);          // Arranca trituracion
        cambiarEstado(TRITURACION);
      }
      break;

    case TRITURACION:
      leds(false, true, false);
      if (ahora - tUltLCD > 500) {
        tUltLCD = ahora;
        unsigned long resta = T_TRITURACION_S * 1000UL - (ahora - tEstado);
        lcdLinea(0, "Triturando...");
        lcdLinea(1, "Resta " + mmss(resta));
      }
      if (ahora - tEstado >= T_TRITURACION_S * 1000UL) {
        reles(false, true);          // Pasa a mezcla
        cambiarEstado(MEZCLA);
      }
      break;

    case MEZCLA:
      leds(false, true, false);
      if (ahora - tUltLCD > 500) {
        tUltLCD = ahora;
        unsigned long resta = T_MEZCLA_INIC_S * 1000UL - (ahora - tEstado);
        lcdLinea(0, "Mezclando...");
        lcdLinea(1, "Resta " + mmss(resta));
      }
      if (ahora - tEstado >= T_MEZCLA_INIC_S * 1000UL) {
        apagarMotores();
        tCicloInicio = ahora;
        tUltLectura = 0;
        tUltVolteo = ahora;
        cambiarEstado(MONITOREO);
      }
      break;

    case MONITOREO: {
      leds(true, false, false);

      // Lectura periodica de sensores
      if (ahora - tUltLectura >= INTERVALO_LECT_S * 1000UL || tUltLectura == 0) {
        tUltLectura = ahora;
        temperatura = leerTemperatura();
        humedad = leerHumedad();

        Serial.print((ahora - tCicloInicio) / 1000UL); Serial.print(',');
        Serial.print(F("MONITOREO,"));
        Serial.print(temperatura, 1); Serial.print(',');
        Serial.print(humedad, 1); Serial.print(',');
        Serial.println(volteando ? 1 : 0);

        if (humedad < HUM_MIN) {
          mensajeAjuste = "Agregar AGUA";
          apagarMotores(); tAjusteInicio = ahora; pitar(400);
          cambiarEstado(AJUSTE); break;
        }
        if (humedad > HUM_MAX) {
          mensajeAjuste = "Agregar SECO";
          apagarMotores(); tAjusteInicio = ahora; pitar(400);
          cambiarEstado(AJUSTE); break;
        }
      }

      // Volteo periodico (mas frecuente si hay sobretemperatura)
      unsigned long intervalo = INTERVALO_VOLT_S * 1000UL;
      if (temperatura > TEMP_MAX) intervalo = intervalo / 3;

      if (!volteando && ahora - tUltVolteo >= intervalo) {
        reles(false, true);
        volteando = true;
        tVolteoInicio = ahora;
      }
      if (volteando && ahora - tVolteoInicio >= DURACION_VOLT_S * 1000UL) {
        apagarMotores();
        tUltVolteo = ahora;
      }

      // Pantalla
      if (ahora - tUltLCD > 500) {
        tUltLCD = ahora;
        String l0 = "T:" + String(temperatura, 0) + "C H:" + String(humedad, 0) + "%";
        if (temperatura > TEMP_MAX) l0 += " !";
        unsigned long transc = ahora - tCicloInicio;
        unsigned long total = T_CICLO_MIN * 60000UL;
        unsigned long resta = transc >= total ? 0 : total - transc;
        lcdLinea(0, l0);
        lcdLinea(1, String(volteando ? "Aireando " : "Monit.   ") + mmss(resta));
      }

      // Fin del ciclo
      if (ahora - tCicloInicio >= T_CICLO_MIN * 60000UL) {
        apagarMotores();
        pitar(1000);
        cambiarEstado(DESCARGA);
      }
      break;
    }

    case AJUSTE: {
      leds(false, true, false);
      if (ahora - tUltLCD > 500) {
        tUltLCD = ahora;
        lcdLinea(0, mensajeAjuste);
        lcdLinea(1, "Luego pulse INICIO");
      }
      // Salida: el usuario confirma con INICIO y la humedad esta en rango
      if (startFlanco()) {
        humedad = leerHumedad();
        if (humedad >= HUM_MIN && humedad <= HUM_MAX) {
          tCicloInicio += (ahora - tAjusteInicio);  // la pausa no descuenta tiempo de ciclo
          tUltLectura = 0;
          cambiarEstado(MONITOREO);
        } else {
          pitar(200);   // Aun fuera de rango
        }
      }
      break;
    }

    case DESCARGA:
      leds(true, true, false);
      if (ahora - tUltLCD > 500) {
        tUltLCD = ahora;
        lcdLinea(0, "CICLO TERMINADO");
        lcdLinea(1, "Retire material");
      }
      if (startFlanco()) cambiarEstado(REPOSO);
      break;

    case EMERGENCIA:
      apagarMotores();
      leds(false, false, true);
      if (ahora - tUltLCD > 500) {
        tUltLCD = ahora;
        lcdLinea(0, "!! EMERGENCIA !!");
        lcdLinea(1, paroActivo() ? "Rearme el paro" : "Pulse INICIO");
      }
      // Reinicio manual: solo si el paro fue rearmado
      if (!paroActivo() && startFlanco()) cambiarEstado(REPOSO);
      break;
  }
}
