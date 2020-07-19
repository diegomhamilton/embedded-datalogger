#ifndef _LOGGER_ANALOG_CH_H
#define _LOGGER_ANALOG_CH_H

#include "ch.h"
#include "hal.h"
#include "loggerconf.h"

void logger_analog_ch_start(void);
static void io_adc_error_callback(ADCDriver *adcp, adcerror_t err);
void io_adc_conv_callback(ADCDriver *adcp);

/*
 * 1 block (512 by) = 32 samples (16 by).
 * We want to add timestamp in each block, so the maximum of samples is 31.
 */
#define IO_ANALOG_BUFFER_DEPTH      62

extern adcsample_t io_analog_buffer[IO_ANALOG_BUFFER_DEPTH * IO_ANALOG_NUM_CHANNELS];
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

extern thread_t *logging_thread;
extern block_buffer_t block_buffer;

#endif