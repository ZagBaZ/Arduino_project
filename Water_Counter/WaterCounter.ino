#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "GyverTimer.h"
#include <Wire.h>

/////////////////// SETTINGS /////////////////////////////

// Wi-Fi
const char* ssid = "<login>";                                       // настройки точки доступа
const char* password = "<password>";

// MQTT
const char* mqtt_server = "<IP-address>";                              // настройки MQTT сервера
const int mqtt_port = <port>;
const char* mqtt_user = "<login>";
const char* mqtt_password = "<password>";
const bool retain_flag = false;

// WATER
GTimer HotWaterTimer;
GTimer ColdWaterTimer;
boolean HotWaterFlag = false;
boolean ColdWaterFlag = false;
bool WaterEdit = false;
const String water_topic = "water/topic";
const String water_edit_gvs = "water/edit/gvs";
const String water_edit_hvs = "water/edit/hvs";
const char* water_gvs_topic = "water/gvs";
const char* water_hvs_topic = "water/hvs";
float gvs = 1000.000;                       // переменная; счетчик горячей воды
float hvs = 2000.000;                       // переменная; счетчик холодной воды
int InPinGVS = 0;                           // импульс от ГВС - D3
int InPinHVS = 2;                           // импульс от XВС - D4

/////////////////////////////////////////////////////////


WiFiClient espClient;
PubSubClient client(espClient);

uint32_t tmr1;


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {

  String data_pay;
  for (int i = 0; i < length; i++) {
    data_pay += String((char)payload[i]);
  }

  if( (String(topic) == water_edit_gvs) && (WaterEdit) ){     // редактирование показаний ГВС
    char GvsStr1[10];
    data_pay.toCharArray(GvsStr1, data_pay.length());         // перевод string во float  
    float x = atof(GvsStr1);
        gvs = x;
    }
  if( (String(topic) == water_edit_hvs) && (WaterEdit) ){     // редактирование показаний ХВС
    char HvsStr1[10];
    data_pay.toCharArray(HvsStr1, data_pay.length());         // перевод string во float
    float y = atof(HvsStr1);
        hvs = y;
    }
  if( String(topic) == water_topic ){                         // кнопка "редактирование показаний"
        if(data_pay == "ON" || data_pay == "1") WaterEdit = true;
        if(data_pay == "OFF" || data_pay == "0") WaterEdit = false;
    }     
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266-" + WiFi.macAddress();
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password) ) {
      Serial.println("connected");
      
      client.subscribe( (water_topic + "/#").c_str() );
      client.subscribe( (water_edit_gvs + "/#").c_str() );
      client.subscribe( (water_edit_hvs + "/#").c_str() );
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void sendData() {                                                        // отправка данных на mqtt-сервер
  client.publish(water_gvs_topic, String(gvs, 3).c_str(), retain_flag);
  client.publish(water_hvs_topic, String(hvs, 3).c_str(), retain_flag);
  }


void water() {
  if (HotWaterTimer.isReady()) HotWaterFlag = false;
  if (ColdWaterTimer.isReady()) ColdWaterFlag = false;
  if ((digitalRead(InPinGVS) == LOW) && (!HotWaterFlag)) {               // если есть импульс от ГВС  
    gvs = gvs + 0.001;
    HotWaterTimer.setTimeout(4000);                                     
    HotWaterFlag = true;
  }
  if ((digitalRead(InPinHVS) == LOW) && (!ColdWaterFlag))  {             // если есть импульс от ГВС  
    hvs = hvs + 0.001;
    ColdWaterTimer.setTimeout(4000); 
    ColdWaterFlag = true;
  }  
}


void setup() {
  Serial.begin(115200);
  pinMode(InPinGVS, INPUT_PULLUP);                           // D3 сконфигурировать как входной импульс ГВС
  pinMode(InPinHVS, INPUT_PULLUP);                           // D4 сконфигурировать как входной импульс ХВС
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  water();
            
  if (millis() - tmr1 >= (10 * 1000)) {                     //отправка показаний каждые 10 секунд
    tmr1 = millis();
    sendData();
  }
}
