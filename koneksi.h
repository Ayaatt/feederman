#ifndef KONEKSI_H
#define KONEKSI_H

#include <Arduino.h>

class koneksi
{
private:
    const char *ssid;
    const char *password;

public:
    koneksi(const char *ssid, const char *password)
    {
        this->ssid = ssid;
        this->password = password;
    };
    void initKoneksi();
};

class getAPI
{
private:
    const char *server;

public:
    bool connectAPI(const char *server, String &payload);
};

struct AIdata
{
    int beratPakan;
    int jumlahCairan;
    bool aktif;
};

AIdata ambilDataAI();
// void aturJadwal();

struct dataJadwal
{
    int jam_pagi, jam_siang, jam_malam, menit_pagi, menit_siang, menit_malam;
    bool aktif;
};
struct loadPrefs
{
    int j_pagi, j_siang, j_malam, m_pagi, m_siang, m_malam;
};

dataJadwal *ambilDataJadwal();
void parsingWaktu(String payload, int *jp, int *js, int *jm, int *mp, int *ms, int *mm, bool *aktif);
void simpanData(dataJadwal *jadwal);
loadPrefs loadData();

void otaSETUP(const char *host);
void otaLOOP();

#endif