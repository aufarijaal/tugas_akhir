
// Pin definition
int pinBtnMerah = 15; /* pin D8 */


int btnMerahState = 0;
int lastBtnMerahState;
int newBtnMerahValue;
String mesinState = "OFF";

void setup() {
  Serial.begin(9600);

  pinMode(pinBtnMerah, INPUT_PULLUP);
}

void loop() {
  newBtnMerahValue = digitalRead(pinBtnMerah);
  if(newBtnMerahValue == 1 && lastBtnMerahState == 0) {
    if(mesinState == "OFF") {
      Serial.println("mesin set to ON");
      mesinState = "ON";
    }
    else {
      Serial.println("mesin set to OFF");
      mesinState = "OFF";
    }
  }
  delay(100);
  lastBtnMerahState = newBtnMerahValue;
}