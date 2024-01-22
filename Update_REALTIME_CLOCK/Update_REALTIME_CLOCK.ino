#include <Wire.h>                   // for I2C communication
#include <RTClib.h>                 // for RTC

RTC_DS3231 rtc;      

void setup() {
  rtc.begin();
  Serial.begin(9600);
  updateRTC();

}

void loop() {
}


void updateRTC()
{
  // ask user to enter new date and time
  const char txt[6][15] = { "year [4-digit]", "month [1~12]", "day [1~31]",
                            "hours [0~23]", "minutes [0~59]", "seconds [0~59]"};
  String str = "";
  long newDate[6];

  while (Serial.available()) {
    Serial.read();  // clear serial buffer
  }

  for (int i = 0; i < 6; i++) {
    Serial.print("Enter ");
    Serial.print(txt[i]);
    Serial.print(": ");
    while (!Serial.available()) {
      ; // wait for user input
    }

    str = Serial.readString();  // read user input
    newDate[i] = str.toInt();   // convert user input to number and save to array
    Serial.println(newDate[i]); // show user input
  }
  // update RTC
  rtc.adjust(DateTime(newDate[0], newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]));
  Serial.println("RTC Updated!");
}