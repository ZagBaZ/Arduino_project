# Анализатор качества воздуха PM2.5
## Ikea Vindriktning + Wemos D1 + AHT10 + BMP180
![Vindriktning_main](https://github.com/ZagBaZ/Arduino_project/blob/main/IKEA_Vindriktning_MQTT\images\Vindriktning_main.png)
 
Модифицируем анализатор воздуха PM2.5 IKEA VINDRIKTNING добавляя датчик температуры, влажности и давления.  
С возможностью передачи данных на MQTT-сервер и отображением в мобильном приложение.  

### Прошивка ESP Wemos D1:

![EasyFlasher](https://github.com/ZagBaZ/Arduino_project/blob/main/IKEA_Vindriktning_MQTT\images\ESPEasyFlasher.png)

Для увязки котроллера ESP Wemos D1 с датчиком воздуха IKEA, прошиваем Wemos D1 с помощью программы ESP Easy Flasher,  
программным обеспечением Tasmota-allsensor.bin, подключаемся к точке доступа от ESP,  
вводим логин-пароль от своей точки доступа в интернет, получаем IP-адрес устройств в вашей сети.  
Подключаемся к свой точке-доступа, в браузере вбиваем IP-адрес устройства, попадаем на главную страницу ОС Tasmota.
В настройках -> входы настраиваем один из цифровых входов на VINDRIKTNING(например D4),  
датчики температуры, влажности и давления на шину I2C (для Wemos - D2, D1).

![Tasmota-Conf](https://github.com/ZagBaZ/Arduino_project/blob/main/IKEA_Vindriktning_MQTT\images\Tamosta_conf.png)

далее настраиваем  параметры MQTT-сервера, адрес брокера, порт, логин и пароль.
 
### Монтаж оборудования:

От платы управления VINDRIKTNING берём питание 5V-GRD, выход REST подключаем к WEMOS вход D4.
Питание датчиков ATH10, BMP180 берем от Wemos 3.3V. Подключаем их по протоколу I2C.

![Vindriktning_1](https://github.com/ZagBaZ/Arduino_project/blob/main/IKEA_Vindriktning_MQTT\images\Vindriktning_1.png)

Приклеиваем датчики и микроконтроллер к корпусу, так чтобы не мешало движению воздуха к внутреннему вентилятору.

![Vindriktning_2](https://github.com/ZagBaZ/Arduino_project/blob/main/IKEA_Vindriktning_MQTT\images\Vindriktning_2.png)

Включаем, открываем браузер и видим полученные данные от датчиков.

![Tasmota-main](https://github.com/ZagBaZ/Arduino_project/blob/main/IKEA_Vindriktning_MQTT\images\Tamosta_main.png)


@zagbaz
