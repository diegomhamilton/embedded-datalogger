#ifndef _LOGGER_ANALOG_CH_H
#define _LOGGER_ANALOG_CH_H

#include "ch.h"
#include "hal.h"
#include "logger.h"

void logger_analog_ch_start(void);
static void io_adc_error_callback(ADCDriver *adcp, adcerror_t err);
void io_adc_conv_callback(ADCDriver *adcp);

/*
 * ADC conversion group.
 * Mode:        Circular buffer, 8 samples of 2 channels, SW triggered.
 * Channels:    IN0, IN1, IN2, IN3, IN4, IN5, IN6, IN7.
 */
extern const ADCConversionGroup adcgrpcfg1;

#endif