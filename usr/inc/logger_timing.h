#ifndef _LOGGER_TIMING_H
#define _LOGGER_TIMING_H

#include "hal.h"
#include "logger.h"

void logger_timing_start(void);
void io_timer_cb(void);
void io_timer_init(GPTConfig *timerconfig);
void io_timer_start(void);

#define TIMER_FREQUENCY     50000

static const GPTConfig xTIM3Config = {
    /* Frequency */
    TIMER_FREQUENCY,
    /* Callback */
    io_timer_cb,
    0,
    /* Dier */
    TIM_DIER_TIE | TIM_DIER_UIE
};

#endif
