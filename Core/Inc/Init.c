/*
 * Source file for Init functions
 *
 *
 *
 */

#include "Init.h"


void InitClock()
{
    // Set Clock

    RCC->CR |= RCC_CR_HSION;

    while (!(RCC->CR & RCC_CR_HSIRDY));

    // Configure waite states
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    //FLASH->ACR |= FLASH_ACR_LATENCY_0WS;

    // Select HSI as system clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    // Wait until clock is selected
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}
