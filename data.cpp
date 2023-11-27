#include "data.h"
#include "rtc.h"
#include <ESP32Servo.h>

// INIT PELONTAR
#define EN_PEL 23
#define L_PWM_PEL 19
#define R_PWM_PEL 18

// INIT MIXING
#define EN_MIX 12
#define L_PWM_MIX 4
#define R_PWM_MIX 14

// INIT PROBIOTIK
#define EN_PRO 5
#define IN1_PRO 26
#define IN2_PRO 25

#define PAKAN 33
#define MIX 32
Servo katup;
suervo pakan(PAKAN);
suervo mix(MIX);

const int R_freq = 30000;
const int L_freq = 30000;
const int R_res = 8;
const int L_res = 8;

const int R_channel_PEL = 1;
const int L_channel_PEL = 2;
int dutyCyclePel = 80;
int dutyCycleMix = 80;

const int R_channel_MIX = 3;
const int L_channel_MIX = 4;

const int freq_PRO = 30000;
const int res_PRO = 8;
const int pwmCH_probiotik = 0;

void motor::initBTS()
{
    pinMode(EN, OUTPUT);
    pinMode(R_PWM, OUTPUT);
    pinMode(L_PWM, OUTPUT);
    ledcSetup(R_channel, R_freq, R_res);
    ledcSetup(L_channel, L_freq, L_res);
    ledcAttachPin(R_PWM, R_channel);
    ledcAttachPin(L_PWM, L_channel);
    Serial.println("INIT BTS");
}

void motor::setupBTS()
{
    motor pelontar(EN_PEL, L_PWM_PEL, R_PWM_PEL, R_channel_PEL, L_channel_PEL, R_freq, L_freq, R_res, L_res);
    motor mixing(EN_MIX, L_PWM_MIX, R_PWM_MIX, R_channel_MIX, L_channel_MIX, R_freq, L_freq, R_res, L_res);
    pelontar.initBTS();
    mixing.initBTS();
    Serial.println("SETUP BTS");
}

void motor::runBTS(byte EN, int R_channel, int L_channel, int dutyCycle)
{
    digitalWrite(EN, HIGH);
    ledcWrite(R_channel, dutyCycle);
    ledcWrite(L_channel, dutyCycle);
    delay(3000);
    digitalWrite(EN, LOW);
}

void motor::runPelontar()
{
    runBTS(EN_PEL, R_channel_PEL, L_channel_PEL, dutyCyclePel);
    Serial.println("PELONTAR JALAN");
}

void motor::runMixing()
{
    runBTS(EN_MIX, R_channel_MIX, L_channel_MIX, dutyCycleMix);
    Serial.println("MIXING JALAN");
}

defineProbiotik probiotik(defineProbiotik pro)
{
    pinMode(pro.EN, OUTPUT);
    pinMode(pro.IN1, OUTPUT);
    pinMode(pro.IN2, OUTPUT);
    ledcSetup(pro.pwmCH, pro.freq, pro.res);
    ledcAttachPin(pro.EN, pro.pwmCH);
    Serial.println("INIT PROBIOTIK");
    return pro;
}
void setupLN(defineProbiotik pro)
{
    pro = probiotik(pro);
    Serial.println("SETUP LN");
}

void runProbiotik()
{
    digitalWrite(IN1_PRO, LOW);
    digitalWrite(IN2_PRO, HIGH);
    ledcWrite(EN_PRO, 150);
    delay(3000);
    digitalWrite(IN1_PRO, HIGH);
    digitalWrite(IN2_PRO, HIGH);
}

void suervo::initServo()
{
    katup.attach(pin);
    Serial.println("INIT SERVO");
}

void suervo::setupServo()
{
    pakan.initServo();
    mix.initServo();
    Serial.println("SETUP SERVO");
}

void suervo::runServo(int derajat)
{
    katup.write(derajat);
}

void suervo::pakanKeluar(int pakannya)
{
    int out = (pakannya / 77.21) * 1000; // bps
    Serial.println("Servo Pakan Open");
    pakan.runServo(0);
    // delay(1000);
    delay(out);
    pakan.runServo(140);
    Serial.println("Servo Pakan Close");
}

void suervo::mixingKeluar()
{
    Serial.println("Servo Mix Open");
    mix.runServo(120);
    delay(5000);
    mix.runServo(60);
    Serial.println("Servo Mix Tutup");
}
