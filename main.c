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

/*===========================================================================*/
/* Digital and Analog I/O configuration.                                     */
/*===========================================================================*/
void io_timer_cb(void);
void io_timer_init(GPTConfig *timerconfig);
void io_timer_start(void);
static void io_adc_error_callback(ADCDriver *adcp, adcerror_t err);
void io_adc_conv_callback(ADCDriver *adcp);

adcsample_t io_analog_buffer[IO_ANALOG_BUFFER_DEPTH * IO_ANALOG_NUM_CHANNELS];
/*
 * ADC conversion group.
 * Mode:        Circular buffer, 8 samples of 2 channels, SW triggered.
 * Channels:    IN0, IN1, IN2, IN3, IN4, IN5, IN6, IN7.
 */
static const ADCConversionGroup adcgrpcfg1 = {
    TRUE,
    IO_ANALOG_NUM_CHANNELS,
    io_adc_conv_callback,
    io_adc_error_callback,
    0, ADC_CR2_EXTTRIG | (0b100 << ADC_CR2_EXTSEL_Pos),                         /* CR1, CR2 */
    0,
    0,                            /* SMPR2 */
    ADC_SQR1_NUM_CH(IO_ANALOG_NUM_CHANNELS),
    ADC_SQR2_SQ7_N(ADC_CHANNEL_IN6) | ADC_SQR2_SQ8_N(ADC_CHANNEL_IN7),           /* SQR2 */
    ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN1)   | \
    ADC_SQR3_SQ3_N(ADC_CHANNEL_IN2) | ADC_SQR3_SQ4_N(ADC_CHANNEL_IN3)    | \
    ADC_SQR3_SQ5_N(ADC_CHANNEL_IN4) | ADC_SQR3_SQ6_N(ADC_CHANNEL_IN5)      \
};

static const GPTConfig xTIM3Config = {
    /* Frequency */
    TIMER_FREQUENCY,
    /* Callback */
    io_timer_cb,
    0,
    /* Dier */
    TIM_DIER_TIE | TIM_DIER_UIE
};

/*===========================================================================*/
/* Main and generic code.                                                    */
/*===========================================================================*/
void logger_start(void);

/*===========================================================================*/
/* Threads.                                                    */
/*===========================================================================*/

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

void logger_start(void) {
    palSetPadMode(IOPORT3, 13, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(IOPORT3, 13);
    palClearPad(IOPORT3, 13);
    io_timer_init(&xTIM3Config);
    io_timer_start();
    adcStart(&ADCD1, NULL);
    adcStartConversion(&ADCD1, &adcgrpcfg1, io_analog_buffer, IO_ANALOG_BUFFER_DEPTH);
}

void io_timer_cb(void) {
    //TODO: Implement Digital readings
    return;
}

void io_timer_init(GPTConfig *timerconfig) {
    gptObjectInit(&GPTD3);
    gptStart(&GPTD3, timerconfig);
    /* TRGO Event */
    GPTD3.tim->CR2 &= ~TIM_CR2_MMS;
    GPTD3.tim->CR2 |= TIM_CR2_MMS_1;
}

void io_timer_start(void) {
    gptStartContinuous(&GPTD3, LOGGER_TIMER_PRE);
}

static void io_adc_error_callback(ADCDriver *adcp, adcerror_t err) {
    (void)adcp;
    (void)err;
}

void io_adc_conv_callback(ADCDriver *adcp) {
    eventmask_t events = 0;

    if (adcIsBufferComplete(adcp)) {
        /* Handle DMA full buffer complete */
        events |= EVT_ADC_FULL_BUFFER;
    } else {
        /* Handle DMA half buffer complete */
        events |= EVT_ADC_HALF_BUFFER;
    }

    chSysLockFromISR();
    chEvtSignalI(logging_thread, events);
    chSysUnlockFromISR();
    palTogglePad(IOPORT3, 13);
}