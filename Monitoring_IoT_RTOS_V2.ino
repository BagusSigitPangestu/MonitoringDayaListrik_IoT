/*
 * Program by Takahada 19 Desember 2022
 * 
 * Update :
 * -> Add perintah /help untuk menampilkan list perintah yang digunakan di bot
 * -> Add key button di bot
 * -> Fix bug delay respon bot telegram
 */


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "EmonLib.h"            
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ThingerESP32.h>



LiquidCrystal_I2C lcd(0x27,16,2);

EnergyMonitor emon1;

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;


void TaskHandleThinger( void *pvParameters );
void TaskGetSensor( void *pvParameters );
void TaskHandleNewMessages( void *pvParameters );
void TaskHandleRelay( void *pvParameters );

// Konfigurasi LM35
#define ADC_VREF_mV 5000.0 
#define ADC_RESOLUTION 4095.0

// Konfigurasi Variabel pin           
#define pinTegangan 35
#define pinArus 34
#define pinSuhu 36
#define pinBuzzer 32
#define pinRelay1 19
#define pinRelay2 18

const char* ssid       = "dimas";
const char* password   = "dimas12345";

// Konfigurasi Bot token
#define BOT_TOKEN "5549025517:AAH81BI04LxP7h9rl51Ejc7Vc0A4HfLTP3g"

//Konfigurasi Thinger.io
#define USERNAME "takahada"
#define DEVICE_ID "ESP32_monitoring"
#define DEVICE_CREDENTIAL "f5M#vb8TJR#8CzQO"

// Variabel thinger.io
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

// Variabel milis
const unsigned long BOT_MTBS = 1000;
const unsigned long waktu = 500;

unsigned long waktuSebelum = 0;
unsigned long waktusebelum = 0;

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

const int ledPin = LED_BUILTIN;

int ADCval;
float milliVolt;

// Variabel pembacaan sensor
float Voltage = 0;
float Current = 0;
float Power = 0;
float Temp = 0;

// Variabel status
bool lcdState = 0;
bool relayState = false;
bool notifState = false;
bool botState = 0;
bool msgState = 0;
bool buzzerState = false;

// Variabel batas awal Kwh
int batasCut = 500;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

#include "InitialSetup.h"
#include "newMassage.h"
#include "GetMassage.h"


void setup() {
  
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  InitialSetup();

  
  xTaskCreatePinnedToCore(
    TaskHandleNewMessages,
    "TaskHandleThinger",
    7168,
    NULL,
    1,
    &Task1,
    0);
    
  delay (200);
  
  xTaskCreatePinnedToCore(
    TaskHandleThinger,
    "TaskHandleNewMessages",
    7168,
    NULL,
    1,
    &Task2,
    1);

  delay (200);

  xTaskCreatePinnedToCore(
    TaskGetSensor,
    "TaskGetSensor",
    7168,
    NULL,
    2,
    &Task3,
    1);

  delay (200);

    thing["monitoring"] >> [] (pson & out){
    out["volt"] = Voltage;
    out["current"] = Current;
    out["power"] = Power;
    out["temperature"] = Temp;
  };

}

void loop(){
 
}


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskHandleThinger(void *pvParameters){
  (void) pvParameters;

  thing["monitoring"] >> [] (pson & out){
    out["volt"] = Voltage;
    out["current"] = Current;
    out["power"] = Power;
    out["temperature"] = Temp;
  };
  
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;){
    
    thing.handle(); 
    Serial.print("Task1 berhasil ");
    Serial.println(xPortGetCoreID());
    Serial.println("");
    vTaskDelay(500);
    
  }
}

void TaskGetSensor(void *pvParameters){
  (void) pvParameters;
  
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;){
    baca_Sensor();

  if ( relayState == true  ){
    if (Power >= batasCut ){
      buzzerState = true;
      notifState = true;
      digitalWrite ( pinRelay1, LOW);
      if (buzzerState == true){
         digitalWrite ( pinBuzzer, HIGH);
      }
    }
  }
  if ( relayState == false){
    digitalWrite ( pinRelay1, HIGH);
    digitalWrite ( pinBuzzer, LOW);
  }

 if (lcdState == 0) {
    lcd.setCursor (0, 0);
    lcd.print("Volt:");
    lcd.setCursor (5 + 1, 0);
    lcd.print(Voltage);
    lcd.print(" V  ");
    lcd.setCursor (0, 1);
    lcd.print("Arus:");
    lcd.setCursor (5 + 1, 1);
    lcd.print(Current);
    lcd.print(" A    ");
    lcdState = 1;
  }
  else {
    lcd.setCursor (0, 0);
    lcd.print("Daya:");
    lcd.setCursor (5+ 1, 0);
    lcd.print(Power);
    lcd.print(" W  ");
    lcd.setCursor (0, 1);
    lcd.print("Suhu:");
    lcd.setCursor (5 + 1, 1);
    lcd.print(Temp);    
    lcd.print(" C   ");
    lcdState = 0;
  }
  
  Serial.print("Task2 berhasil ");
  Serial.println(xPortGetCoreID());
  Serial.println("");
    vTaskDelay(1000);
  }
}


void TaskHandleNewMessages(void *pvParameters){
  (void) pvParameters;
  Serial.print("Task3 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
  
  GetMassage();  
  Serial.print("Task3 berhasil ");
  Serial.println(xPortGetCoreID());
  Serial.println("");
  vTaskDelay(500);
  }
}


void baca_Sensor(){
     emon1.calcVI(20,2000);
  ADCval = analogRead(pinSuhu);
  milliVolt = ADCval * (ADC_VREF_mV / ADC_RESOLUTION);
  Voltage = emon1.Vrms;
  Current = emon1.calcIrms(1480);

  Temp = milliVolt / 10;
  Power = Voltage * Current;
}
