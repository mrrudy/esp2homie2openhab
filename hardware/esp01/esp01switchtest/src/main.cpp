
#include "Arduino.h"
#include "main.h"


void setup() {

  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);

}

void loop() {

  digitalWrite(0, HIGH);
  delay(1000);
  digitalWrite(0, LOW);
  delay(1000);

}
