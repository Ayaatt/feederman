#ifndef RTC_H
#define RTC_H

struct rtc
{
    int jam, menit, detik, tanggal, bulan, tahun;
};

rtc getRTC();
void initRTC();

#endif