#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

/*
 * Constant variables and functions definitions
 */

/* Default action for when poping an element with the buffer empty. */
#ifndef BUFFER_POP_EMPTY_ACTION
#define BUFFER_POP_EMPTY_ACTION(buf, c) (c) = -1
#endif /* BUFFER_POP_EMPTY_ACTION */

/* Default action for when pushing an element with the buffer full.
   For the case where we write over the oldest entry, note that there
   is no protection against a concurrent pop call */
#ifndef BUFFER_PUSH_FULL_ACTION
#ifdef BUFFER_OVERWRITE_OLDEST_WHEN_FULL
#define BUFFER_PUSH_FULL_ACTION(buf, c, ret_val) do {			\
	(ret_val) = 0;							\
	(buf).data[(buf).head] = (c);					\
	(buf).head = (buf).new_head;					\
	(buf).tail++;							\
	if ((buf).tail >= buffer_size(buf))				\
	    (buf).tail = 0;						\
    } while (0)
#else /* BUFFER_OVERWRITE_WHEN_FULL */
#define BUFFER_PUSH_FULL_ACTION(buf, c, ret_val) do {	\
    } while (0)
#endif /* BUFFER_OVERWRITE_WHEN_FULL */
#endif /*  BUFFER_PUSH_FULL_ACTION */


/*
 * Structs and types definitions
 */

#define BUFFER_STRUCT_DEF(ELEM_TYPE, IDX_TYPE, SIZE) struct {	\
	ELEM_TYPE data[SIZE];					\
	IDX_TYPE head;						\
	IDX_TYPE tail;						\
	IDX_TYPE new_head;					\
	IDX_TYPE size;						\
}

#define BYTE_BUFFER_DEF(SIZE) BUFFER_STRUCT_DEF(uint8_t, uint8_t, SIZE)
#define BYTE_LARGE_BUFFER_DEF(SIZE) BUFFER_STRUCT_DEF(uint8_t, uint16_t, SIZE)

/*
 * Macros to access the buffer
 */

#define buffer_reset(buf) do {						\
	(buf).head = (buf).tail = 0;					\
	(buf).size = sizeof((buf).data)/sizeof((buf).data[0]);		\
    } while (0)

#define buffer_size(buf) ((buf).size)

#define buffer_occupancy(buf) (((buf).head >= (buf).tail ? 0 : buffer_size(buf)) \
			      + (buf).head - (buf).tail)

#define buffer_free_space(buf) buffer_size(buf) - 1 - buffer_occupancy(buf)

#define is_buffer_empty(buf) ((buf).head == (buf).tail)

#define is_buffer_full(buf) ((buf).head + 1 >= buffer_size(buf) ?	\
			     (buf).tail == 0 : (buf).head + 1 == (buf).tail)

#define buffer_generic_push(buf, c, ret_val, full_action, pos_push_action) do { \
	(buf).new_head = (buf).head + 1;				\
	if ((buf).new_head >= buffer_size(buf))				\
	    (buf).new_head = 0;						\
	if ((buf).new_head != (buf).tail) {				\
	    (buf).data[(buf).head] = (c);				\
	    (buf).head = (buf).new_head;				\
	    (ret_val) = 0;						\
	    pos_push_action(buf, c, ret_val);				\
	} else {							\
	    (ret_val) = -1;						\
	    full_action(buf, c, ret_val);				\
	}								\
    } while (0)

#define buffer_push(buf, c, ret_val) \
    buffer_generic_push(buf, c, ret_val, BUFFER_PUSH_FULL_ACTION, {})

#define buffer_generic_pop(buf, c, empty_action, pos_pop_action) do {	\
	if ((buf).head == (buf).tail) {					\
	    empty_action(buf, c);					\
	}								\
	else {								\
	    (c) = (buf).data[(buf).tail];				\
	    if (++((buf).tail) >= buffer_size(buf))			\
		(buf).tail = 0;						\
	    pos_pop_action(buf, c);					\
	}								\
    } while (0)

#define buffer_pop(buf, c) buffer_generic_pop(buf, c, BUFFER_POP_EMPTY_ACTION, {})

#endif /* BUFFER_H */