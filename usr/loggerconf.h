#ifndef _LOGGER_CONF_H
#define _LOGGER_CONF_H

#include "buffer.h"


/*
 * Block buffer definitions.
 */

typedef uint8_t block_t[512];

#define BLOCK_BUFFER_SIZE       (uint8_t)16
#define BLOCK_BUFFER_DEF(SIZE)  BUFFER_STRUCT_DEF(block_t, uint8_t, (SIZE))

typedef BLOCK_BUFFER_DEF(BLOCK_BUFFER_SIZE) block_buffer_t;

extern block_buffer_t block_buffer;

/*===========================================================================*/
/* Channels Configuration                                                    */
/*===========================================================================*/

#define IO_DIGITAL_NUM_CHANNELS     8

#define IO_ANALOG_NUM_CHANNELS      8
#define IO_ANALOG_BUFFER_DEPTH      60      // 60, 32 samples = 512 by. This gives us a half-buffer of 30 samples, leaving 4 bytes for timestamp (SD block has 512 bytes)

#define LOGGER_FREQUENCY            200
#define LOGGER_TIMER_PRE            TIMER_FREQUENCY/LOGGER_FREQUENCY

/*===========================================================================*/
/* Events to wake logger thread when a block is ready to be saved.           */
/*===========================================================================*/

#define EVT_ADC_HALF_BUFFER EVENT_MASK(0)   // ADC Half Buffer complete event
#define EVT_ADC_FULL_BUFFER EVENT_MASK(1)   // ADC Full Buffer complete event

#endif /* _LOGGER_CONF_H */