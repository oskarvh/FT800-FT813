/*
@file    EVE_target_RA4M3.h
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

Changes added in by Oskar von Heideken to support Renesas RA4M3 chipset

*/

#ifndef EVE_TARGET_RA4M3_H
#define EVE_TARGET_RA4M3_H

#if defined (EK_RA4M3) || defined(COMSCICALC_CM_V0)


//#define DIRECT_SPI
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

// Renesas specific includes
#include "bsp_api.h"
#include "r_ioport.h"
#include "common_data.h"
#include "r_sci_spi.h"
#include "hal_data.h"
#include "renesas_utils.h"

#if !defined (CS_PIN)
#if defined(EK_RA4M3)
#define CS_PIN BSP_IO_PORT_01_PIN_13
#endif

#if defined(COMSCICALC_CM_V0)
#define CS_PIN BSP_IO_PORT_01_PIN_03
#endif
#endif

#if !defined (PDN_PIN)
#if defined(EK_RA4M3)
#define PDN_PIN BSP_IO_PORT_01_PIN_15
#endif

#if defined(COMSCICALC_CM_V0)
#define PDN_PIN BSP_IO_PORT_04_PIN_02
#endif
#endif
#define BSP_DELAY_UNITS_MILLISECONDS 1000


void EVE_SPI_Init(void);


static inline void DELAY_MS(uint16_t val)
{
    R_BSP_SoftwareDelay(val, BSP_DELAY_UNITS_MILLISECONDS);
}



static inline void EVE_pdn_set(void)
{
       /* Power-Down low */
       R_IOPORT_PinWrite(&g_ioport_ctrl, PDN_PIN, BSP_IO_LEVEL_LOW);
}

static inline void EVE_pdn_clear(void)
{
       /* Power-Down high */
       R_IOPORT_PinWrite(&g_ioport_ctrl, PDN_PIN, BSP_IO_LEVEL_HIGH);
}

static inline void EVE_cs_set(void)
{
    /* CS low */
    R_IOPORT_PinWrite(&g_ioport_ctrl, CS_PIN, BSP_IO_LEVEL_LOW);
}

static inline void EVE_cs_clear(void)
{
    /* CS high */
    R_IOPORT_PinWrite(&g_ioport_ctrl, CS_PIN, BSP_IO_LEVEL_HIGH);
}

static inline void spi_transmit(uint8_t data)
{
    spiSend(&g_spi2_ctrl, &data, sizeof(uint8_t), SPI_BIT_WIDTH_8_BITS);
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
    uint8_t rxData = 0;
    spiSendReceive(&g_spi2_ctrl, &data ,&rxData, 1, SPI_BIT_WIDTH_8_BITS);
    return rxData;
}

static inline uint8_t fetch_flash_byte(const uint8_t *data)
{
    return *data;
}

#endif // EK_RA4M3 or COMSCICALC_CM_V0

#endif /* EVE_TARGET_RA4M3_H */
