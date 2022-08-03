
// Pin definition
const int pinMesin = 16; /* pin D0 */

void setup() {
  pinMode(pinMesin, OUTPUT);
  digitalWrite(pinMesin, HIGH);
}

void loop() {
  digitalWrite(pinMesin, LOW);
  delay(5000);
  digitalWrite(pinMesin, HIGH);
}