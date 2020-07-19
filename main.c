/*
 * Embedded Logger v0
 * Author: Diego Hamilton
 *
 * This project uses a STM32F103C8T6 and a SD card to store data of
 * multiple channels, such as: analog sensors, digital level sensors,
 * frequency sensors, CAN bus messages, IMU data (acceleration and
 * angular speed) and GPS.
 * 
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "usrconf.h"
#include "logger.h"

/*
 * Application entry point.
 */
int main(void) {
    /* Hardware Abstraction Layer initialization */
    halInit();
    /* ChibiOS initialization */
    chSysInit();

    /* 
     * Initialize Serial Driver 1 in the serial driver 1 with default configuration.
     * Pins PA9(TX) and PA10(RX) are used.
     */
    sdStart(&SD1, NULL);
    chprintf((BaseSequentialStream *)&SD1, "Ahoy, Logger is alive \r\n");

    /* Initialize Logger */
    bool res = logger_init();
    if (res == true) {
        //TODO: add verification of "start logging" (maybe a button?)
        /* If initialization succeeds, start logging thread and channels */
        logging_thread = chThdCreateStatic(waLogThread, sizeof(waLogThread), NORMALPRIO + 2, LogThread, NULL);
        logger_start();
    }

    while (true) {
        /* Currently the main thread doesn't executes tasks */
        chprintf((BaseSequentialStream *)&SD1, "Block buffer occupancy: %d\r\n", buffer_occupancy(block_buffer));
        chThdSleepMilliseconds(500);
    }
}
