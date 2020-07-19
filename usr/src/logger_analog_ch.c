#include "logger_analog_ch.h"

adcsample_t io_analog_buffer[IO_ANALOG_BUFFER_DEPTH * IO_ANALOG_NUM_CHANNELS];

void logger_analog_ch_start(void) {
    adcStart(&ADCD1, NULL);
    adcStartConversion(&ADCD1, &adcgrpcfg1, &io_analog_buffer[1 * IO_ANALOG_NUM_CHANNELS], IO_ANALOG_BUFFER_DEPTH);
}

static void io_adc_error_callback(ADCDriver *adcp, adcerror_t err) {
    (void)adcp;
    (void)err;
}

void io_adc_conv_callback(ADCDriver *adcp) {
    int8_t ret = 0;
    block_element_t tmp_block = {
        .blocks     = NULL,
        .blocks_tw  = 1,
        .src        = FIL_ANALOG
    };

    if (adcIsBufferComplete(adcp)) {
        /* Handle DMA full buffer complete */
        //TODO: add timestamp in io_analog_buffer[31]
        tmp_block.blocks = (void *) &io_analog_buffer[31 * IO_ANALOG_NUM_CHANNELS];
    } else {
        /* Handle DMA half buffer complete */
        //TODO: add timestamp in io_analog_buffer[0]
        tmp_block.blocks = (void *)io_analog_buffer;
    }

    chSysLockFromISR();
    buffer_push(block_buffer, tmp_block, ret);
    if (ret == 0) chSemSignal(&block_to_write_sem);
    chSysUnlockFromISR();
}