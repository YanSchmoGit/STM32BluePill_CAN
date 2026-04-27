/*
 * Source file for I2C communication and functions
 *
 */


#include "I2C.h"


void EnableBusClock(GPIO_TypeDef* GPIOx)
{
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    if (GPIOx == GPIOA)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    }

    if (GPIOx == GPIOB)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    }

    if (GPIOx == GPIOC)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    }

    if (GPIOx == GPIOD)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
    }
}


int8_t InitI2C(GPIO_TypeDef* GPIOxSCL, uint8_t pinSCL, GPIO_TypeDef* GPIOxSDA, uint8_t pinSDA)
{
    // ##### Activate bus clocks #####

    // GPIO
    EnableBusClock(GPIOxSCL);
    EnableBusClock(GPIOxSDA);

    // I2C
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;


    // ##### GPIO register settings #####

    // PUPD & Outputtype

    // Set to:  Output mode, max speed 50 MHz
    //          Alternate function output Open-drain

    if (pinSCL < 8 && pinSDA < 8)
    {
        GPIOxSCL->CRL |= (0x3UL << (pinSCL * 4)); // Set output mode
        GPIOxSCL->CRL |= (0x3UL << ((pinSCL * 4) + 2)); // Set alternate function & open drain

        GPIOxSDA->CRL |= (0x3UL << (pinSDA * 4)); // Set output mode
        GPIOxSDA->CRL |= (0x3UL << ((pinSDA * 4) + 2)); // Set alternate function & open drain

    }
    else if (pinSCL >= 8 && pinSDA >= 8)
    {
        GPIOxSCL->CRH |= (0x3UL << ((pinSCL - 8) * 4)); // Set output mode
        GPIOxSCL->CRH |= (0x3UL << (((pinSCL - 8) * 4) + 2)); // Set alternate function & open drain

        GPIOxSDA->CRH |= (0x3UL << ((pinSDA - 8) * 4)); // Set output mode
        GPIOxSDA->CRH |= (0x3UL << (((pinSDA - 8) * 4) + 2)); // Set alternate function & open drain

        AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
    }
    else
    {
        return 1; // Return error

    }


    // ##### I2C initialisieren #####

    I2C1->CR1 &= ~I2C_CR1_PE;
    I2C1->CR2 = 8; // Clock frequency 8 HHz
    I2C1->CCR = 40; // Calculate CCR for 100 mHz --> T_high = CCR * T_pclk1 -> CCR = 8MHz / (2 * 100kHz) = 40
    I2C1->TRISE = 9; // Set TRISE --> 1000ns / T_pclk1 + 1 = 1000ns / 125ns + 1 = 9
    I2C1->CR1 |= I2C_CR1_PE;

    return 0;
}

int8_t ReadI2C(uint8_t devAdr, uint8_t regAdr, uint8_t numberOfBytes, uint8_t* data)
{
    // Wait for bus to be free
    while (I2C1->SR2 & I2C_SR2_BUSY)
    {
        ;
    };

    // Generate start condition
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
        ;
    }; // Check, if no start condition is generated

    // Send device address
    I2C1->DR = (devAdr << 1);
    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        ;
    }; // Wait for address bit to be set
    (void)I2C1->SR2; // Access SR2 register without using it, to reset ADDR Flag

    // Address register
    I2C1->DR = regAdr;
    while (!(I2C1->SR1 & I2C_SR1_TXE))
    {
        ;
    };

    // Repeat start signal and read data
    // Send address and set read bit to TRUE
    I2C1->DR = (devAdr << 1) | 0x01;
    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        ;
    };


    for (size_t i = 0; i < numberOfBytes - 1; i++)
    {
        I2C1->CR1 |= I2C_CR1_ACK; // Acknowledge to read another byte
        (void)I2C1->SR2;

        while (!(I2C1->SR1 & I2C_SR1_RXNE))
        {
            ;
        };
        *data = I2C1->DR;
        data++;
    }

    I2C1->CR1 &= ~I2C_CR1_ACK;
    (void)I2C1->SR2;
    I2C1->CR1 |= I2C_CR1_STOP;

    while (!(I2C1->SR1 & I2C_SR1_RXNE))
    {
        ;
    };
    *data = I2C1->DR;

    return 0;
}

int8_t WriteI2C(uint8_t devAdr, uint8_t regAdr, uint8_t data)
{
    // Wait for bus to be free
    while (I2C1->SR2 & I2C_SR2_BUSY)
    {
        ;
    };

    // Generate start condition
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
        ;
    }; // Check, if no start condition is generated

    // Send address
    I2C1->DR = (devAdr << 1);
    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        ;
    }; // Wait for address bit to be set
    (void)I2C1->SR2; // Access SR2 register without using it, to reset ADDR Flag

    // Address data register
    I2C1->DR = regAdr;
    while (!(I2C1->SR1 & I2C_SR1_TXE))
    {
        ;
    };

    // Send Data
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF))
    {
        ;
    };

    // Generate stop condition
    I2C1->CR1 |= I2C_CR1_STOP;

    return 0;
}
