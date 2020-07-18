#include "logger_timing.h"

void logger_timing_start(void) {
    io_timer_init(&xTIM3Config);
    io_timer_start();
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