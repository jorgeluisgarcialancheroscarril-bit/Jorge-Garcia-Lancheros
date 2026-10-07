// Prueba de LCD I2C 16x2. Si no se ve nada, ajusta el contraste (potenciometro del modulo)
// o prueba la direccion 0x3F en lugar de 0x27.
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Fertilizate - TEST");
  lcd.setCursor(0, 1);
  lcd.print("LCD OK");
}

void loop() {
  lcd.setCursor(10, 1);
  lcd.print(millis() / 1000);
  lcd.print("s   ");
  delay(500);
}
