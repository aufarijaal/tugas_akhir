
// Pin definition
const int pinAyakan = 14; /* pin D5 */
const int pinMesin = 16; /* pin D0 */

int limaMenit = 300000;
unsigned long lastTime = 0;

bool produksiBerjalan = false;

void setup() {
  Serial.begin(9600);
  pinMode(pinAyakan, OUTPUT);
  pinMode(pinMesin, OUTPUT);

  digitalWrite(pinAyakan, HIGH);
  digitalWrite(pinMesin, HIGH);

  Serial.print("Mesin akan menyala dalam 2 detik");
  delay(2000);

  produksiBerjalan = true;
  digitalWrite(pinMesin, LOW);
  Serial.print("Mesin telah menyala, menyala selama 5 menit");
}

void loop() {
  if((millis() - lastTime) >= limaMenit && produksiBerjalan) {
    digitalWrite(pinAyakan, LOW);
    Serial.print("5 menit terlewati, Menyalakan ayakan");
  }

  if(millis() - lastTime >= (limaMenit * 2) && produksiBerjalan) {
    digitalWrite(pinAyakan, HIGH);
    digitalWrite(pinMesin, HIGH);
    Serial.print("10 menit terlewati, Menonaktifkan mesin dan ayakan");

    lastTime = millis();
    produksiBerjalan = false;
  }
}