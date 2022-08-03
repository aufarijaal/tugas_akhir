#incude "HX711.h"

// Pin definition
const int pinBtnPutih = 3; /* pin RX */
const int pinBtnMerah = 15; /* pin D8 */
const int LOAD_CELL_DOUT = 13; /* pin D7 */
const int LOAD_CELL_CLK = 12; /* pin D6 */


int btnPutihState = 0;
int lastBtnPutihState;
int newBtnPutihValue;

int btnMerahState = 0;
int lastBtnMerahState;
int newBtnMerahValue;


// Object definition
HX711 scale;
float calibration_factor = 691535;

float reading = 0;

void setup() {
  Serial.begin(9600);

  scale.begin(LOAD_CELL_DOUT, LOAD_CELL_CLK);
  scale.set_scale();
  scale.tare();


  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  pinMode(pinBtnPutih, INPUT);
  // pinMode(pinBtnPutih, INPUT_PULLUP);
  pinMode(pinBtnMerah, INPUT);
  // pinMode(pinBtnMerah, INPUT_PULLUP);
}

void loop() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  reading = scale.get_units() * 1000;
  Serial.print("Reading: ");
  //  Serial.print(scale.get_units(), 3);
  if(reading <= 2) {
    Serial.print(0);
  }else {
    Serial.print(reading, 0);
  }
  //  Serial.print(" kg");
  Serial.print(" gr");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();


  newBtnPutihValue = digitalRead(pinBtnPutih);
  if(newBtnPutihValue == 1 && lastBtnPutihState == 0) {
    scale.tare();
    Serial.println("Button Putih clicked");
  }
  delay(100);
  lastBtnPutihState = newBtnPutihValue;

  newBtnMerahValue = digitalRead(pinBtnMerah);
  if(newBtnMerahValue == 1 && lastBtnMerahState == 0) {
    Serial.println("Button Merah clicked");
  }
  delay(100);
  lastBtnMerahState = newBtnMerahValue;
}