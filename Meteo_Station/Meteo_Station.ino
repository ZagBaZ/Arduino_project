#include <GyverOLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_AHT10.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "hex_images.h"

GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
Adafruit_AHT10 aht;

const char* ssid     = "<login Wi-Fi>";
const char* password = "<password Wi-Fi>";
const String server  = "api.openweathermap.org";
const String lat     = "55.45";
const String lon     = "37.36";
const String appid   = "<tokenAPI>";
const String url     = "http://api.openweathermap.org/data/2.5/weather?id=524901&appid=<tokenApi>&units=metric";

unsigned long lastConnectionTime = 30000;
unsigned long postingInterval = 60000;

String httpData;

struct weather_structure {
  unsigned int id;
  const char* main;
  const char* icon;
  const char* descript;
  const char* city;
  int temp;
  int temp_min;
  int temp_max;
  int pressure;
  byte  humidity;
  float speed;
  int deg;
};
weather_structure weather;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, 10800);

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("\nconnected!");
}


void setup() {
  Serial.begin(115200);
  aht.begin();
  oled.init();  
  oled.clear();  
  oled.setCursor(20, 1);   
  oled.setScale(2);
  oled.print("ZAG INC.");
  oled.setCursor(30, 3);   
  oled.setScale(1);
  oled.print("соединение...");
  oled.update();
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  connect();
  timeClient.begin();
  
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}


void oled_temp() {
  oled.clear();
  oled.update();
  
  String icon = weather.icon;
  if (icon == "01d") oled.drawBitmap(12, 0, sunny_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else if (icon == "01n") oled.drawBitmap(12, 0, moon_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else if (icon == "02d") oled.drawBitmap(12, 0, clouds_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else if (icon == "02d") oled.drawBitmap(12, 0, clouds_moon_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else if (icon == "03d" || "03n" || "04d" || "04n") oled.drawBitmap(12, 0, tuchi_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else if (icon == "09d" || "09n" || "10d" || "10n") oled.drawBitmap(12, 0, rainy_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else if (icon == "11d" || "11n") oled.drawBitmap(12, 0, thunder_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else if (icon == "13d" || "13n") oled.drawBitmap(12, 0, snow_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else if (icon == "50d" || "50n") oled.drawBitmap(12, 0, mist_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  else oled.drawBitmap(12, 0, gradusnik_32x32, 32, 32, BITMAP_INVERT, BUF_ADD); 

  oled.setCursor(70, 0); 
  oled.setScale(2);
  if (weather.temp > 0) oled.print("+");
  oled.print(weather.temp);
  oled.setScale(2);
  oled.print(" C");
  oled.setCursor(65, 3);
  oled.setScale(1);
  if (weather.temp_min > 0) oled.print("+");
  oled.print(weather.temp_min);
  oled.print(" - "); 
  if (weather.temp_max > 0) oled.print("+");
  oled.print(weather.temp_max);
  oled.update();

  delay(10000);
   
  oled.clear();
  oled.drawBitmap(12, 0, hum_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  oled.setCursor(70, 1);
  oled.setScale(2);
  oled.print(weather.humidity);
  oled.setScale(2);
  oled.print(" %");
  oled.update();

  delay(10000);

  oled.clear();
  oled.drawBitmap(12, 0, pressure_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  oled.setCursor(55, 1);
  oled.setScale(2);
  oled.print(int(weather.pressure * 0.750063755419211));
  oled.setScale(1);
  oled.print(" mmHg");
  oled.update();

  delay(10000);

  oled.clear();  
  oled.drawBitmap(10, 0, wind_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  oled.setCursor(60, 0);   
  oled.setScale(2);
  oled.print(weather.speed);
  oled.setScale(1);
  oled.print(" м/с");
  oled.setCursor(65, 2);
  oled.setScale(2);
  if (weather.deg < 23 ) oled.print("СЕВ");
  else if (weather.deg < 68 ) oled.print("С-В");
  else if (weather.deg < 113 ) oled.print("ВОС");
  else if (weather.deg < 158 ) oled.print("Ю-В");
  else if (weather.deg < 203 ) oled.print("ЮЖ");
  else if (weather.deg < 248 ) oled.print("Ю-З");
  else if (weather.deg < 293 ) oled.print("ЗАП");
  else if (weather.deg < 338 ) oled.print("С-З");
  else oled.print("СЕВ");
  oled.update();
  
  delay(10000);

  oled.clear();   // очистить дисплей (или буфер)
  oled.drawBitmap(12, 0, home_32x32, 32, 32, BITMAP_INVERT, BUF_ADD);
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  int home_h = humidity.relative_humidity; 
  int home_t = temp.temperature;
  Serial.println(home_h);
  Serial.println(home_t);
  oled.setCursor(70, 0);   // курсор в (пиксель X, строка Y)
  oled.setScale(2);
  if (home_t == 2147483647 ) oled.print("err");
  else oled.print(home_t);
  
  oled.print(" C");
  oled.circle(100, 1, 1, OLED_STROKE);        // окружность с центром в (x,y, с радиусом)
  oled.setCursor(70, 2);   // курсор в (пиксель X, строка Y)
  oled.setScale(2);
  if (home_h == 2147483647 ) oled.print("err");
  else oled.print(home_h);  
  oled.print(" %");
  oled.update();

  delay(10000);

  oled.clear();   
  timeClient.update();
  oled.setCursor(38, 0);   
  oled.setScale(2);
  if (timeClient.getHours() < 10) oled.print("0");
  oled.print(timeClient.getHours());
  oled.print(":");
  if (timeClient.getMinutes() < 10) oled.print("0");
  oled.print(timeClient.getMinutes());

  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  String currentDate = String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear);
  Serial.println(currentDate);   
  oled.setCursor(35, 2);
  oled.setScale(1);
  oled.print(currentDate);
  oled.print("г.");

  oled.setCursor(40, 3);
  oled.setScale(1);
  if (timeClient.getDay() == 0) oled.print("Воскресенье");
  else if (timeClient.getDay() == 1) oled.print("Понедельник");
  else if (timeClient.getDay() == 2) oled.print("Вторник");
  else if (timeClient.getDay() == 3) oled.print("Среда");
  else if (timeClient.getDay() == 4) oled.print("Четверг");
  else if (timeClient.getDay() == 5) oled.print("Пятница");
  else if (timeClient.getDay() == 6) oled.print("Суббота");
  else {
    oled.setCursor(10, 3);   
    oled.print("Обрыв связи с сервером");
  } 
  oled.update();

  delay(10000);
}

void loop() {
  if (WiFi.status() == !WL_CONNECTED) connect();
  if (WiFi.isConnected() == false ) {
    Serial.print("Разрыв");
        oled.clear();
        oled.update();
        oled.setCursor(15, 1);   
        oled.print("...обрыв связи...");
        oled.update();
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    timeClient.begin();
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
      if (httpRequest() and parseData()) {
        Serial.println("\nWeather");
        Serial.printf("city: %s\n", weather.city);
        Serial.printf("id: %d\n", weather.id);
        Serial.printf("main: %s\n", weather.main);
        Serial.printf("description: %s\n", weather.descript);
        Serial.printf("icon: %s\n", weather.icon);
        Serial.printf("temp: ");Serial.println(weather.temp);
        Serial.printf("humidity: "); Serial.println(weather.humidity);
        Serial.printf("pressure: "); Serial.println(weather.pressure * 0.750063755419211);
        Serial.printf("wind's speed: "); Serial.println(weather.speed);
        Serial.printf("wind's direction "); Serial.println(weather.deg);
        Serial.println();      
    }
    oled_temp(); 
    }
  }


bool httpRequest() {
  WiFiClient clientwifi;
  HTTPClient client;
  bool find = false;
  //client.setTimeout(1000);
  Serial.print("Connecting ");
  client.begin(clientwifi, url);
  int httpCode = client.GET();

  if (httpCode > 0) {
    Serial.printf("successfully, code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      httpData = client.getString();
      if (httpData.indexOf(F("\"main\":{\"temp\":")) > -1) {
        lastConnectionTime = millis();
        find = true;
      }
      else Serial.println("Failed, json string is not found");
    }
  }
  else Serial.printf("failed, error: %s\n", client.errorToString(httpCode).c_str());

  postingInterval = find ? 600L * 1000L : 60L * 1000L;
  client.end();

  return find;
}

bool parseData() {
  Serial.println(httpData);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(httpData);

  if (!root.success()) {
    Serial.println("Json parsing failed!");
    return false;
  }
  weather.city     = root["name"];
  weather.id       = root["weather"][0]["id"];
  weather.main     = root["weather"][0]["main"];
  weather.descript = root["weather"][0]["description"];
  weather.icon     = root["weather"][0]["icon"];
  weather.temp     = root["main"]["temp"];
  weather.temp_min = root["main"]["temp_min"];
  weather.temp_max = root["main"]["temp_max"];
  weather.humidity = root["main"]["humidity"];
  weather.pressure = root["main"]["pressure"];
  weather.speed    = root["wind"]["speed"];
  weather.deg      = root["wind"]["deg"];

  httpData = "";
  return true;
}

 
