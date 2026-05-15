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

// CAN Control Type Def

typedef struct
{
    uint8_t data_ready;
} CANControl;


extern volatile CANMessage CAN_Message;
extern volatile CANControl CAN_Status;

// Functions

int8_t CanInit();

int8_t CanFilter(uint16_t id, uint16_t mask);

int8_t CanReceive(volatile CANMessage* msg);

// Interrupt handler
void USB_LP_CAN1_RX0_IRQHandler(void);


#endif //STM32BLUEPILL_CAN_CANBUS_H
;
