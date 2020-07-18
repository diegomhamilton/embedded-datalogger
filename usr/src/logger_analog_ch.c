#include "logger_analog_ch.h"

adcsample_t io_analog_buffer[IO_ANALOG_BUFFER_DEPTH * IO_ANALOG_NUM_CHANNELS];

/*
 * ADC conversion group.
 * Mode:        Circular buffer, 8 samples of 2 channels, SW triggered.
 * Channels:    IN0, IN1, IN2, IN3, IN4, IN5, IN6, IN7.
 */
const ADCConversionGroup adcgrpcfg1 = {
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

void logger_analog_ch_start(void) {
    adcStart(&ADCD1, NULL);
    adcStartConversion(&ADCD1, &adcgrpcfg1, io_analog_buffer, IO_ANALOG_BUFFER_DEPTH);
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