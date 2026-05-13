#include <stdint.h>
#include <stm32f1xx.h>

#include "I2C.h"
#include "LCDFunctions.h"
#include "CANBus.h"

#define I2C_SCL_Pin 6
#define I2C_SCL_Port GPIOB

#define I2C_SDA_Pin 7
#define I2C_SDA_Port GPIOB

//CANMessage message;
//CANControl CAN;


int main(void)
{
    // Init I2C
    InitI2C(I2C_SCL_Port, I2C_SCL_Pin, I2C_SDA_Port, I2C_SDA_Pin);

    // Init LCD
    LCDSetup();

    // Init CAN Bus
    CanInit();
    CanFilter(0x7B, 0x7FF); // Id 123 = hex 0x7B


    while (1)
    {
        if (CAN_Status.data_ready)
        {
            LCDSendString("         ");
            LCDSetCursorLocation(0, 0);
            LCDSendInteger(CAN_Message.value_1);
            LCDSendString(" C");

            LCDSetCursorLocation(0, 1);
            LCDSendInteger(CAN_Message.value_2);
            CAN_Status.data_ready = 0;
        }
    }
}


