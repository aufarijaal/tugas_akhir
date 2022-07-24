// import library
#include "HX711.h"
#include <Servo.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// WIFI Kredensial
const char* ssid = "hpnenyong";
const char* password = "gratisanbae";

unsigned long lastTime = 0;
unsigned long lastTimeButton = 0;
unsigned long timerDelay = 500;

unsigned long timerLimaMenit = 300_000;
#define DHTTYPE DHT11   // DHT 11

WiFiClient client;
HTTPClient http;

Servo servo;

// Deklarasi pin pin
const int relay = 16; /* pin D0 */
const int ayakanPin = 14; /* pin D5 */
const int servoPin = 0; /* pin D3 */
const int ayakanPushButton = 3 /* pin RX */;
const int pushButton = 15; /* pin D8 */
const int sensorDHT = 2; /* pin D4 */

const int LCD_I2C_SCL_PIN = 5; /* pin D1 */
const int LCD_I2C_SDA_PIN = 4; /* pin D2 */

const int LOAD_CELL_DOUT = 13; /* pin D7 */
const int LOAD_CELL_CLK = 12; /* pin D6 */

HX711 scale(LOAD_CELL_DOUT, LOAD_CELL_CLK);
float calibration_factor = 691535;

// definisi dimensi LCD Liquid Crystal
const int jumlahKolomLCD = 16;
const int jumlahBarisLCD = 2;

// variabel variabel dibawah digunakan untuk mengatur klik dari push button mesin utama
// supaya bisa dipakai sebagai switch on off mesin utama
int pushButtonState = 0;
int lastButtonState;
int newButtonValue;
String relayState = "OFF";

// variabel variabel dibawah digunakan untuk mengatur klik dari push button ayakan
// supaya bisa dipakai sebagai switch on off ayakan
int ayakanPushButtonState = 0;
int ayakanLastButtonState;
int ayakanNewButtonValue;
String ayakanRelayState = "OFF";

// Definisi Sensor dan LCD
DHT dht(sensorDHT, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, jumlahKolomLCD, jumlahBarisLCD);

// Definisi ikon ikon
byte ikonOnOff[8] = {
  B00100,
  B00100,
  B10101,
  B10101,
  B10101,
  B10001,
  B10001,
  B11111
};
byte ikonSuhu[8]={
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
  };
byte pembatas[8]={
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001
  };
byte derajat[8] = {
  B01000,
  B10100,
  B01000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
void setup() {

  dht.begin();
  pinMode(relay, OUTPUT);
  pinMode(ayakanPin, OUTPUT);
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(ayakanPushButton, INPUT_PULLUP);

  scale.set_scale();
  scale.tare(); // Reset berat ke 0

  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  // meng off kan semua relay terlebih dahulu
  digitalWrite(relay, HIGH);
  digitalWrite(ayakanPin, HIGH);
  
  // Inisiasi LCD dan menyimpan ikon pada memori
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, ikonOnOff);
  lcd.createChar(1, ikonSuhu);
  lcd.createChar(2, pembatas);
  lcd.createChar(3, derajat);

  // Inisiasi Serial monitor
  Serial.begin(115200); 

  // Inisiasi servo
  servo.attach(servoPin);

  WiFi.begin(ssid, password);
  Serial.println("connecting");

  lcd.setCursor(0,0);
  lcd.print("connecting...");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());


  // awal blok program update kondisi mesin utama dan ayakan ke off pada database
  // \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  WiFiClient client;
  HTTPClient http;
  
  http.begin(client, "http://pbct.my.id/api/set-machine-power");

  // http header untuk memberi tahu server tipe dari request content
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Request parameter
  String httpRequestData = "machineid=abc123&power=0";           
  // kirim request dan masukkan response code nya ke variabel
  int httpResponseCode = http.POST(httpRequestData);

  Serial.print("menonaktifkan status mesin ketika pertama kali program jalan HTTP Response code: ");
  Serial.println(httpResponseCode);
    
  // akhiri request dan bebaskan memori
  http.end();
  // ^^^^^^^^^^^^^^^^^^^^^^^
  // akhir blok program update kondisi mesin utama dan ayakan ke off pada database
  
  // tampilkan ip ketika sudah tersambung WiFi
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("your ip:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(500);
  
  // Inisiasi tampilan ikon ke LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(byte(0));
  lcd.setCursor(0,1);
  lcd.write(byte(1));
  lcd.print(" 00 C");
  lcd.setCursor(7,0);
  lcd.write(byte(2));
  lcd.print(" Hr ini:");
  lcd.setCursor(7,1);
  lcd.write(byte(2));
  lcd.home();
}

void loop() {

  // awal blok program untuk menerima input dari push button ayakan
  // \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  ayakanNewButtonValue = digitalRead(ayakanPushButton);
  if(ayakanNewButtonValue == 1 && ayakanLastButtonState == 0) {
    if(ayakanRelayState == "OFF") {
      WiFiClient client;
      HTTPClient http;
      
      http.begin(client, "http://pbct.my.id/api/set-ayakan-power");
    
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestData = "machineid=abc123&power=1";           
      int httpResponseCode = http.POST(httpRequestData);
    
      Serial.print("Response code dari request ubah power ayakan ke ON: ");
      Serial.println(httpResponseCode);
        
      http.end();
      Serial.println("ayakan set to ON");
      ayakanRelayState = "ON";
    }else {
      WiFiClient client;
      HTTPClient http;
      
      http.begin(client, "http://pbct.my.id/api/set-ayakan-power");
    
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestData = "machineid=abc123&power=0";           
      int httpResponseCode = http.POST(httpRequestData);
    
      Serial.print("Response code dari request ubah power ayakan ke OFF: ");
      Serial.println(httpResponseCode);
        
      http.end();
      Serial.println("ayakan set to OFF");
      ayakanRelayState = "OFF";
    }
  }
  delay(100);
  ayakanLastButtonState = ayakanNewButtonValue;
  // ^^^^^^^^^^^^^^^^^^^^^^^
  // akhir blok program untuk menerima input dari push button ayakan

  // awal blok program untuk menerima input dari push button mesin utama
  // \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  newButtonValue = digitalRead(pushButton);
  if(newButtonValue == 1 && lastButtonState == 0) {
    if(relayState == "OFF") {
      WiFiClient client;
      HTTPClient http;
      
      http.begin(client, "http://pbct.my.id/api/set-machine-power");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestData = "machineid=abc123&power=1";           
      int httpResponseCode = http.POST(httpRequestData);
    
      Serial.print("Response code dari request ubah power mesin utama ke ON: ");
      Serial.println(httpResponseCode);
        
      http.end();
      Serial.println("relay set to ON");
      relayState = "ON";
      
    }else {
      WiFiClient client;
      HTTPClient http;
      
      http.begin(client, "http://pbct.my.id/api/set-machine-power");
    
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestData = "machineid=abc123&power=0";           
      int httpResponseCode = http.POST(httpRequestData);
    
      Serial.print("Response code dari request ubah power mesin utama ke OFF: ");
      Serial.println(httpResponseCode);
        
      http.end();
      Serial.println("relay set to OFF");
      relayState = "OFF";
      
    }
  }
  delay(100);
  lastButtonState = newButtonValue;
  // ^^^^^^^^^^^^^^^^^^^^^^^
  // akhir blok program untuk menerima input dari push button mesin utama


  // awal blok program untuk request server kondisi dari:
  // - Meminta Status aktif/nonaktif dari Mesin Utama
  // - Meminta Status aktif/nonaktif Status aktif dari Ayakan
  // - Update nilai suhu mesin utama ke database
  // - Meminta berat produksi bubuk cangkang telur dengan filter waktu hari ini

  // Request dilakukan setiap setengah detik
  // \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      beriInformasiBeratKeServer();
      dapatkanStatusAktifMesin();
      dapatkanStatusAktifAyakan();
      beriInformasiSuhuKeServer();
      dapatkanProduksiHariIni();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  // ^^^^^^^^^^^^^^^^^^^^^^^
  // awal blok program untuk request server kondisi dari:
}

// Fungsi untuk mengirimkann berat yang terdeteksi dari load cell ke server
void beriInformasiBeratKeServer() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  reading = scale.get_units() * 1000;

  if(reading > 1 && reading < 99999 || reading == 0) {
     WiFiClient client;
     HTTPClient http;
     
     http.begin(client, "http://192.168.43.119:8000/api/set-weight");
    //  http.begin(client, "http://pbct.my.id/api/set-weight");
   
     // http header untuk memberi tahu server tipe dari request content
     http.addHeader("Content-Type", "application/x-www-form-urlencoded");
     // Request parameter
     String httpRequestData = "machineid=abc123&berat=" + String(reading);
     Serial.println(httpRequestData);  
     // kirim request dan masukkan response code nya ke variabel
     int httpResponseCode = http.POST(httpRequestData);
   
     Serial.print("kirim berat dari load cell ke server HTTP Response code: ");
     Serial.println(httpResponseCode);
     
     // akhiri request dan bebaskan memori
     http.end(); 
   }
}
// Fungsi untuk mengirimkan request ke server untuk update
// nilai suhu mesin utama ke database
void beriInformasiSuhuKeServer() {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, "http://pbct.my.id/api/set-machine-suhu");

  float suhu = dht.readTemperature();
  
  if(!isnan(suhu)) {
    Serial.print("suhu: ");
    Serial.println(suhu);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "machineid=abc123&temperature=" + String(suhu,2); 
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode>0) {
      Serial.print("update suhu mesin ok. code: ");    
      Serial.println(http.getString());
      lcd.setCursor(2, 1);
      lcd.print(suhu, 0);
      lcd.write(byte(3));
      lcd.home();
    }
    else {
      Serial.print("Update suhu mesin error code: ");
      Serial.println(httpResponseCode);
    }
    http.end(); 
  }
}

// Fungsi untuk mengirimkan request ke server untuk meminta
// kondisi on/off dari mesin utama
// jika off maka matikan mesin utama begitu pula sebaliknya
void dapatkanStatusAktifMesin() {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, "http://pbct.my.id/api/esp-machine-state?machineid=abc123");
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    Serial.print("Status mesin didatabase: ");
    Serial.println(http.getString());
    
    lcd.setCursor(2,0);
    if(http.getString() == "0") {
      lcd.print("Off");
      relayState = "OFF";
      digitalWrite(relay, HIGH);
    }else if(http.getString() == "1"){
      lcd.print("On ");
      relayState = "ON";
      digitalWrite(relay, LOW);
    }
    
    Serial.print("Isi variabel relayState : ");
    Serial.println(relayState);
    
    lcd.home();
  }
  else {
    Serial.print("Status mesin error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Fungsi untuk mengirimkan request ke server untuk meminta
// kondisi on/off dari Ayakan
// jika off maka matikan ayakan begitu pula sebaliknya
void dapatkanStatusAktifAyakan() {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, "http://pbct.my.id/api/ayakan-state?machineid=abc123");
  
  int httpResponseCode = http.GET();
  
  
  if (httpResponseCode>0) {
    Serial.print("Status ayakan didatabase: ");
    Serial.println(http.getString());

    if(http.getString() == "0") {
      ayakanRelayState = "OFF";
      digitalWrite(ayakanPin, HIGH);
      servoTutup();
    }else if(http.getString() == "1"){
      ayakanRelayState = "ON";
      digitalWrite(ayakanPin, LOW);
      servoBuka();
    }
    
    Serial.print("Isi variabel ayakanRelayState : ");
    Serial.println(ayakanRelayState);
  }
  else {
    Serial.print("Status ayakan error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Fungsi untuk mengirimkan request ke server untuk meminta
// berat produksi bubuk cangkang telur yang dihasilkan HARI INI
void dapatkanProduksiHariIni() {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, "http://pbct.my.id/api/esp-machine-todayprod?machineid=abc123");
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    String berat = http.getString();
    Serial.print("hasil produksi hari ini didatabase: ");
    Serial.println(berat);
    
    lcd.setCursor(9,1);
    lcd.print(float(berat) / 1000);
    lcd.print(" Kg");
    lcd.home();
  }
  else {
    Serial.print("hasil produksi hari ini error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Fungsi untuk menggerakkan servo untuk MEMBUKA pintu keluar bubuk
void servoBuka() {
  servo.write(180);
}

// Fungsi untuk menggerakkan servo untuk MENUTUP pintu keluar bubuk
void servoTutup() {
  servo.write(0);
}
