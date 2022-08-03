#include "DHT.h"

#define DHTTYPE DHT11
// Pin definition
const int pinDHT = 2; /* pin D4 */

// Object definition
DHT dht(pinDHT, DHTTYPE);
float suhu;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  suhu = dht.readTemperature();
  Serial.print("Suhu: ");
  Serial.println(suhu);
  delay(1000);
}