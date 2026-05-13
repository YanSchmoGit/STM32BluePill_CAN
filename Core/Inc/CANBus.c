//
// Created by yan on 4/27/26.
//

#include "CANBus.h"

#include "stm32f103xb.h"

volatile CANMessage CAN_Message;
volatile CANControl CAN_Status;

int8_t CanInit()
{
    // Enable system clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

    // Set GPIOs to alternate function

        // PA 11 - RX Pin
        // Set to: Input Floating
        GPIOA->CRH &= ~(GPIO_CRH_CNF11_Msk | GPIO_CRH_MODE11_Msk);
        GPIOA->CRH |= GPIO_CRH_CNF11_0;


        // PA 12 - TX Pin
        // Set to: Output mode max 50MHz, alternate function, Push-pull
        GPIOA->CRH &= ~(GPIO_CRH_CNF12_Msk | GPIO_CRH_MODE12_Msk);
        GPIOA->CRH |= (GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 | GPIO_CRH_CNF12_1);


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


    // Deactivate initialization mode
    CAN1->MCR &= ~CAN_MCR_INRQ;
    while (CAN1->MSR & CAN_MSR_INAK)
    {
        ;
    } // wait until normal mode is active

    // Activate interrupt for pending message
    CAN1->IER |= CAN_IER_FMPIE0;



    // NVIC Interrupt aktivieren

    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

    return 0;
};

int8_t CanFilter(uint16_t id, uint16_t mask)
{
    // Configure filter

    CAN1->FMR |= CAN_FMR_FINIT; // Init mode for all filter
    CAN1->FA1R &= ~CAN_FA1R_FACT0; // Deactivate filter 0
    CAN1->FS1R |= CAN_FS1R_FSC0; // Enable 32-bit scale mode
    CAN1->FM1R &= ~CAN_FM1R_FBM0; // Identifier mask mode

    CAN1->sFilterRegister[0].FR1 = id << 21; // Filter id  - shift 21 bytes --> Standard ID is used (Bits 21:31)
    CAN1->sFilterRegister[0].FR2 = mask << 21; // Filter mask - shift 21 bytes --> Standard ID is used (Bits 21:31)

    CAN1->FFA1R &= ~(CAN_FFA1R_FFA0); // Assign filter 0 to FIFO 0
    CAN1->FA1R |= CAN_FA1R_FACT0;
    CAN1->FMR &= ~CAN_FMR_FINIT;

    return 1;
}

int8_t CanReceive(volatile CANMessage* msg)
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

    uint32_t lowReg = CAN1->sFIFOMailBox[0].RDLR;
    uint32_t highReg = CAN1->sFIFOMailBox[0].RDHR;

    // Divide data to output structure

    msg->value_1 = lowReg;
    msg->value_2 = highReg;

    // Release pending message
    CAN1->RF0R |= CAN_RF0R_RFOM0;

    return 1; // Message received
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CanReceive(&CAN_Message);
    CAN_Status.data_ready = 1;

}