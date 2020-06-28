#ifndef _USER_CONF_H
#define _USER_CONF_H

#include "hal.h"
#include "mcuconf.h"

#ifdef STM32_SPI_USE_SPI2
#define SPI_SCK_PORT    GPIOB
#define SPI_SCK_PIN     13
#define SPI_MISO_PORT   GPIOB
#define SPI_MISO_PIN    14
#define SPI_MOSI_PORT   GPIOB
#define SPI_MOSI_PIN    15
#define SPI_CS_PORT     GPIOB
#define SPI_CS_PIN      12
#define SPI_DRIVER      (SPIDriver *)&SPID2
#endif

/*
 * SPI Configurations
 *
 * Maximum speed SPI configuration (18MHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig hs_spicfg = {
    false,
    NULL,
    SPI_CS_PORT,
    SPI_CS_PIN,
    0,
    0
};

/*
 * Low speed SPI configuration (281.250kHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig ls_spicfg = {
    false,
    NULL,
    SPI_CS_PORT,
    SPI_CS_PIN,
    SPI_CR1_BR_2 | SPI_CR1_BR_1,
    0
};


#endif