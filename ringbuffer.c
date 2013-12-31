// Generic ring buffer management

// io/data ring buffer functions & defs
#include <stm32f10x.h> 
#include "ringbuffer.h"

void rb_buffer_init(struct ringbuf *buf, uint32_t size) {
	buf->size = size-1 ;
	buf->r_ptr = 0;
	buf->w_ptr = 0;
}

// The following functions take these arguments:
//	1) pointer to buffer control struct

inline uint32_t rb_is_writeable(struct ringbuf *buf) {
	// see if buffer is writeable (ie r_ptr != w_ptr)
	return ( (buf->r_ptr-buf->w_ptr-1 ) & buf->size );
}

inline uint32_t rb_is_readable(struct ringbuf *buf) {
	// buffer readable? 
	return ( (buf->w_ptr-buf->r_ptr) & buf->size );
}

// The following functions take these arguments:
//	1) pointer to buffer control struct
//	2) pointer to actual buffer element 0
//	3) byte or word to write

inline void rb_write_8(struct ringbuf *buf, uint8_t *array, uint8_t byte) {
	// buf->buffer[buf->w_ptr] = byte;
	array[buf->w_ptr] = byte; 
	buf->w_ptr = (buf->w_ptr + 1) & buf->size;
}

inline void rb_write_16(struct ringbuf *buf, uint16_t *array, uint16_t word) {
	// buf->buffer[buf->w_ptr] = byte;
	array[buf->w_ptr] = word;
	buf->w_ptr = (buf->w_ptr + 1) & buf->size;
}

// The following functions take these arguments:
//	1) pointer to buffer control struct
//	2) pointer to actual buffer element 0

inline uint8_t rb_read_8(struct ringbuf *buf, uint8_t *array) {
	// uint8_t value = buf->buffer[buf->r_ptr];
	volatile uint8_t value = array[buf->r_ptr];
	buf->r_ptr = (buf->r_ptr + 1) & buf->size;
	return(value);
}

inline uint16_t rb_read_16(struct ringbuf *buf, uint16_t *array) {
	// uint8_t value = buf->buffer[buf->r_ptr];
	volatile uint16_t value = array[buf->r_ptr];
	buf->r_ptr = (buf->r_ptr + 1) & buf->size;
	return(value);
}


