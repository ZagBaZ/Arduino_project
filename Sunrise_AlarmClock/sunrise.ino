#include "pitches.h"
#include "RTClib.h"
#include <EEPROM.h>
#include "GyverEncoder.h"
#include "GyverTimer.h"
#include <TM1637Display.h>
#include "FastLED.h"

RTC_DS3231 rtc;
#define PIN_PHOTO_SENSOR A0                            //вход подключения фоторезистора
#define LED_ALARM_PIN 11                               //выход индикации включения будильника
#define LED_PIN 5                                      //выход адресной светодиодной ленты
#define LED_NUM 5                                      //количество светодиодов в ленте
#define CLKd 8                                         //выход Clk дисплея
#define DIOd 9                                         //выход DIO дисплея
TM1637Display display = TM1637Display(CLKd, DIOd);

GTimer_ms DawnTimer(7060);                              //таймер одного цикла, при рассвете 30 мин. (30*60/256, где 256 - шагов рассвета)
GTimer MinuteTimer;                                     //таймер защиты повторного включения рассвета
GTimer WaitTimer;                                       //таймер возврата на режим часы
GTimer_ms dotsTimer(1000);                              //таймер мигания секундной точки на дисплее

#define CLKe 2                                          // вход энкодера S1
#define DTe 3                                           // вход энкодера S2
#define SWe 4                                           // вход энкодера KEY
Encoder enc(CLKe, DTe, SWe, TYPE1);

int8_t hrs, mins;
int8_t new_hrs, new_mins;
int8_t alm_hour, alm_min;
int8_t dawn_hour, dawn_min;

byte mode = 0;                                          // режимы работы: 0 - часы, 1 - уствновка будильника, 2 - установка времени


boolean dawn_start = false;
boolean alarmFlag = false;
boolean minuteFlag = false;
boolean alarm = false;
boolean dotsFlag = false;
boolean waitFlag = false;

int bright = 1;
int green = 0;
int blue = 0;
CRGB leds[LED_NUM];
int lcd_bright;

void setup() {
  Serial.begin(115200);
  
  rtc.begin();
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  
  display.setBrightness(1);
  display.clear();
  
  alm_hour = EEPROM.read(0);                            // при включение считываем время будильника из EEPROM
  alm_min = EEPROM.read(1);
  alarmFlag = EEPROM.read(2);
  calc_dawn();                                          // высчитываем время начала будильника
     
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.setBrightness(bright);
  for (int i = 0; i < LED_NUM; i++) {
      leds[i] = CRGB::Black;;
      }
  FastLED.show();
}
  

void loop() {
  DateTime now = rtc.now();
  hrs = now.hour();
  mins = now.minute();
  
  lcd_bright = map((analogRead(PIN_PHOTO_SENSOR)), 0, 1023, 0, 7);        // яркость экрана времени от фоторезистора
  display.setBrightness(lcd_bright);
 
  encoderTick();                                                          // работа энкодера
  alarmTick();                                                            

  if (alarmFlag) digitalWrite(LED_ALARM_PIN, HIGH);                       //светодиод будильника горит, если будильник включен
  else digitalWrite(LED_ALARM_PIN, LOW);
  
  if (MinuteTimer.isReady()) {
    minuteFlag = false;
  }

  if (WaitTimer.isReady()) {                                              //возвращает режим часов из режимов установки будильника или установки времени при бездействие
    waitFlag = true;
      if ( (waitFlag && mode == 1) || (waitFlag && mode == 2) ) {
      mode = 0;
    }
    waitFlag = false;
  }
      
  if (dotsTimer.isReady()) dotsFlag = !dotsFlag;                          // мигание секундных точек на дисплее
    
  if (mode == 0) {
    int displaytime = (now.hour() * 100) + now.minute();
    if (dotsFlag) display.showNumberDecEx(displaytime, 0b11100000, true); 
    else display.showNumberDecEx(displaytime, 0b10000000, true);
  }
  if (mode == 1) {                                                        // Режим установки будильника
    if (enc.isRight()) {
      alm_min++;
      if (alm_min > 59) {
        alm_min = 0;
        alm_hour++;
        if (alm_hour > 23) alm_hour = 0;
      }
    }
    if (enc.isLeft()) {
      alm_min--;
      if (alm_min < 0) {
        alm_min = 59;
        alm_hour--;
        if (alm_hour < 0) alm_hour = 23;
      }
    }
    if (enc.isRightH()) {
      alm_hour++;
      if (alm_hour > 23) alm_hour = 0;
    }
    if (enc.isLeftH()) {
      alm_hour--;
      if (alm_hour < 0) alm_hour = 23;
    }
    int alarmtime = (alm_hour * 100) + alm_min;
    display.showNumberDecEx(alarmtime, 0b11100000, true);
  }
  
  if (mode == 2) {                                                      // Режим установки времени
    if (enc.isRight()) {
      new_mins++;
      if (new_mins > 59) {
        new_mins = 0;
        new_hrs++;
        if (new_hrs > 23) hrs = 0;
      }
    }
    if (enc.isLeft()) {
      new_mins--;
      if (new_mins < 0) {
        new_mins = 59;
        new_hrs--;
        if (new_hrs < 0) new_hrs = 23;
      }
    }
    if (enc.isRightH()) {
      new_hrs++;
      if (new_hrs > 23) new_hrs = 0;
    }
    if (enc.isLeftH()) {
      new_hrs--;
      if (new_hrs < 0) new_hrs = 23;
    }
    int new_displaytime = (new_hrs * 100) + new_mins;
    display.showNumberDecEx(new_displaytime, true);
    }
       
}


void calc_dawn() {                           // Функция расчёта времени начала включения будильника
  if (alm_min > 29) {                        // если минут во времени будильника больше продолжительности рассвета
    dawn_hour = alm_hour;                    // час рассвета равен часу будильника
    dawn_min = alm_min - 30;                 // минуты рассвета = минуты будильника - продолж. рассвета
  } else {                                   // если минут во времени будильника меньше продолжительности рассвета
    dawn_hour = alm_hour - 1;                // значит рассвет будет часом раньше
    if (dawn_hour < 0) dawn_hour = 23;       // защита от совсем поехавших
    dawn_min = 60 - (30 - alm_min);          // находим минуту рассвета в новом часе
  }
}


void alarmTick() {                           // Функция включения будильника
   if ((hrs == dawn_hour) && (mins == dawn_min) && (alarmFlag) && (!minuteFlag)) {    
    dawn_start = true;
    alarm = true;
    minuteFlag = true;
    MinuteTimer.setTimeout(60000);
   }
   if (dawn_start) {                                  //включение рассвета на светодиодной ленте
    if (DawnTimer.isReady()) {
    if ((bright < 255)) bright+=1;
    if ((0 < bright < 150)) { 
      for (int i = 0; i < LED_NUM; i++) {
      leds[i] = CRGB(255, green, blue);               
    }}
    if ((bright > 100 and green < 255)) {
      green += 1; 
    }
    if ((bright > 200 and blue < 150)) {
      blue += 1; 
    }
    FastLED.setBrightness(bright);
    FastLED.show();
    }
   }
    if ((hrs == alm_hour) && (mins == alm_min) && (alarm)) {    //включение пищалки
      for (int thisNote = 0; thisNote < 8; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(10, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;                                                                                                                                                                                
      delay(pauseBetweenNotes);
      noTone(10);
     }
   alarm = false;
   }
}


void encoderTick() {
  enc.tick();                 // работаем с энкодером
         // *** клик по энкодеру
  if (enc.isClick()) {        // клик по энкодеру
    mode++;                   // сменить режим
    WaitTimer.setTimeout(120000);
    if (mode == 1) {
      new_hrs = hrs;
      new_mins = mins;
    }
    if (mode > 2) {           // выход с режима установки будильника и часов
      mode = 0;
      alm_hour = EEPROM.read(0);
      alm_min = EEPROM.read(1);      
    }
  }

         // *** удержание кнопки энкодера 
  if (enc.isHolded()) {                                        // кнопка удержана
    if (dawn_start) {                                          // если удержана во время рассвета или будильника
      dawn_start = false;                                      // прекратить рассвет
      alarmFlag = false;                                       // и будильник
      alarm = false;
      for (int i = 0; i < LED_NUM; i++) {
      leds[i] = CRGB::Black;;
      }
      FastLED.show();
      bright = 1;
      green = 0;
      blue = 0;        
    }
    if (mode == 0 && !dawn_start) {                            // кнопка удержана в режиме часов и сейчас не рассвет
      alarmFlag = !alarmFlag;                                  // переключаем будильник
      if (alarmFlag) EEPROM.update(2, alarmFlag);              // запоминаем в EEPROM включен ли будильник 
      else EEPROM.update(2, alarmFlag);
      delay(1000);
    } 
    else if (mode == 1) {                                      // кнопка удержана в режиме настройки будильника
      mode = 0;                                                // сменить режим
      calc_dawn();                                             // расчёт времени рассвета
      EEPROM.update(0, alm_hour);
      EEPROM.update(1, alm_min);    
    }
    else if (mode == 2) {
      rtc.adjust(DateTime(2014, 1, 21, new_hrs, new_mins, 0));
      mode = 0;
      delay(1000);   
    }
  }
}
