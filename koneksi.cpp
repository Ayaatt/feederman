#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <HTTPUpdateServer.h>
#include "BluetoothSerial.h"
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>
#include "koneksi.h"
#include <ArduinoOTA.h>

/* Inisialisasi BLUETOOTH */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
Preferences prefs;
WebServer httpServer(80);
HTTPUpdateServer httpUpdater;

const char *server_ann = "http://192.168.91.101:7000/info_ai";
const char *server_jdw = "http://192.168.91.101:7000/info_jadwal";

/* Inisialisasi UI OTA */
const char *loginIndex =
    "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
    "<tr>"
    "<td colspan=2>"
    "<center><font size=4><b>ESP32 Login Page</b></font></center>"
    "<br>"
    "</td>"
    "<br>"
    "<br>"
    "</tr>"
    "<tr>"
    "<td>Username:</td>"
    "<td><input type='text' size=25 name='userid'><br></td>"
    "</tr>"
    "<br>"
    "<br>"
    "<tr>"
    "<td>Password:</td>"
    "<td><input type='Password' size=25 name='pwd'><br></td>"
    "<br>"
    "<br>"
    "</tr>"
    "<tr>"
    "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
    "</tr>"
    "</table>"
    "</form>"
    "<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/SI')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
    "</script>";

const char *SI =
    "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Update'>"
    "</form>"
    "<div id='prg'>progress: 0%</div>"
    "<script>"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $('#upload_form')[0];"
    "var data = new FormData(form);"
    " $.ajax({"
    "url: '/update',"
    "type: 'POST',"
    "data: data,"
    "contentType: false,"
    "processData:false,"
    "xhr: function() {"
    "var xhr = new window.XMLHttpRequest();"
    "xhr.upload.addEventListener('progress', function(evt) {"
    "if (evt.lengthComputable) {"
    "var per = evt.loaded / evt.total;"
    "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
    "}"
    "}, false);"
    "return xhr;"
    "},"
    "success:function(d, s) {"
    "console.log('success!')"
    "},"
    "error: function (a, b, c) {"
    "}"
    "});"
    "});"
    "</script>";
/*
 * setup function
 */

void koneksi::initKoneksi()
{
    Serial.println("The device started, now you can pair it with bluetooth!");
    WiFi.hostname("feeder");
    SerialBT.begin("ESP32 LORA");
    prefs.begin("feeder");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Wifi Terhubung");
    Serial.println(WiFi.localIP());
}

bool getAPI::connectAPI(const char *server, String &payload)
{
    WiFiClient client;
    HTTPClient http;

    http.begin(client, server);
    int rc = http.GET();

    bool success = false; // Initialize success flag as false
    payload = "";         // Initialize payload as an empty string

    if (rc > 0)
    {
        payload = http.getString();
        success = true; // Set success to true if response code is positive
    }
    else
    {
        Serial.print("Error on sending GET: ");
        Serial.println(rc);
    }

    http.end(); // Ensure http.end() is called

    return success;
}

AIdata ambilDataAI()
{
    getAPI API;
    AIdata dataAI;
    dataAI.beratPakan = 0;
    dataAI.jumlahCairan = 0;
    dataAI.aktif = false;

    String payload;
    if (API.connectAPI(server_ann, payload))
    {
        dataAI.beratPakan = payload.substring(0, payload.indexOf("ab")).toInt();
        dataAI.jumlahCairan = payload.substring(payload.indexOf("ab") + 2, payload.indexOf("cd")).toInt();
        dataAI.aktif = payload.substring(payload.indexOf("cd") + 2, payload.indexOf("cd") + 3) == "t" ? true : false;
        Serial.print("HASIL PARSING AI : ");
        Serial.print(dataAI.beratPakan);
        Serial.print(" = ");
        Serial.print(dataAI.jumlahCairan);
        Serial.print(" = ");
        Serial.println(dataAI.aktif);
    }
    else
    {
        // HTTP request failed
        Serial.println("Server AI GAGAL.");
    }

    return dataAI;
}

dataJadwal *ambilDataJadwal()
{
    getAPI API;
    dataJadwal dataJadwal;
    dataJadwal.jam_pagi = 0;
    dataJadwal.jam_siang = 0;
    dataJadwal.jam_malam = 0;
    dataJadwal.menit_pagi = 0;
    dataJadwal.menit_siang = 0;
    dataJadwal.menit_malam = 0;
    dataJadwal.aktif = false;

    String payload;
    if (API.connectAPI(server_jdw, payload))
    {
        Serial.println(payload);
        parsingWaktu(payload, &dataJadwal.jam_pagi, &dataJadwal.jam_siang, &dataJadwal.jam_malam, &dataJadwal.menit_pagi, &dataJadwal.menit_siang, &dataJadwal.menit_malam, &dataJadwal.aktif);
        Serial.println("HASIL PARSING JADWAL : ");
        Serial.print("JAM : ");
        Serial.print(dataJadwal.jam_pagi);
        Serial.print(" = ");
        Serial.print(dataJadwal.jam_siang);
        Serial.print(" = ");
        Serial.println(dataJadwal.jam_malam);
        Serial.print("MENIT : ");
        Serial.print(" = ");
        Serial.print(dataJadwal.menit_pagi);
        Serial.print(" = ");
        Serial.print(dataJadwal.menit_siang);
        Serial.print(" = ");
        Serial.println(dataJadwal.menit_malam);
        Serial.print("AKTIF : ");
        Serial.println(dataJadwal.aktif);

        if (dataJadwal.aktif)
        {
            simpanData(&dataJadwal);
            Serial.println("Jadwal Aktif, Data disimpan di EEPROM");
        }
    }
    else
    {
        // HTTP request failed
        Serial.println("Server Jadwal GAGAL.");
    }

    return &dataJadwal;
}

void parsingWaktu(String payload, int *jp, int *js, int *jm, int *mp, int *ms, int *mm, bool *aktif)
{
    *jp = payload.substring(payload.indexOf("gh") + 2, payload.indexOf("gh") + 7).substring(0, 2).toInt();
    *js = payload.substring(payload.indexOf("pg") + 2, payload.indexOf("pg") + 7).substring(0, 2).toInt();
    *jm = payload.substring(payload.indexOf("s") + 1, payload.indexOf("s") + 6).substring(0, 2).toInt();

    *mp = payload.substring(payload.indexOf("gh") + 2, payload.indexOf("gh") + 7).substring(3, 5).toInt();
    *ms = payload.substring(payload.indexOf("pg") + 2, payload.indexOf("pg") + 7).substring(3, 5).toInt();
    *mm = payload.substring(payload.indexOf("s") + 1, payload.indexOf("s") + 6).substring(3, 5).toInt();

    // aktif = payload.substring(payload.indexOf("m") + 0, payload.indexOf("m") + 1) == "t" ? true : false;
    *aktif = payload.endsWith("t");
}

void simpanData(dataJadwal *jadwal)
{
    prefs.putInt("jam_pagi", jadwal->jam_pagi);
    prefs.putInt("jam_siang", jadwal->jam_siang);
    prefs.putInt("jam_malam", jadwal->jam_malam);

    prefs.putInt("menit_pagi", jadwal->menit_pagi);
    prefs.putInt("menit_siang", jadwal->menit_siang);
    prefs.putInt("menit_malam", jadwal->menit_malam);

    Serial.print("SIMPAN DATA : ");
    Serial.print(jadwal->jam_pagi);
    Serial.print(" : ");
    Serial.print(jadwal->jam_siang);
    Serial.print(" : ");
    Serial.print(jadwal->jam_malam);
    Serial.print(" : ");

    Serial.print(jadwal->menit_pagi);
    Serial.print(" : ");
    Serial.print(jadwal->menit_siang);
    Serial.print(" : ");
    Serial.print(jadwal->menit_malam);
    Serial.println();
}

loadPrefs loadData()
{
    loadPrefs Load;
    Load.j_pagi = prefs.getInt("jam_pagi");
    Load.j_siang = prefs.getInt("jam_siang");
    Load.j_malam = prefs.getInt("jam_malam");

    Load.m_pagi = prefs.getInt("menit_pagi");
    Load.m_siang = prefs.getInt("menit_siang");
    Load.m_malam = prefs.getInt("menit_malam");
    return Load;
}

void otaSETUP(const char *host)
{
    // SerialBT.begin("ESP32");
    if (!MDNS.begin(host))
    { // http://esp32.local
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");
    SerialBT.println("mDNS responder started");
    /*return index page which is stored in SI */
    httpServer.on("/", HTTP_GET, []()
                  {
    httpServer.sendHeader("Connection", "close");
    httpServer.send(200, "text/html", loginIndex); });
    httpServer.on("/SI", HTTP_GET, []()
                  {
    httpServer.sendHeader("Connection", "close");
    httpServer.send(200, "text/html", SI); });
    /*handling uploading firmware file */
    httpServer.on(
        "/update", HTTP_POST, []()
        {
    httpServer.sendHeader("Connection", "close");
    httpServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); },
        []()
        {
            HTTPUpload &upload = httpServer.upload();
            if (upload.status == UPLOAD_FILE_START)
            {
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                { // start with max available size
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE)
            {
                /* flashing firmware to ESP*/
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END)
            {
                if (Update.end(true))
                { // true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else
                {
                    Update.printError(Serial);
                }
            }
        });
    httpServer.begin();
}

void otaLOOP()
{
    httpServer.handleClient();
    delay(1);
}