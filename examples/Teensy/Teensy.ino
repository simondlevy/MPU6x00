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

static void reportAxes(const char * label, const int16_t x, const int16_t y, const int16_t z)
{
    Serial.print(label);
    Serial.print("x = ");
    Serial.print((float)x, 4);
    Serial.print("  ");
    Serial.print(label);
    Serial.print("y = ");
    Serial.print(y);
    Serial.print("  ");
    Serial.print(label);
    Serial.print("z = ");
    Serial.print(z);
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
        int16_t gx=0, gy=0, gz=0;
        imu.getRawGyro(gx, gy, gz);

        Serial.printf("gx=%+05d gy=%+05d\n", gx, gy);

        //reportAxes("g", x, y, z);
        //Serial.println();

        gotInterrupt = false;
    }
}
