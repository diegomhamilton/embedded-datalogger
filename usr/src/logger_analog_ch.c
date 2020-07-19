#include "logger_analog_ch.h"

adcsample_t io_analog_buffer[IO_ANALOG_BUFFER_DEPTH * IO_ANALOG_NUM_CHANNELS];

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