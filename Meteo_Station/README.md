# Метеостанция на ESP8266
## Wemos D1 + OLED + AHT10
![layout](https://github.com/ZagBaZ/Arduino_project/blob/main/Meteo_Station/images/layout.jpg)
### Список отображения:

- Температура воздуха и осадки
- Влажность воздуха
- Давление
- Скорость ветра и направление
- Температура и влажность в помещение
- Время, дата, день недели
- 
### Настройки Метеостанции:

Для работы метеостанции требуется получить токен на сайте https://openweathermap.org/api.
В скетч вставить логин и пароль от точки доступа, ID города, токен.

Устанавливаем библиотеки:
```
Adafruit_AHT10
ArduinoJson
GyverOLEDWIFI
NTPClient
```

### Схема подключения:
![scheme](https://github.com/ZagBaZ/Arduino_project/blob/main/Meteo_Station/images/scheme.jpg)

@zagbaz
