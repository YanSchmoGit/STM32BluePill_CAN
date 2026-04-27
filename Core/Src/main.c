#include <stdint.h>
#include <stm32f1xx.h>

#include "I2C.h"
#include "LCDFunctions.h"

#define I2C_SCL_Pin 6
#define I2C_SCL_Port GPIOB

#define I2C_SDA_Pin 7
#define I2C_SDA_Port GPIOB

#define PORTEXPANDER_DEVICE_ADR  (0x20)


int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH |= GPIO_CRH_CNF13_0;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;

    GPIOC->BSRR |= GPIO_BSRR_BS13;

    // Init I2C
    InitI2C(I2C_SCL_Port, I2C_SCL_Pin, I2C_SDA_Port, I2C_SDA_Pin);

    // Init LCD
    LCDSetup();

    LCDSendString("Tja bloed");


    while (1)
    {
    }
}
