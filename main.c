/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <string.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "ff.h"


/*===========================================================================*/
/* FatFs related.                                                            */
/*===========================================================================*/

/**
 * @brief FS object.
 */
static FATFS SDC_FS;

/* FS mounted and ready.*/
static bool fs_ready = FALSE;

/*
 * Maximum speed SPI configuration (18MHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig hs_spicfg = {
  false,
  NULL,
  GPIOA,
  4,
  0,
  0
};

/*
 * Low speed SPI configuration (281.250kHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig ls_spicfg = {
  false,
  NULL,
  GPIOA,
  4,
  SPI_CR1_BR_2 | SPI_CR1_BR_1,
  0
};

/* MMC/SD over SPI driver configuration.*/
static MMCConfig mmccfg = {&SPID1, &ls_spicfg, &hs_spicfg};

static FRESULT scan_files(BaseSequentialStream *chp, char *path) {
  static FILINFO fno;
  FRESULT res;
  DIR dir;
  size_t i;
  char *fn;

  res = f_opendir(&dir, path);
  if (res == FR_OK) {
    i = strlen(path);
    while (((res = f_readdir(&dir, &fno)) == FR_OK) && fno.fname[0]) {
      if (FF_FS_RPATH && fno.fname[0] == '.')
        continue;
      fn 
      = fno.fname;
      if (fno.fattrib & AM_DIR) {
        *(path + i) = '/';
        strcpy(path + i + 1, fn);
        res = scan_files(chp, path);
        *(path + i) = '\0';
        if (res != FR_OK)
          break;
      }
      else {
        chprintf(chp, "%s/%s\r\n", path, fn);
      }
    }
  }
  return res;
}

/*===========================================================================*/
/* Main and generic code.                                                    */
/*===========================================================================*/

MMCDriver MMCD1;

/*
 * Application entry point.
 */
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 1 using the driver default configuration.
   * PA9(TX) and PA10(RX) are routed to USART1.
   */
  sdStart(&SD2, NULL);

  /*
   * SPI1 I/O pins setup.
   */
  palSetPadMode(IOPORT1, 5, PAL_MODE_STM32_ALTERNATE_PUSHPULL);     /* SCK. */
  palSetPadMode(IOPORT1, 6, PAL_MODE_STM32_ALTERNATE_PUSHPULL);     /* MISO.*/
  palSetPadMode(IOPORT1, 7, PAL_MODE_STM32_ALTERNATE_PUSHPULL);     /* MOSI.*/
  palSetPadMode(IOPORT1, 4, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPad(IOPORT1, 4);

  chprintf((BaseSequentialStream *)&SD2, "Ahoy, i'm alive \r\n");

  /*
   * Initializes the MMC driver to work with SPI2.
   */
  palSetPadMode(IOPORT2, GPIOB_PIN12, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPad(IOPORT2, GPIOB_PIN12);
  mmcObjectInit(&MMCD1);
  mmcStart(&MMCD1, &mmccfg);

  /*
   * On insertion SDC initialization and FS mount.
   */
  chprintf((BaseSequentialStream *)&SD2, "Trying to connect\r\n");
  while (mmcConnect(&MMCD1)) {
    chprintf((BaseSequentialStream *)&SD2, "Card not connected\r\n");
    chThdSleepMilliseconds(200);
  }
  chprintf((BaseSequentialStream *)&SD2, "Connected?\r\n");

  
  FRESULT err;

  err = f_mount(&SDC_FS, "/", 1);
  if (err != FR_OK) {
    chprintf((BaseSequentialStream *)&SD2, "Disconnecting\r\n");
    mmcDisconnect(&MMCD1);
  } else {
    chprintf((BaseSequentialStream *)&SD2, "TO suave\r\n");
    fs_ready = TRUE;
  }

  char b[100];
  char c[12] = "/oi123.txt";
  // c[0] = 0;
  b[0] = 0;

  FIL fp;
  uint32_t bw;
  err = f_open(&fp, c, FA_CREATE_NEW | FA_WRITE);
  f_write(&fp, (void *)b, sizeof(b), &bw);
  chprintf((BaseSequentialStream *)&SD2, "open = %d, written %d\r\n", err, bw);
  f_close(&fp);

  scan_files((BaseSequentialStream *)&SD2, &b);

  while (true) {
  }
}
