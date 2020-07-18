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
#include "usrconf.h"
#include "logger.h"

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
    sdStart(&SD1, NULL);
    chprintf((BaseSequentialStream *)&SD1, "Ahoy, i'm alive \r\n");

    bool sd_init_succeed = logger_init();

    logging_thread = chThdCreateStatic(waLogThread, sizeof(waLogThread), NORMALPRIO + 2, LogThread, NULL);

    // FRESULT err;

    // char b[1000];
    // char c[12] = "/oi123.txt";
    // // c[0] = 0;
    // b[0] = 0;

    // FIL fp;
    // uint32_t bw;
    // err = f_open(&fp, c, FA_CREATE_NEW | FA_WRITE);
    // f_write(&fp, (void *)b, sizeof(b), (UINT *)&bw);
    // chprintf((BaseSequentialStream *)&SD1, "open = %d, written %d\r\n", err, bw);
    // f_close(&fp);
    // scan_files((BaseSequentialStream *)&SD1, (char *)&b);
    chprintf((BaseSequentialStream *)&SD1, "Thread was terminated\r\n");

    logger_start();

    while (true) {
        chThdSleepMilliseconds(500);
    }
}
