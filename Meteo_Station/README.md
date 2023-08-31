# Метеостанция на ESP8266
## Wemos D1 + OLED + AHT10

![layout](https://github.com/zagbaz/Arduino_project/tree/main/Meteo_Station/images/layout.jpg)
![changelogblack](https://user-images.githubusercontent.com/91951462/136007755-d7fb8881-b59d-4202-abd2-df8f28fa6759.jpg) 

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

### Список отображения:

- Температура воздуха и осадки
- Влажность воздуха
- Давление
- Скорость ветра и направление
- Температура и влажность в помещение
- Время, дата, день недели

### Схема подключения:

![Image alt](https://github.com/zagbaz/Arduino_project/tree/main/Meteo_Station/images/scheme.jpg)

@zagbaz
