#include "logger.h"

/* SD Logging Thread */
thread_t *logging_thread;

THD_WORKING_AREA(waLogThread, 2048);
THD_FUNCTION(LogThread, arg) {
    chprintf((BaseSequentialStream *)&SD1, "Enter Logging thread...\r\n");
    /* Error check variables */
    FRESULT err;
    uint32_t bw;
    /* File pointers */
    FIL fp_analog, fp_digital, fp_imu, fp_can, fp_gps;
    char filename[10] = "/an.dat";

    err = f_open(&fp_analog, filename, FA_CREATE_NEW | FA_WRITE);
    f_close(&fp_analog);

    err = f_open(&fp_analog, filename, FA_OPEN_APPEND | FA_WRITE);

    while (true) {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        
        if (evt & EVT_ADC_HALF_BUFFER) {
            f_write(&fp_analog, (void *)io_analog_buffer, 512, (UINT *)&bw);
        }
        else if (evt & EVT_ADC_FULL_BUFFER) {
            f_write(&fp_analog, (void *)(&io_analog_buffer[32]), 512, (UINT *)&bw);
        }
        
        chprintf((BaseSequentialStream *)&SD1, "open = %d, written %d\r\n", err, bw);
    }
}

/* MMC configurations */
static MMCConfig mmccfg = {SPI_DRIVER, &ls_spicfg, &hs_spicfg};
MMCDriver MMCD1;
/* 
 * MMC/SD over SPI driver configuration .
 */
bool logger_init(void) {
    return sd_init(&MMCD1, &mmccfg);
}

void logger_start(void) {
    palSetPadMode(IOPORT3, 13, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(IOPORT3, 13);
    palClearPad(IOPORT3, 13);
    logger_timing_start();
    logger_analog_ch_start();
}


/*
 * SD initialization.
 */
bool sd_init(MMCDriver *mmcd, MMCConfig *mmcconfig) {
    mmc_start_with_spi(mmcd, mmcconfig);

    /*
    * On insertion SDC initialization and FS mount.
    */
    chprintf((BaseSequentialStream *)&SD1, "Trying to connect\r\n");
    while (mmcConnect(mmcd)) {
        chprintf((BaseSequentialStream *)&SD1, "Card not connected\r\n");
        chThdSleepMilliseconds(1000);
    }
    chprintf((BaseSequentialStream *)&SD1, "Connected?\r\n");

    BYTE mount_succeeded = sd_mount(&SDC_FS, mmcd);
    if (mount_succeeded) {
        return true;
    } else {
        chprintf((BaseSequentialStream *)&SD1, "Card isn't FAT formatted\r\n");
        return false;
    }
}

void mmc_start_with_spi(MMCDriver *mmcd, MMCConfig *cfg) {
    spi_pin_config();
    mmcObjectInit(mmcd);
    mmcStart(mmcd, cfg);
}

void spi_pin_config(void) {
    /*
    * SPI I/O pins setup.
    */
    palSetPadMode(SPI_SCK_PORT, SPI_SCK_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL);   // SCK
    palSetPadMode(SPI_MISO_PORT, SPI_MISO_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL); // MISO
    palSetPadMode(SPI_MOSI_PORT, SPI_MOSI_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL); // MOSI
    palSetPadMode(SPI_CS_PORT, SPI_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL);              // CS
    palSetPad(SPI_CS_PORT, SPI_CS_PIN);                                            // Enable CS
}

BYTE sd_mount(FATFS *fs, MMCDriver *mmcd) {
    FRESULT err;

    err = f_mount(fs, "/", 1);
    if (err != FR_OK) {
        chprintf((BaseSequentialStream *)&SD1, "Disconnecting\r\n");
        mmcDisconnect(mmcd);
    } else {
        chprintf((BaseSequentialStream *)&SD1, "TO suave\r\n");
    }

    return is_fs_ready(fs);
}

static FRESULT scan_files(BaseSequentialStream *chp, char *path) {
    static FILINFO fno;
    FRESULT res;
    DIR dir;
    size_t i;
    char *fn;

    chprintf(chp, "scanning files\r\n");
    chThdSleepMilliseconds(1000);

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        while (((res = f_readdir(&dir, &fno)) == FR_OK) && fno.fname[0]) {
            if (FF_FS_RPATH && fno.fname[0] == '.') {
                chprintf(chp, "first if\r\n");
                continue;
            }

            fn = fno.fname;
            if (fno.fattrib & AM_DIR) {
                *(path + i) = '/';
                strcpy(path + i + 1, fn);
                chprintf(chp, "before recursive call\r\n");
                chThdSleepMilliseconds(200);
                res = scan_files(chp, path);
                chprintf(chp, "after recursive call\r\n");
                chThdSleepMilliseconds(200);
                
                *(path + i) = '\0';
                if (res != FR_OK)
                    break;
            } else {
                chprintf(chp, "%s/%s\r\n", path, fn);
            }
        }
    }
    return res;
}
