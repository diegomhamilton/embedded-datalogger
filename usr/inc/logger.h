#ifndef _LOGGER_H
#define _LOGGER_H

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "usrconf.h"
#include "ff.h"
/* Include data logger channels */
#include "loggerconf.h"
#include "logger_timing.h"
#include "logger_analog_ch.h"
#include "buffer.h"

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
FRESULT sd_mount(FATFS *fs, MMCDriver *mmcd);
// static FRESULT scan_files(BaseSequentialStream *chp, char *path);
bool sd_init(MMCDriver *mmcd, MMCConfig *mmcconfig);
FRESULT init_files(void);
FRESULT init_folders(char *path, size_t n);

/*===========================================================================*/
/* Logger definitions.                                                              */
/*===========================================================================*/

#define FILENAME_LEN    12                                  // Max. number of characters for filename
#define DIRNAME_LEN     FILENAME_LEN + 2                    // Max. number of characters for directories
#define FULLPATH_LEN    FILENAME_LEN + DIRNAME_LEN + 4      // Max. mumber of characters for full path

extern thread_t *logging_thread;

extern THD_WORKING_AREA(waLogThread, 2048);
extern THD_FUNCTION(LogThread, arg);
void logger_start(void);

#endif
