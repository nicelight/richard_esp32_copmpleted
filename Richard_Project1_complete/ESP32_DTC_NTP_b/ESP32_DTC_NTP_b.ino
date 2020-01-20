#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <RTClib.h>

//ntp + RTS
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
RTC_DS1307 rtc;



//подключаем wifi
const char* ssid     = "mruscom";
const char* password = "Melitta#01";


//переменные Rtc
byte hourRtc = 0;
byte minuteRtc = 0;
byte secondRtc = 0;
int yearRtc = 0;
byte monthRtc = 0;
byte dayRtc = 0;

//переменные Ntp
int yaerNtp = 0;
byte monthNtp = 0;
byte dayNtp = 0;
byte hourNtp = 0;
byte minuteNtp = 0;


unsigned long ntpUpdateMs = 0;
unsigned long rtcUpdateMs = 0;
unsigned long ms = 0;

unsigned long rtcUpdate = 5000; // устанавливаем периодичность получния данных с rtc
unsigned long ntpUpdate = 5000; // устанавливаем периодичность получния данных с ntp

byte minuteNtpRtc = 30;          // устанавливаем минуту часа, на которой обновим данные с сервера на модуле времени
byte secondRtcNtpRtc = 5;       // устанавливаем секунду, на которой обновим данные с сервера на модуле времени (если <=5 сек)

String formattedDate; // стринг от сервера NTP

void setup() {
  Serial.begin(115200);

  rtc.begin();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //стартуем нтп
  timeClient.begin();

  // Добавляем смещение по часовому поясу GMT +3 = 3600*3
  timeClient.setTimeOffset(10800);

  if (rtc.isrunning()) {
    Serial.println("RTC is running!");
    // поставить время на дефолтное
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}//setup


void loop() {
  ms = millis();

  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  //Обновляем время с НТП
  if ((ms - ntpUpdateMs) > ntpUpdate) {
    ntpUpdateMs = ms;

    formattedDate = timeClient.getFormattedDate();
    //Serial.println(formattedDate);

    yaerNtp = formattedDate.substring(0, 4).toInt();
    monthNtp = formattedDate.substring(5, 7).toInt();
    dayNtp = formattedDate.substring(8, 10).toInt();
    hourNtp = formattedDate.substring(11, 13).toInt();
    minuteNtp = formattedDate.substring(14, 17).toInt();

    Serial.println("************* NTP *************");

    //год
    Serial.print(yaerNtp);
    Serial.print(".");

    //месяц
    if (monthNtp < 10) {
      Serial.print("0");
      Serial.print(monthNtp);
    }
    else Serial.print(monthNtp);
    Serial.print(".");

    //день
    if (dayNtp < 10) {
      Serial.print("0");
      Serial.print(dayNtp);
    }
    else Serial.print(dayNtp);

    Serial.print("    H: ");

    // Час
    if (hourNtp < 10) {
      Serial.print("0");
      Serial.print(hourNtp);
    }
    else Serial.print(hourNtp);

    Serial.print(":");

    //Минуты
    if (minuteNtp < 10) {
      Serial.print("0");
      Serial.println(minuteNtp);
    }
    else Serial.println(minuteNtp);
  } //ms

  //секунд на этом сервере нтп нет, но есть дата!


  //Обновляем время с rtc
  if ((ms - rtcUpdateMs) > rtcUpdate) {
    rtcUpdateMs = ms;


    Serial.println("************* RTC *************");

    //раздел rtc
    //**************************************************************************************************************
    DateTime now = rtc.now();

    hourRtc = now.hour();
    minuteRtc = now.minute();
    secondRtc = now.second();
    yearRtc = now.year();
    monthRtc = now.month();
    dayRtc = now.day();

    //год
    Serial.print(yearRtc);
    Serial.print(".");

    //месяц
    if (monthRtc < 10) {
      Serial.print("0");
      Serial.print(monthRtc);
    }
    else Serial.print(monthRtc);
    Serial.print(".");

    //месяц
    if (dayRtc < 10) {
      Serial.print("0");
      Serial.print(dayRtc);
    }
    else Serial.print(dayRtc);

    Serial.print("    H: ");

    // ЧАС
    if (hourRtc < 10) {
      Serial.print("0");
      Serial.print(hourRtc);
    }
    else Serial.print(hourRtc);
    Serial.print(":");

    //Минуты
    if (minuteRtc < 10) {
      Serial.print("0");
      Serial.println(minuteRtc);
    }
    else Serial.print(minuteRtc);

    Serial.print(":");

    //секунды
    if (secondRtc < 10) {
      Serial.print("0");
      Serial.println(secondRtc);
    }
    else Serial.println(secondRtc);

  }//ms

  //обновляем время DTС (с NTP) каждую вторую минуту часа.
  if ((timeClient.update()) && (rtc.isrunning()) && (minuteRtc == minuteNtpRtc) && (secondRtc <= secondRtcNtpRtc)) { // или (minuteNtp > 0)
    Serial.println("Time Update ntp-rtc");
    rtc.adjust(DateTime(yaerNtp, monthNtp, dayNtp, hourNtp, minuteNtp, 0));
  }
}//loop
