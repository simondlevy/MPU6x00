/*
   Class definition for MPU6000, MPU6500 IMUs using SPI bus

   Copyright (c) 2023 Simon D. Levy

   MIT License
 */

#pragma once

#include <stdint.h>

#include <SPI.h>

class Mpu6x00 {

    public:

        typedef enum {

            GYRO_250DPS,
            GYRO_500DPS,
            GYRO_1000DPS,
            GYRO_2000DPS

        } gyroFsr_e;

        typedef enum {

            ACCEL_2G,
            ACCEL_4G,  
            ACCEL_8G,  
            ACCEL_16G

        } accelFsr_e;

        Mpu6x00(
                SPIClass & spi,
                const uint8_t csPin,
                const gyroFsr_e gyroFsr = GYRO_2000DPS,
                const accelFsr_e accelFsr = ACCEL_16G)
        {

            m_spi = &spi;
            m_csPin = csPin;
            m_gyroFsr = gyroFsr;
            m_accelFsr = accelFsr;
        }

        void begin(void)
        {
            writeRegister(REG_PWR_MGMT_1, BIT_RESET);
            delay(100);

            writeRegister(REG_PWR_MGMT_1, BIT_CLK_SEL_PLLGYROZ);
            delayMicroseconds(7);

            writeRegister(REG_USER_CTRL, BIT_I2C_IF_DIS);
            delayMicroseconds(15);

            writeRegister(REG_PWR_MGMT_2, 0x00);
            delayMicroseconds(15);

            writeRegister(REG_SMPLRT_DIV, 0);
            delayMicroseconds(15);

            writeRegister(REG_GYRO_CONFIG, (uint8_t)(m_gyroFsr << 3));
            delayMicroseconds(15);

            writeRegister(REG_ACCEL_CONFIG, (uint8_t)(m_accelFsr << 3));
            delayMicroseconds(15);

            writeRegister(REG_INT_PIN_CFG, 0x10);
            delayMicroseconds(15);

            writeRegister(REG_INT_ENABLE, BIT_RAW_RDY_EN);
            delayMicroseconds(15);

            writeRegister(REG_CONFIG, 0);
            delayMicroseconds(1);
        }

    private:

        // Configuration bits  
        static const uint8_t BIT_RAW_RDY_EN       = 0x01;
        static const uint8_t BIT_CLK_SEL_PLLGYROZ = 0x03;
        static const uint8_t BIT_I2C_IF_DIS       = 0x10;
        static const uint8_t BIT_RESET            = 0x80;

        // Registers
        static const uint8_t REG_SMPLRT_DIV   = 0x19;
        static const uint8_t REG_CONFIG       = 0x1A;
        static const uint8_t REG_GYRO_CONFIG  = 0x1B;
        static const uint8_t REG_ACCEL_CONFIG = 0x1C;
        static const uint8_t REG_INT_PIN_CFG  = 0x37;
        static const uint8_t REG_INT_ENABLE   = 0x38;
        static const uint8_t REG_USER_CTRL    = 0x6A;
        static const uint8_t REG_PWR_MGMT_1   = 0x6B;
        static const uint8_t REG_PWR_MGMT_2   = 0x6C;

        static const uint32_t SPI_FULL_CLK_HZ = 20000000;
        static const uint32_t SPI_INIT_CLK_HZ = 1000000;

        SPIClass * m_spi;

        uint8_t m_csPin;

        gyroFsr_e m_gyroFsr;
        accelFsr_e m_accelFsr;

        void writeRegister(const uint8_t reg, const uint8_t val)
        {
            m_spi->beginTransaction(SPISettings(SPI_INIT_CLK_HZ, MSBFIRST, SPI_MODE3)); 

            digitalWrite(m_csPin, LOW);
            m_spi->transfer(reg);
            m_spi->transfer(val);
            digitalWrite(m_csPin, HIGH);

            m_spi->endTransaction(); 
        }

        void readRegisters(
                const uint8_t addr,
                uint8_t * buffer,
                const uint8_t count,
                const uint32_t spiClkHz)
        {
            m_spi->beginTransaction(SPISettings(spiClkHz, MSBFIRST, SPI_MODE3));

            digitalWrite(m_csPin, LOW);

            buffer[0] = addr | 0x80;
            m_spi->transfer(buffer, count+1);

            digitalWrite(m_csPin, HIGH);

            m_spi->endTransaction();
        }

}; // class Mpu6x00
