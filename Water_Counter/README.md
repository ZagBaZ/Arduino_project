# Счетчик потребления воды с отправкой показаний на MQTT сервер и мобильное приложение.
## NodeMCU + MQTT + IoTMQQTTPlaner
Приложение создано для удаленного получения показаний потребления горячей и холодной  
воды со счетчиков.

![placing](https://github.com/ZagBaZ/Arduino_project/blob/main/Water_Counter/images/placing.jpg)

## Установка и настройка приложения

![mobile](https://github.com/ZagBaZ/Arduino_project/blob/main/Water_Counter/images/mobile.jpg)

Для получения информации с MQTT сервера использовалось приложение IoTMQQTTPlaner.  
В настройках устанавливаем:  
- IP адрес или вебсайт сервера
- Порт
- Тип соединения
- Логин и пароль

далее Создаем панель с топиками:  
- text log: water/gvs
- text log: water/hvs
- switch: water/topic
- text input: water/edit/gvs
- text input: water/edit/gvs

для редактирования показаний требуется перевести switch в положение ON(1). Ввести показания  
ГВС и ХВС и отправить. Далее перевести switch d положение OFF(0).

## Настройка скетча:

Для работы скетча требуется вставить логин и пароль от точки доступа, ip-адрес mqtt_server,  
mqtt_port, mqtt_user, mqtt_password. 

Устанавливаем библиотеки:
```
PubSubClient
GyverTimer
```

### Схема подключения:
![scheme](https://github.com/ZagBaZ/Arduino_project/blob/main/Water_Counter/images/Scheme.jpg)
@zagbaz
