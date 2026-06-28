#include <Wire.h>
#include <WiFi.h>
#include "ThingSpeak.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_MPU6050 mpu;

const char* ssid = "Wokwi-GUEST";
const char* password = "";

unsigned long channelID = 3417900;
const char *writeAPIKey = "KTUE5MLZKXLP77UQ";

WiFiClient client;

void setup() {

  Serial.begin(115200);

  Wire.begin(21,22);

  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C))
  {
    Serial.println("OLED Failed");
    while(1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(20,20);
  display.println("Starting...");
  display.display();

  if(!mpu.begin())
  {
    Serial.println("MPU6050 Failed");

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("MPU6050 Error");
    display.display();

    while(1);
  }

  Serial.println("MPU6050 Ready");

  WiFi.begin(ssid,password);

  Serial.print("Connecting");

  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi Connected");
  display.display();

  ThingSpeak.begin(client);

  delay(2000);
}

void loop() {

  sensors_event_t a,g,temp;

  mpu.getEvent(&a,&g,&temp);

  display.clearDisplay();

  display.setCursor(0,0);
  display.println("Position Monitor");

  display.setCursor(0,12);
  display.print("Ax:");
  display.print(a.acceleration.x,2);

  display.setCursor(64,12);
  display.print("Ay:");
  display.print(a.acceleration.y,2);

  display.setCursor(0,24);
  display.print("Az:");
  display.print(a.acceleration.z,2);

  display.setCursor(0,38);
  display.print("Gx:");
  display.print(g.gyro.x,2);

  display.setCursor(64,38);
  display.print("Gy:");
  display.print(g.gyro.y,2);

  display.setCursor(0,52);
  display.print("Gz:");
  display.print(g.gyro.z,2);

  display.display();

  Serial.println("---------------------------");

  Serial.print("Ax: ");
  Serial.println(a.acceleration.x);

  Serial.print("Ay: ");
  Serial.println(a.acceleration.y);

  Serial.print("Az: ");
  Serial.println(a.acceleration.z);

  Serial.print("Gx: ");
  Serial.println(g.gyro.x);

  Serial.print("Gy: ");
  Serial.println(g.gyro.y);

  Serial.print("Gz: ");
  Serial.println(g.gyro.z);

  ThingSpeak.setField(1,a.acceleration.x);
  ThingSpeak.setField(2,a.acceleration.y);
  ThingSpeak.setField(3,a.acceleration.z);
  ThingSpeak.setField(4,g.gyro.x);
  ThingSpeak.setField(5,g.gyro.y);
  ThingSpeak.setField(6,g.gyro.z);

  int response = ThingSpeak.writeFields(channelID,writeAPIKey);

  if(response == 200)
  {
    Serial.println("Data uploaded to ThingSpeak");
  }
  else
  {
    Serial.print("Upload failed. Error Code: ");
    Serial.println(response);
  }

  delay(20000);
}