
// Pin definition
const int pinAyakan = 14; /* pin D5 */

void setup() {
  pinMode(pinAyakan, OUTPUT);
  digitalWrite(pinAyakan, HIGH);
}

void loop() {
  digitalWrite(pinAyakan, LOW);
  delay(5000);
  digitalWrite(pinAyakan, HIGH);
}