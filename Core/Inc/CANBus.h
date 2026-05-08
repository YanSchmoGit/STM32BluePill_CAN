//
// Created by yan on 4/27/26.
//

#ifndef STM32BLUEPILL_CAN_CANBUS_H
#define STM32BLUEPILL_CAN_CANBUS_H
#include <stdint.h>

// CAN Message Type Def

typedef struct
{
    uint32_t id;
    uint8_t dlc;
    uint32_t value_1;
    uint32_t value_2;
} CANMessage;

// Functions

int8_t CanInit();

int8_t CanReceive(CANMessage* msg);


#endif //STM32BLUEPILL_CAN_CANBUS_H
