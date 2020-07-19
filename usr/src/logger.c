#include "logger.h"

/* 
 * SD Logging Thread
 */
thread_t *logging_thread;

THD_WORKING_AREA(waLogThread, 2048);
THD_FUNCTION(LogThread, arg) {
    chprintf((BaseSequentialStream *)&SD1, "Enter Logging thread...\r\n");
    /* Error check variables */
    FRESULT err;
    uint32_t bw;
    /* File pointers */
    FIL fp;

    while (true) {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        chprintf((BaseSequentialStream *)&SD1, "Opening file: %34s\r\n", filepaths[0]);
        err = f_open(&fp, filepaths[0], FA_OPEN_APPEND | FA_WRITE);

        if (evt & EVT_ADC_HALF_BUFFER) {
            f_write(&fp, (void *)io_analog_buffer, 512, (UINT *)&bw);
        }
        else if (evt & EVT_ADC_FULL_BUFFER) {
            f_write(&fp, (void *)(&io_analog_buffer[32]), 512, (UINT *)&bw);
        }

        f_close(&fp);

        chprintf((BaseSequentialStream *)&SD1, "open = %d, written %d\r\n", err, bw);
    }
}

/* MMC configurations */
static MMCConfig mmccfg = {SPI_DRIVER, &ls_spicfg, &hs_spicfg};
MMCDriver MMCD1;

/* 
 * Initialize Logger file structure: SD, FileSystem, folders and files.
 */
bool logger_init(void) {
    return (sd_init(&MMCD1, &mmccfg) && (init_files() == FR_OK));
}

/*
 * Start Logger channels.
 */
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

    chprintf((BaseSequentialStream *)&SD1, "Trying to connect\r\n");
    /* Wait for SD card connection. */
    while (mmcConnect(mmcd)) {
        //TODO: implement card insertion detection.
        chprintf((BaseSequentialStream *)&SD1, "Card not connected\r\n");
        chThdSleepMilliseconds(1000);
    }
    chprintf((BaseSequentialStream *)&SD1, "Connected?\r\n");

    /* Mount SD FileSystem. */
    FRESULT res = sd_mount(&SDC_FS, mmcd);
    if (res == FR_OK) {
        return true;
    } else {
        chprintf((BaseSequentialStream *)&SD1, "Card isn't FAT formatted\r\n");
        return false;
    }
}

/*
 * Start MMC over SPI.
 */
void mmc_start_with_spi(MMCDriver *mmcd, MMCConfig *cfg) {
    spi_pin_config();
    mmcObjectInit(mmcd);
    mmcStart(mmcd, cfg);
}

/*
 * Setup I/O pins of SPI.
 */
void spi_pin_config(void) {
    palSetPadMode(SPI_SCK_PORT, SPI_SCK_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL);   // SCK
    palSetPadMode(SPI_MISO_PORT, SPI_MISO_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL); // MISO
    palSetPadMode(SPI_MOSI_PORT, SPI_MOSI_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL); // MOSI
    palSetPadMode(SPI_CS_PORT, SPI_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL);              // CS
    /* Enable Chip Select. If other SPI devices are connected to the same peripheralm,
     * be aware of setting the correct Chip Select during each transfer.
     */
    palSetPad(SPI_CS_PORT, SPI_CS_PIN);
}

/*
 * Mount FatFS on SD.
 */
FRESULT sd_mount(FATFS *fs, MMCDriver *mmcd) {
    FRESULT res;

    res = f_mount(fs, "/", 1);
    if (res != FR_OK) {
        chprintf((BaseSequentialStream *)&SD1, "Disconnecting...\r\n");
        mmcDisconnect(mmcd);
    } else {
        chprintf((BaseSequentialStream *)&SD1, "FatFS mounted.\r\n");
    }

    return res;
}

static char filepaths[NUM_OF_FILES][FULLPATH_LEN];
/*
 * Initialize all folders and files used in current logger session.
 */
FRESULT init_files(void) {
    char dir[DIRPATH_LEN];
    FIL fp;
    FRESULT res;
    
    res = init_folders(dir, DIRPATH_LEN);
    chprintf((BaseSequentialStream *)&SD1, "Folder %12s initialization RESULT = %d\r\n", dir, res);
    /* Initialize files if folder was succesfully created */
    if (res == FR_OK) {
        for(int i = 0; i < NUM_OF_FILES; i++) {
            chsnprintf(filepaths[i], FULLPATH_LEN, "/%s/%s", dir, filenames[i]);
            /* Create a new file and overwrite if existing */
            res = f_open(&fp, filepaths[i], FA_CREATE_ALWAYS | FA_WRITE);
            chprintf((BaseSequentialStream *)&SD1, "Initializing file: %30s, RESULT = %d\r\n", filepaths[i], res);
            f_close(&fp);
            if (res) {
                chprintf((BaseSequentialStream *)&SD1, "Error creating file.\r\n");
                break;
            }
        }
    }

    return res;
}

/*
 * Initialize folders and subfolders relevant to the current logger session. 
 */
FRESULT init_folders(char *path, size_t n) {
    char dir[n];
    //TODO: create dir based on date
    chsnprintf(dir, n, "%s", "XXYYZZ");
    FRESULT res;
    uint8_t dir_count = 0;

    res = f_mkdir(dir);
    /* If directory couldn't be initialized, return error */
    if ((res == FR_OK) || (res == FR_EXIST)) {
        do {
            //TODO: replace XXYYZZ with current date
            chsnprintf(path, n, "%s/%03d", dir, dir_count);
            res = f_mkdir(path);
            chprintf((BaseSequentialStream *)&SD1, "Open dir: %12s, RESULT = %d\r\n", path, res);
            dir_count += 1;
            
            if (dir_count == 0) {
                /* All 256 sub folders (dir_count limited) already exists. 
                 * This is the only case where init_folders returns FR_EXIST.
                 */
                break;
            }
        } while(res == FR_EXIST);
    }

    return res;
}

// static FRESULT scan_files(BaseSequentialStream *chp, char *path) {
//     static FILINFO fno;
//     FRESULT res;
//     DIR dir;
//     size_t i;
//     char *fn;

//     chprintf(chp, "scanning files\r\n");
//     chThdSleepMilliseconds(1000);

//     res = f_opendir(&dir, path);
//     if (res == FR_OK) {
//         i = strlen(path);
//         while (((res = f_readdir(&dir, &fno)) == FR_OK) && fno.fname[0]) {
//             if (FF_FS_RPATH && fno.fname[0] == '.') {
//                 chprintf(chp, "first if\r\n");
//                 continue;
//             }

//             fn = fno.fname;
//             if (fno.fattrib & AM_DIR) {
//                 *(path + i) = '/';
//                 strcpy(path + i + 1, fn);
//                 chprintf(chp, "before recursive call\r\n");
//                 chThdSleepMilliseconds(200);
//                 res = scan_files(chp, path);
//                 chprintf(chp, "after recursive call\r\n");
//                 chThdSleepMilliseconds(200);
                
//                 *(path + i) = '\0';
//                 if (res != FR_OK)
//                     break;
//             } else {
//                 chprintf(chp, "%s/%s\r\n", path, fn);
//             }
//         }
//     }
//     return res;
// }