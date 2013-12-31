// Generic ring buffer management (header file)

#ifndef _RINGBUFFER_H_

#define _RINGBUFFER_H_

// generic I/O buffer control structure
struct ringbuf {
	uint32_t size;	// must always be an even 2^N number.
	volatile uint32_t r_ptr;		// read pointer
	volatile uint32_t w_ptr;		// write pointer
};

void rb_buffer_init(struct ringbuf *, uint8_t);

// The following functions take these arguments:
//	1) pointer to buffer control struct
uint32_t rb_is_writeable(struct ringbuf *);

uint32_t rb_is_readable(struct ringbuf *);

// The following functions take these arguments:
//	1) pointer to buffer control struct
//	2) pointer to actual buffer element 0
//	3) byte or word to write
void rb_write_8(struct ringbuf *, uint8_t *, uint8_t);

void rb_write_16(struct ringbuf *, uint16_t *, uint16_t);

// The following functions take these arguments:
//	1) pointer to buffer control struct
//	2) pointer to actual buffer element 0
uint8_t rb_read_8(struct ringbuf *, uint8_t *);

uint16_t rb_read_16(struct ringbuf *, uint16_t *);

#endif

