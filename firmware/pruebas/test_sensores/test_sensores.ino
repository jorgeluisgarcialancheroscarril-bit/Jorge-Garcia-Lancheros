// Prueba y CALIBRACION de sensores.
// 1) Sensor de humedad al aire   -> anota el valor RAW como RAW_SECO
// 2) Sensor sumergido en agua    -> anota el valor RAW como RAW_MOJADO
// 3) Copia ambos valores en ecoabono.ino
#include <OneWire.h>
#include <DallasTemperature.h>

const uint8_t PIN_ONEWIRE = 7;
const uint8_t PIN_HUMEDAD = A0;

OneWire oneWire(PIN_ONEWIRE);
DallasTemperature ds18b20(&oneWire);

void setup() {
  Serial.begin(9600);
  ds18b20.begin();
  Serial.println(F("raw_humedad,temp_C"));
}

void loop() {
  long suma = 0;
  for (int i = 0; i < 20; i++) { suma += analogRead(PIN_HUMEDAD); delay(5); }
  int raw = suma / 20;

  ds18b20.requestTemperatures();
  float t = ds18b20.getTempCByIndex(0);

  Serial.print(raw);
  Serial.print(',');
  if (t == DEVICE_DISCONNECTED_C) Serial.println(F("SENSOR_DESCONECTADO"));
  else Serial.println(t, 1);
  delay(1000);
}
