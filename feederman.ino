#include "koneksi.h"
#include "data.h"
#include "rtc.h"
#include <TelnetStream.h>

#ifndef STASSID
#define STASSID "Ngueri"
#define STAPSK "ayat12345"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const char *host = "192.168.91.152";

koneksi conn(ssid, password);
motor Motor;
suervo tutup;
bool state = true;

void setup()
{
    Serial.begin(115200);
    conn.initKoneksi();
    initRTC();
    Motor.setupBTS();
    defineProbiotik setupLN;
    tutup.setupServo();
    otaSETUP(host);
}

void loop()
{
    otaLOOP();
    ambilDataJadwal();
    motor BTS;
    suervo SERVO;

    loadPrefs loding = loadData();
    AIdata dataAI = ambilDataAI();
    rtc waktu = getRTC();

    int periodeJam[] = {loding.j_pagi, loding.j_siang, loding.j_malam};
    int periodeMenit[] = {loding.m_pagi, loding.m_siang, loding.m_malam};
    Serial.print("DATA AKTIF AI : ");
    Serial.print(dataAI.aktif);
    Serial.print(" : ");
    Serial.print(dataAI.beratPakan);
    Serial.print(" : ");
    Serial.print(dataAI.jumlahCairan);
    Serial.println();
    Serial.print("DATA LOAD PREFS : ");
    Serial.print(loding.j_pagi);
    Serial.print(" : ");
    Serial.print(loding.j_siang);
    Serial.print(" : ");
    Serial.print(loding.j_malam);
    Serial.print(" : ");
    Serial.print(loding.m_pagi);
    Serial.print(" : ");
    Serial.print(loding.m_siang);
    Serial.print(" : ");
    Serial.print(loding.m_malam);
    Serial.println();
    Serial.print("TANGGAL : ");
    Serial.print(waktu.tahun);
    Serial.print('/');
    Serial.print(waktu.bulan);
    Serial.print('/');
    Serial.print(waktu.tanggal);
    Serial.print(" (");
    Serial.print("JAM RTC : ");
    Serial.print(waktu.jam);
    Serial.print(':');
    Serial.print(waktu.menit);
    Serial.print(':');
    Serial.print(waktu.detik);
    Serial.print(" )");
    Serial.println();

    for (int a = 0; a < 3; a++)
    {
        // Serial.printf("perulangan ke %i \n", a);
        // Serial.printf("\n\n jam saat ini %d:%d, dicek dengan periode %d:%d", waktu.jam, waktu.menit, periodeJam[a], periodeMenit[a]);
        static int menitCek = -1;
        if (waktu.menit != menitCek)
        {
            state = false;          // Allow task execution
            menitCek = waktu.menit; // Update last checked minute
        };
        if (waktu.jam == periodeJam[a] && waktu.menit == periodeMenit[a] && !state)
        {
            Serial.println("EKSEKUSI DIMULAI");
            SERVO.pakanKeluar(dataAI.beratPakan);
            runProbiotik();
            BTS.runMixing();
            SERVO.mixingKeluar();
            BTS.runPelontar();
            Serial.println("EKSEKUSI SELESAI");
            state = true;
        };
    };
    delay(2000);
}