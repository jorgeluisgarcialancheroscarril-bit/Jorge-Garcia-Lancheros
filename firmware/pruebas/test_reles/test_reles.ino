// Prueba de reles SIN MOTORES CONECTADOS A POTENCIA.
// Debes escuchar el "clic" de cada rele en secuencia.
const uint8_t PIN_RELE_TRIT = 4;
const uint8_t PIN_RELE_VOLT = 5;
const uint8_t RELE_ON  = LOW;   // invierte si tu modulo es activo en HIGH
const uint8_t RELE_OFF = HIGH;

void setup() {
  pinMode(PIN_RELE_TRIT, OUTPUT);
  pinMode(PIN_RELE_VOLT, OUTPUT);
  digitalWrite(PIN_RELE_TRIT, RELE_OFF);
  digitalWrite(PIN_RELE_VOLT, RELE_OFF);
  Serial.begin(9600);
  Serial.println(F("Prueba de reles"));
}

void loop() {
  Serial.println(F("Rele TRITURACION ON"));
  digitalWrite(PIN_RELE_TRIT, RELE_ON);  delay(1500);
  digitalWrite(PIN_RELE_TRIT, RELE_OFF); delay(1000);

  Serial.println(F("Rele VOLTEO ON"));
  digitalWrite(PIN_RELE_VOLT, RELE_ON);  delay(1500);
  digitalWrite(PIN_RELE_VOLT, RELE_OFF); delay(1000);
}
