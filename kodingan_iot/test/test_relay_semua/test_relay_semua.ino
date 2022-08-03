
// Pin definition
const int pinAyakan = 14; /* pin D5 */
const int pinMesin = 16; /* pin D0 */

void setup() {
  pinMode(pinAyakan, OUTPUT);
  pinMode(pinMesin, OUTPUT);

  digitalWrite(pinAyakan, HIGH);
  digitalWrite(pinMesin, HIGH);
}

void loop() {
  delay(1000);
  digitalWrite(pinMesin, LOW);
  delay(5000);
  digitalWrite(pinMesin, HIGH);
  delay(1000);
  digitalWrite(pinAyakan, LOW);
  delay(5000);
  digitalWrite(pinAyakan, HIGH);
}