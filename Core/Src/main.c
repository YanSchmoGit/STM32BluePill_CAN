#include <stdint.h>
#include <stm32f1xx.h>

#include "I2C.h"
#include "LCDFunctions.h"
#include "CANBus.h"

#define I2C_SCL_Pin 6
#define I2C_SCL_Port GPIOB

#define I2C_SDA_Pin 7
#define I2C_SDA_Port GPIOB

CANMessage message;

uint32_t data;

int main(void)
{
    /*
    // 1. HSE (High Speed External) einschalten
    RCC->CR |= RCC_CR_HSEON;

    // 2. Warten, bis der HSE stabil schwingt
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 3. Flash Wait States anpassen
    // Bei 8 MHz reicht 0 Wait States (Standard), aber wir stellen sicher,
    // dass keine unnötigen Verzögerungen aktiv sind.
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_0;

    // 4. Bus-Prescaler einstellen (AHB, APB1, APB2)
    // Alle auf /1 setzen, damit der gesamte Chip mit 8 MHz läuft
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);

    // 5. System Clock auf HSE umschalten
    // SW Bits: 01 = HSE als System Clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSE;

    // 6. Warten, bis HSE als System Clock bestätigt ist
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);

    // 7. (Optional) HSI ausschalten, um Strom zu sparen
    RCC->CR &= ~RCC_CR_HSION;
*/


    // Init I2C
    InitI2C(I2C_SCL_Port, I2C_SCL_Pin, I2C_SDA_Port, I2C_SDA_Pin);

    // Init LCD
    LCDSetup();

    // Init CAN Bus
    CanInit();





    while (1)
    {
    if (CanReceive(&message) == 1 )
    {
        data = (uint32_t)message.data[3] | (uint32_t)message.data[2] << 8 | (uint32_t)message.data[1] << 16 | (uint32_t)message.data[0] << 24;
        LCDSendString("         ");
        LCDSetCursorLocation(0,0);
        LCDSendInteger(data);

    }
        else
        {
            LCDSetCursorLocation(0,0);
            LCDSendString("false");
        }
    }
}
