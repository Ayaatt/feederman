#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "uRTCLib.h"
#include "rtc.h"

uRTCLib ds(0x68);

void initRTC()
{
    URTCLIB_WIRE.begin();
    Serial.println("INIT RTC");
    // ds.set(10, 45, 15, 5, 24, 11, 23);
    //   RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
}

rtc getRTC()
{
    rtc getRTC;
    getRTC.jam = 0;
    getRTC.menit = 0;
    getRTC.detik = 0;
    getRTC.tanggal = 0;
    getRTC.bulan = 0;
    getRTC.tahun = 0;

    ds.refresh();
    // DateTime now = rtc.now();

    // waktu.hari = daysOfTheWeek[now.dayOfTheWeek()];
    getRTC.tahun = ds.year();
    getRTC.bulan = ds.month();
    getRTC.tanggal = ds.day();

    getRTC.jam = ds.hour();
    getRTC.menit = ds.minute();
    getRTC.detik = ds.second();

    return getRTC;
}
