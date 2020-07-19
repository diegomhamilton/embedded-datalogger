#ifndef _LOGGER_CONF_H
#define _LOGGER_CONF_H

#define BUFFER_POP_EMPTY_ACTION(buf, c) ({})

/* Include buffer library and set buffer to circular mode. */
#define BUFFER_OVERWRITE_OLDEST_WHEN_FULL
#include "buffer.h"

/*
 * Block buffer definitions.
 */

typedef struct {
    void* blocks;    // Pointer to block(s)
    uint8_t blocks_tw;    // Blocks to write
    uint8_t src;    // File index
} block_element_t;

#define BLOCK_BUFFER_SIZE       16
#define BLOCK_BUFFER_DEF(SIZE)  BUFFER_STRUCT_DEF(block_element_t, uint8_t, (SIZE))

typedef BLOCK_BUFFER_DEF(BLOCK_BUFFER_SIZE) block_buffer_t;

extern block_buffer_t block_buffer;

/*===========================================================================*/
/* Channels Configuration                                                    */
/*===========================================================================*/

#define IO_DIGITAL_NUM_CHANNELS     8

#define IO_ANALOG_NUM_CHANNELS      8
#define LOGGER_FREQUENCY            200
#define LOGGER_TIMER_PRE            TIMER_FREQUENCY/LOGGER_FREQUENCY

/*===========================================================================*/
/* Events to wake logger thread when a block is ready to be saved.           */
/*===========================================================================*/

extern binary_semaphore_t block_to_write_sem;

#define NUM_OF_FILES    5

/*===========================================================================*/
/* File index of channels definitions.                                       */
/*===========================================================================*/

#define FIL_ANALOG      0
#define FIL_DIGITAL     1
#define FIL_IMU         2
#define FIL_GPS         3
#define FIL_CAN         4


#endif /* _LOGGER_CONF_H */