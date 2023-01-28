/*
   MPU6x00 IMU on Teensy MCU

   Copyright (c) 2023 Simon D. Levy

   MIT License
 */

#include "mpu6x00.h"

static const uint8_t CS_PIN  = 10;
static const uint8_t INT_PIN = 9;

static Mpu6x00 imu = Mpu6x00(SPI, CS_PIN);

static bool gotInterrupt;

static void handleInterrupt(void)
{
    gotInterrupt = true;
}

static void errorForever(void)
{
    while (true) {
        Serial.println("Error initializing IMU");
        delay(500);
    }
}

void setup(void)
{
    Serial.begin(115200);

    if (!imu.begin()) {
        errorForever();
    }

    pinMode(INT_PIN, INPUT);
    attachInterrupt(INT_PIN, handleInterrupt, RISING);
}

void loop(void)
{
    if (gotInterrupt) {
        imu.readData();
        int16_t x=0, y=0, z=0;
        imu.getRawGyro(x, y, z);
        Serial.print(x);
        Serial.print("  ");
        Serial.print(y);
        Serial.print("  ");
        Serial.print(z);
        Serial.println();
        gotInterrupt = false;
    }
}
