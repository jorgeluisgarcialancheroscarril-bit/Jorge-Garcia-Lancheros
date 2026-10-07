// Prueba de entradas de seguridad y pulsador. Abre el monitor serie a 9600.
// Verifica: tapa cerrada = CERRADA, paro normal = NORMAL, pulsador = PRESIONADO al pulsar.
const uint8_t PIN_START = 2;
const uint8_t PIN_TAPA  = 3;
const uint8_t PIN_PARO  = 6;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_TAPA, INPUT_PULLUP);
  pinMode(PIN_PARO, INPUT_PULLUP);
}

void loop() {
  Serial.print(F("TAPA: "));
  Serial.print(digitalRead(PIN_TAPA) == LOW ? F("CERRADA") : F("ABIERTA"));
  Serial.print(F(" | PARO: "));
  Serial.print(digitalRead(PIN_PARO) == HIGH ? F("ACTIVADO") : F("NORMAL"));
  Serial.print(F(" | INICIO: "));
  Serial.println(digitalRead(PIN_START) == LOW ? F("PRESIONADO") : F("-"));
  delay(300);
}
