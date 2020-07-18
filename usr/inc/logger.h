#ifndef _LOGGER_H
#define _LOGGER_H

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "usrconf.h"
#include "ff.h"
#include "logger_timing.h"
#include "logger_analog_ch.h"

/*
 * Events to wake logger thread when a block is ready to be saved.
 */
#define EVT_ADC_HALF_BUFFER EVENT_MASK(0)   // ADC Half Buffer complete event
#define EVT_ADC_FULL_BUFFER EVENT_MASK(1)   // ADC Full Buffer complete event

/*===========================================================================*/
/* Analog I/O                                                                */
/*===========================================================================*/

#define IO_ANALOG_NUM_CHANNELS      8
#define IO_DIGITAL_NUM_CHANNELS     8
#define IO_ANALOG_BUFFER_DEPTH      60      // 60, 32 samples = 512 by. This gives us a half-buffer of 30 samples, leaving 4 bytes for timestamp (SD block has 512 bytes)

#define LOGGER_FREQUENCY    200
#define LOGGER_TIMER_PRE    TIMER_FREQUENCY/LOGGER_FREQUENCY

/*
 * ADC buffer
 */
extern adcsample_t io_analog_buffer[IO_ANALOG_BUFFER_DEPTH * IO_ANALOG_NUM_CHANNELS];

/*===========================================================================*/
/* SD related settings.                                                              */
/*===========================================================================*/

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

/**
 * @brief FS object.
 */
static FATFS SDC_FS;
#define is_fs_ready(fs) fs->fs_type

void mmc_start_with_spi(MMCDriver *mmcd, MMCConfig *cfg);
void spi_pin_config(void);

/* 
* SD/FS related functions.
*/
bool logger_init(void);
BYTE sd_mount(FATFS *fs, MMCDriver *mmcd);
static FRESULT scan_files(BaseSequentialStream *chp, char *path);
bool sd_init(MMCDriver *mmcd, MMCConfig *mmcconfig);

/*===========================================================================*/
/* Logger definitions.                                                              */
/*===========================================================================*/

extern thread_t *logging_thread;

extern THD_WORKING_AREA(waLogThread, 2048);
extern THD_FUNCTION(LogThread, arg);
void logger_start(void);

#endif
