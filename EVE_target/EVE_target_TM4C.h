/*
@file    EVE_target_MSP432.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2023-04-12
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2023 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

5.0
- extracted from EVE_target.h
- basic maintenance: checked for violations of white space and indent rules
- split up the optional default defines to allow to only change what needs changing thru the build-environment

Changes added in by Oskar von Heideken to support TM4C123 and TM4C129 chipsets

*/

#ifndef EVE_TARGET_TM4C_H
#define EVE_TARGET_TM4C_H

#if defined (__TI_ARM__)

#if defined (TIVAWARE)
// Using https://microcontrollerslab.com/spi-tm4c123-communication-between-tiva-launchpad-arduino/
//#define DIRECT_SPI
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS_PORT GPIO_PORTA_BASE /* PA.6 */
#define EVE_CS GPIO_PIN_6
#endif

#if !defined (EVE_PDN)
#define EVE_PDN_PORT GPIO_PORTB_BASE /* PB.0 */
#define EVE_PDN GPIO_PIN_0
#endif

#if !defined (EVE_DELAY_1MS)
#define EVE_DELAY_1MS 8000U /* ~1ms at 48MHz Core-Clock */
#endif


void EVE_SPI_Init(void);

static inline void DELAY_MS(uint16_t val)
{
#if 0 //portTICK_PERIOD_MS
     const TickType_t xDelay = val / portTICK_PERIOD_MS;
     vTaskDelay( xDelay );
#else
     for(uint32_t i = 0 ; i < val*EVE_DELAY_1MS ; i++){
         //asm("NOP");
     }
#endif

}

static inline void EVE_pdn_set(void)
{
    GPIOPinWrite(EVE_PDN_PORT, EVE_PDN, 0);   /* Power-Down low */
}

static inline void EVE_pdn_clear(void)
{
    GPIOPinWrite(EVE_PDN_PORT, EVE_PDN, EVE_PDN);   /* Power-Down high */
}

static inline void EVE_cs_set(void)
{
    GPIOPinWrite(EVE_CS_PORT, EVE_CS, 0);   /* CS low */
}

static inline void EVE_cs_clear(void)
{
    GPIOPinWrite(EVE_CS_PORT, EVE_CS, EVE_CS);    /* CS high */
}

static inline void spi_transmit(uint8_t data)
{
    SSIDataPut(SSI0_BASE, data);
    // Wait for FIFO to be empty
    while(SSIBusy(SSI0_BASE));
}

static inline void spi_transmit_32(uint32_t data)
{
    spi_transmit((uint8_t)(data & 0x000000ffUL));
    spi_transmit((uint8_t)(data >> 8U));
    spi_transmit((uint8_t)(data >> 16U));
    spi_transmit((uint8_t)(data >> 24U));
}

/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
static inline void spi_transmit_burst(uint32_t data)
{

    spi_transmit_32(data);

}

static uint8_t spi_receive(uint8_t data)
{
    // Empty the buffer
    uint8_t tmp;
    while(SSIDataGetNonBlocking(SSI0_BASE, &tmp));

    // Send the data to receive the answer
    spi_transmit(data);
    uint32_t tmpRxBuf;
    SSIDataGet(SSI0_BASE, &tmpRxBuf);
    return ((uint8_t)(tmpRxBuf&0xff));
}

static inline uint8_t fetch_flash_byte(const uint8_t *data)
{
    return *data;
}

#endif /* TIVAWARE */

#endif /* __TI_ARM */

#endif /* EVE_TARGET_TM4C_H */
