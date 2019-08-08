#include <Arduino.h>

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  unsigned long diameter_reading = analogRead(A1);
  Serial.print(diameter_reading);
  Serial.print("\n");
  delay(1000);
}
