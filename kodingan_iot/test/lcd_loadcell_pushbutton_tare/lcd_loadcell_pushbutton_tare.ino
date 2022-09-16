#include "HX711.h"
#include <LiquidCrystal_I2C.h>

const int tareButton = 3 /* pin RX */;

const int LCD_I2C_SCL_PIN = 5; /* pin D1 */
const int LCD_I2C_SDA_PIN = 4; /* pin D2 */

const int LOAD_CELL_DOUT = 13; /* pin D7 */
const int LOAD_CELL_CLK = 12; /* pin D6 */

HX711 scale(LOAD_CELL_DOUT, LOAD_CELL_CLK);
float calibration_factor = 691535;

// definisi dimensi LCD Liquid Crystal
const int jumlahKolomLCD = 16;
const int jumlahBarisLCD = 2;

// variabel variabel dibawah digunakan untuk mengatur klik dari push button ayakan
// supaya bisa dipakai sebagai switch on off ayakan
int tarePushButtonState = 0;
int tareLastButtonState;
int tareNewButtonValue;

int reading = 0;

int timerDelay = 1000;
unsigned long lastTime = 0;

LiquidCrystal_I2C lcd(0x27, jumlahKolomLCD, jumlahBarisLCD);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(tareButton, INPUT_PULLUP);

  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  // Inisiasi LCD dan menyimpan ikon pada memori
  lcd.init();
  lcd.backlight();

  scale.set_scale();
  lcd.print("Doing Tare...");
  scale.tare(); // Reset berat ke 0
  lcd.print("Tare Successfully...");

  delay(2000);

  lcd.clear();
  lcd.home();
  lcd.setCursor(4,0);
  lcd.print("Reading:");
}

void loop() {
  // awal blok program untuk menerima input push button tare
  // \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  tareNewButtonValue = digitalRead(tareButton);
  if(tareNewButtonValue == 1 && tareLastButtonState == 0) {
    scale.tare();
  }
  delay(100);
  tareLastButtonState = tareNewButtonValue;
  // ^^^^^^^^^^^^^^^^^^^^^^^
  // akhir blok program untuk menerima input push button tare

  // Request dilakukan setiap setengah detik
  // \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  if ((millis() - lastTime) > timerDelay) {
    scale.set_scale(calibration_factor);
    reading = scale.get_units() * 1000;

    if(reading > 1 && reading < 99999 || reading == 0) {
      lcd.setCursor(4,1);
      lcd.print(reading);
      lcd.print(" gr");
      lcd.print("            ");
    }
    lastTime = millis();
  }


}
