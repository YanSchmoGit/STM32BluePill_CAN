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
}
