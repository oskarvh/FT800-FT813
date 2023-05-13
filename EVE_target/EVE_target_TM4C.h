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

*/

#ifndef EVE_TARGET_TM4C_H
#define EVE_TARGET_TM4C_H

#if defined (__TI_ARM__)

#if defined (TIVAWARE)
// Using https://microcontrollerslab.com/spi-tm4c123-communication-between-tiva-launchpad-arduino/
//#define DIRECT_SPI

#include <ti/drivers/GPIO.h>
//#include <driverlib/gpio.h>
//#include <driverlib/pin_map.h>
#include "Board.h"
#include <stdint.h>
#include <stdbool.h>


#ifdef DIRECT_SPI
#include "TM4C123GH6PM.h"
#else
#include <ti/drivers/SPI.h>
#endif

SPI_Handle spiHandle;

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS GPIO_CS_PIN /* PA.6 */
#endif

#if !defined (EVE_PDN)
#define EVE_PDN GPIO_PDN_PIN /* PB.0 */
#endif

#if !defined (EVE_DELAY_1MS)
#define EVE_DELAY_1MS 8000U /* ~1ms at 48MHz Core-Clock */
#endif
/* you may define these in your build-environment to use different settings */

#define RIVERDI_PORT GPIO_PORT_P1
#define RIVERDI_SIMO BIT6   /* P1.6 */
#define RIVERDI_SOMI BIT7   /* P1.7 */
#define RIVERDI_CLK BIT5    /* P1.5 */

void EVE_SPI_Init(void);

static inline void DELAY_MS(uint16_t val)
{
    for (uint16_t loops = 0; loops < val; loops++)
    {
        for (uint16_t counter = 0; counter < EVE_DELAY_1MS; counter++)
        {
            __nop();
        }
    }
}

static inline void EVE_pdn_set(void)
{
//            GPIO_setOutputLowOnPin(EVE_PDN_PORT,EVE_PDN);   /* Power-Down low */
    GPIO_write(EVE_PDN, 0);   /* Power-Down low */
}

static inline void EVE_pdn_clear(void)
{
//            GPIO_setOutputHighOnPin(EVE_PDN_PORT,EVE_PDN);   /* Power-Down high */
    GPIO_write(EVE_PDN, 1);    /* Power-Down high */
}

static inline void EVE_cs_set(void)
{
//            GPIO_setOutputLowOnPin(EVE_CS_PORT,EVE_CS);   /* CS low */
    GPIO_write(EVE_CS, 0);   /* CS low */
}

static inline void EVE_cs_clear(void)
{
//            GPIO_setOutputHighOnPin(EVE_CS_PORT,EVE_CS);    /* CS high */
    GPIO_write(EVE_CS, 1);    /* CS high */
}

static inline void spi_transmit(uint8_t data)
{
//            SPI_transmitData(EUSCI_B0_BASE, data);
//            while (!(SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_B_SPI_TRANSMIT_INTERRUPT)));

    //UCB0TXBUF_SPI = data;
    //while (!(UCB0IFG_SPI & UCTXIFG)) {} /* wait for transmission to complete */
#ifdef DIRECT_SPI
#else
    SPI_Transaction transaction;
    transaction.count = 1;
    transaction.rxBuf = (void *) NULL;
    transaction.txBuf = (void *) &data;
    // Send command
    SPI_transfer(spiHandle, &transaction);
#endif
}

static inline void spi_transmit_32(uint32_t data)
{
#ifdef DIRECT_SPI
    // TBD: use this instead?: https://microcontrollerslab.com/spi-tm4c123-communication-between-tiva-launchpad-arduino/
    spi_transmit((uint8_t)(data & 0x000000ffUL));
    spi_transmit((uint8_t)(data >> 8U));
    spi_transmit((uint8_t)(data >> 16U));
    spi_transmit((uint8_t)(data >> 24U));
#else
    SPI_Transaction transaction;
    transaction.count = 4;
    transaction.rxBuf = (void *) NULL;
    transaction.txBuf = (void *) &data;
    // Send command
    SPI_transfer(spiHandle, &transaction);
#endif
}

/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
static inline void spi_transmit_burst(uint32_t data)
{
    #if defined (EVE_DMA)
        EVE_dma_buffer[EVE_dma_buffer_index++] = data;
    #else
        spi_transmit_32(data);
    #endif
}

static inline uint8_t spi_receive(uint8_t data)
{
#ifdef DIRECT_SPI
#else
//            SPI_transmitData(EUSCI_B0_BASE, data);
//            while (!(SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_B_SPI_TRANSMIT_INTERRUPT)));
//            return EUSCI_B_CMSIS(EUSCI_B0_BASE)->RXBUF;
    uint8_t tmpRxBuf;
    SPI_Transaction transaction;
    transaction.count = 1;
    transaction.rxBuf = (void *) &tmpRxBuf;
    transaction.txBuf = (void *) &data;
    // Send command
    SPI_transfer(spiHandle, &transaction);
    return tmpRxBuf;
#endif
}

static inline uint8_t fetch_flash_byte(const uint8_t *data)
{
    return *data;
}

#endif /* TIVAWARE */

#endif /* __TI_ARM */

#endif /* EVE_TARGET_TM4C_H */
