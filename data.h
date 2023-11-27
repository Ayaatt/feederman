#ifndef DATA_H
#define DATA_H

#include "rtc.h"
#include <Arduino.h>

class motor
{
private:
    byte EN;
    byte L_PWM;
    byte R_PWM;
    int R_channel;
    int L_channel;
    int R_freq;
    int L_freq;
    int R_res;
    int L_res;
    int dutyCycle;

public:
    motor(){};
    motor(byte EN, byte L_PWM, byte R_PWM, int R_channel, int L_channel, int R_freq, int L_freq, int R_res, int L_res)
        : EN(EN), L_PWM(L_PWM), R_PWM(R_PWM), R_channel(R_channel), L_channel(L_channel), R_freq(R_freq), L_freq(L_freq), R_res(R_res), L_res(L_res){};
    motor(byte EN, int R_channel, int L_channel, int dutyCycle)
    {
        this->EN = EN;
        this->R_channel = R_channel;
        this->L_channel = L_channel;
        this->dutyCycle = dutyCycle;
    };

    void initBTS();
    void setupBTS();
    void runBTS(byte EN, int R_channel, int L_channel, int dutyCycle);
    void runPelontar();
    void runMixing();
};

struct defineProbiotik
{
    byte EN;
    byte IN1;
    byte IN2;
    int freq;
    int res;
    int pwmCH;
    int dutyCycle;
};

defineProbiotik initLN();
void setupLN();
void runProbiotik();

class suervo
{
private:
    byte pin;

public:
    suervo(){};
    suervo(byte pin)
    {
        this->pin = pin;
    };
    void dataRTC();
    void initServo();
    void setupServo();
    void runServo(int derajat);
    void pakanKeluar(int pakannya);
    void mixingKeluar();
};

#endif