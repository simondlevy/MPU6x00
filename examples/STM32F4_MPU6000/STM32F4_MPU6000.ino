/*
   MPU6x00 IMU on STM32F4xx MCU

   Copyright (c) 2023 Simon D. Levy

   MIT License
 */

#include "mpu6x00.h"

static const uint8_t MOSI_PIN = PA7;
static const uint8_t MISO_PIN = PA6;
static const uint8_t SCLK_PIN = PA5;

static const uint8_t CS_PIN  = PA4;
static const uint8_t INT_PIN = PA1;

static const uint8_t LED_PIN = PC13;

static SPIClass spi = SPIClass(MOSI_PIN, MISO_PIN, SCLK_PIN);

static Mpu6000 imu = Mpu6000(spi, CS_PIN);

static void blinkLed(void)
{
    const auto msec = millis();
    static uint32_t prev;

    if (msec - prev > 500) {
        static bool on;
        digitalWrite(LED_PIN, on);
        on = !on;
        prev = msec;
    }
}

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

    spi.begin();

    if (!imu.begin()) {
        errorForever();
    }

    pinMode(INT_PIN, INPUT);
    attachInterrupt(INT_PIN, handleInterrupt, RISING);

    pinMode(LED_PIN, OUTPUT);
}

void loop(void)
{
    blinkLed();

    if (gotInterrupt) {

        imu.readSensor();

        Serial.print(imu.getRawGyroX());
        Serial.print("  ");
        Serial.print(imu.getRawGyroY());
        Serial.print("  ");
        Serial.print(imu.getRawGyroZ());
        Serial.println(); 

        gotInterrupt = false;
    }
}
