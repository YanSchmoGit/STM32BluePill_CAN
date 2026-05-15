/*
 * Source file for Init functions
 *
 *
 *
 */

#include "Init.h"


int8_t InitClockHSI()
{
    // Set Clock

    RCC->CR |= RCC_CR_HSION;

    while (!(RCC->CR & RCC_CR_HSIRDY))
    {
        ;
    }

    // Configure waite states
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    //FLASH->ACR |= FLASH_ACR_LATENCY_0WS;

    // Select HSI as system clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    // Wait until clock is selected
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
    {
        ;
    }

    return 1;
}

int8_t InitClockHSE()
{
    // HSE (High Speed External) enable
    RCC->CR |= RCC_CR_HSEON;

    // Wait for stable HSE signal
    while (!(RCC->CR & RCC_CR_HSERDY))
    {
        ;
    }

    // Configure waite states
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_0;

    // Configure Bus-Prescaler  (AHB, APB1, APB2) - 8 MHz
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);

    // Set HSE as system clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSE;

    // Wait for HSE system clock being acknowledged
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE)
    {
     ;
    }

    // (Optional) Disable HSI to safe energy
    RCC->CR &= ~RCC_CR_HSION;

    return 1;
}
