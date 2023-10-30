#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#define adc_disable() (ADCSRA &= ~(1 << ADEN))  // disable ADC (before power-off)
#define adc_enable() (ADCSRA |= (1 << ADEN))    // re-enable ADC

#define LED PB1       //выход светодиода
#define BUTTON PB2    //вход кнопки
#define RELAY PB4     //выход реле
#define PERIOD 74448  // период работы в секундах (пример: 60*60*24 = 86400 - секунд за день)
#define WORK 10       // время работы в секундах
int DAYS = 1;         // переодичность полива в днях

uint32_t mainTimer, myTimer;  //watchdogCounter
boolean state = true;
boolean setup_start = true;

void setup() {
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);

  adc_disable();  // отключить АЦП (экономия энергии)

  wdt_reset();          // инициализация ватчдога
  wdt_enable(WDTO_1S);  // разрешаем ватчдог
  // 15MS, 30MS, 60MS, 120MS, 250MS, 500MS, 1S, 2S, 4S, 8S

  WDTCR |= _BV(WDTIE);                  // разрешаем прерывания по ватчдогу. Иначе будет ресет
  sei();                                // разрешаем прерывания
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // максимальный сон
}


void blink() {  //функция мигания светодиода
  digitalWrite(LED, HIGH);
  delay(250);
  digitalWrite(LED, LOW);
  delay(250);
}


void loop() {
  if (setup_start) {
    delay(1500);
    if (digitalRead(BUTTON) == HIGH) {  //если нажата кнопка при включение,
      if (DAYS == 4) DAYS = 0;          //выбираем переодичность полива в днях от 1 до 3 дней
      DAYS++;
      if (DAYS == 1) { blink(); }
      if (DAYS == 2) { blink(); blink(); }
      if (DAYS == 3) { blink(); blink(); blink();
      }
      delay(1000);
    } else {
      setup_start = false;  //первоначальная настройка завершена
      pinMode(RELAY, OUTPUT);
    }
  } else {

    mainTimer++;

    if (!state) {
      if ((long)mainTimer - myTimer > PERIOD * DAYS) {  // таймер периода * на количество дней
        myTimer = mainTimer;                            // сброс таймера
        state = true;                                   // флаг на запуск
        digitalWrite(RELAY, LOW);                       // включаем поливатор
      }
    } else {                                   // если помпа включена
      if ((long)mainTimer - myTimer > WORK) {  // таймер времени работы
        myTimer = mainTimer;                   // сброс
        state = false;                         // флаг на выкл
        digitalWrite(RELAY, HIGH);             // выключаем поливатор
      }
    }
    sleep_enable();  // разрешаем сон
    sleep_cpu();     // спать!
  }
}

ISR(WDT_vect) {
  WDTCR |= _BV(WDTIE);  // разрешаем прерывания по ватчдогу. Иначе будет реcет
}
