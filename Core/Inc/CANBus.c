//
// Created by yan on 4/27/26.
//

#include "CANBus.h"

#include "stm32f103xb.h"


int8_t CanInit()
{
    // Enable system clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

    // Set GPIOs to alternate function
/*
    // PA 11 - RX Pin
    // Set to: Input Pull-up/ Pull-down
    GPIOA->CRH &= ~(GPIO_CRH_CNF11_Msk | GPIO_CRH_MODE11_Msk);
    GPIOA->CRH |= GPIO_CRH_MODE11_1;
    GPIOA->ODR |= GPIO_ODR_ODR11; // Set Pull-up

    // PA 12 - TX Pin
    // Set to: Output mode max 50MHz, alternate function, Push-pull
    GPIOA->CRH &= ~(GPIO_CRH_CNF12_Msk | GPIO_CRH_MODE12_Msk);
    GPIOA->CRH |= (GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 | GPIO_CRH_CNF12_1);
*/

    // PA11 (RX): Input Floating (0x4)
    GPIOA->CRH &= ~(0x0F << 12);
    GPIOA->CRH |= (0x04 << 12);

    // PA12 (TX): Alternate Function Push-Pull, 50MHz (0xB)
    GPIOA->CRH &= ~(0x0F << 16);
    GPIOA->CRH |= (0x0B << 16);

    // Init CAN Bus

    CAN1->MCR |= CAN_MCR_INRQ; // Activate initialization mode
    while (!(CAN1->MSR & CAN_MSR_INAK))
    {
        ;
    }

    CAN1->MCR &= ~CAN_MCR_SLEEP; // Leave sleep mode


    // Set Bauderate - 500 kbit/s at 8MHz MCU system clock
    // Bauderate = PCLK1  / (Prescaler * (1 + TS1 + TS2))



    CAN1->BTR = ((0 << CAN_BTR_SJW_Pos) | // SWJ value = 1
        (9 << CAN_BTR_TS1_Pos) | // TS1 value = 10
        (4 << CAN_BTR_TS2_Pos) | // TS2 value = 5
        (0 << CAN_BTR_BRP_Pos)); // Prescaler value = 1

    CAN1->BTR &= ~CAN_BTR_SILM;
    // Deactivate initialization mode

    CAN1->MCR &= ~CAN_MCR_INRQ;
    while (CAN1->MSR & CAN_MSR_INAK)
    {
        ;
    } // wait until normal mode is active

    // Configure filter

    CAN1->FMR |= CAN_FMR_FINIT; // Init mode for all filter
    CAN1->FA1R &= ~CAN_FA1R_FACT0; // Deactivate filter 0
    CAN1->FS1R |= CAN_FS1R_FSC0; // Enable 32-bit scale mode
    CAN1->FM1R &= ~CAN_FM1R_FBM0; // Identifier mask mode

    CAN1->sFilterRegister[0].FR1 = 0; // Filter id 0 (accept all)
    CAN1->sFilterRegister[0].FR2 = 0; // Filter mask 0 (accept all)

    CAN1->FFA1R &= ~(CAN_FFA1R_FFA0); // Assign filter 0 to FIFO 0
    CAN1->FA1R |= CAN_FA1R_FACT0;
    CAN1->FMR &= ~CAN_FMR_FINIT;

    return 0;
};

int8_t CanReceive(CANMessage* msg)
{
    // Check if message s ready

    if ((CAN1->RF0R & CAN_RF0R_FMP0) == 0)
    {
        return 0; // No message received

    }

    // Extract ID from data
    msg->id = (CAN1->sFIFOMailBox[0].RIR >> CAN_RI0R_STID_Pos);

    // Get data length
    msg->dlc = (CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_DLC_Msk);

    // Get data from data registers

    uint32_t lowReg  = CAN1->sFIFOMailBox[0].RDLR;
    uint32_t highReg = CAN1->sFIFOMailBox[0].RDHR;

    // Divide data to output structure

    msg->data[0] = (uint8_t)(lowReg >> CAN_RDL0R_DATA0_Pos & CAN_RDL0R_DATA0_Msk);
    msg->data[1] = (uint8_t)(lowReg >> CAN_RDL0R_DATA1_Pos & CAN_RDL0R_DATA0_Msk);
    msg->data[2] = (uint8_t)(lowReg >> CAN_RDL0R_DATA2_Pos & CAN_RDL0R_DATA0_Msk);
    msg->data[3] = (uint8_t)(lowReg >> CAN_RDL0R_DATA3_Pos & CAN_RDL0R_DATA0_Msk);

    msg->data[4] = (uint8_t)(highReg >> CAN_RDH0R_DATA4_Pos & CAN_RDH0R_DATA4_Msk);
    msg->data[5] = (uint8_t)(highReg >> CAN_RDH0R_DATA5_Pos & CAN_RDH0R_DATA4_Msk);
    msg->data[6] = (uint8_t)(highReg >> CAN_RDH0R_DATA6_Pos & CAN_RDH0R_DATA4_Msk);
    msg->data[7] = (uint8_t)(highReg >> CAN_RDH0R_DATA7_Pos & CAN_RDH0R_DATA4_Msk);


    // Release pending message
    CAN1->RF0R |= CAN_RF0R_RFOM0;

    return 1; // Message received
}
