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

        float ax=0, ay=0, az=0;
        float gx=0, gy=0, gz=0;

        imu.getGyro(gx, gy, gz);

        imu.getAccel(ax, ay, az);

        Serial.printf(
                "gx=%+3.3f gy=%+3.3f gz=%+3.3f | ax=%+3.3f ay=%+3.3f az=%+3.3f\n",
                gx, gy, gz, ax, ay, az);

        gotInterrupt = false;
    }
}
