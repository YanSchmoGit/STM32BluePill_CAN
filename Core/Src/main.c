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

uint8_t Data_ready = 0;


int main(void)
{
    // Init I2C
    InitI2C(I2C_SCL_Port, I2C_SCL_Pin, I2C_SDA_Port, I2C_SDA_Pin);

    // Init LCD
    LCDSetup();

    // Init CAN Bus
    CanInit();


    while (1)
    {
        if (Data_ready == 1)
        {
            LCDSendString("         ");
            LCDSetCursorLocation(0, 0);
            LCDSendInteger(message.value_1);
            LCDSendString(" C");

            LCDSetCursorLocation(0, 1);
            LCDSendInteger(message.value_2);
            Data_ready = 0;
        }
    }
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CanReceive(&message);
    Data_ready = 1;
}
