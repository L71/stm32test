// Generic ring buffer management (header file)

#ifndef _RINGBUFFER_H_

#define _RINGBUFFER_H_

// generic I/O buffer control structure
struct ringbuf {
	uint8_t size;	// must always be an even 2^N number.
	volatile uint8_t r_ptr;		// read pointer
	volatile uint8_t w_ptr;		// write pointer
};

void buffer_init(struct ringbuf *, uint8_t);

// The following functions take these arguments:
//	1) pointer to buffer control struct
uint8_t is_writeable(struct ringbuf *);

uint8_t is_readable(struct ringbuf *);

// The following functions take these arguments:
//	1) pointer to buffer control struct
//	2) pointer to actual buffer element 0
//	3) byte or word to write
void write_byte(struct ringbuf *, uint8_t *, uint8_t);

void write_word(struct ringbuf *, uint16_t *, uint16_t);

// The following functions take these arguments:
//	1) pointer to buffer control struct
//	2) pointer to actual buffer element 0
uint8_t read_byte(struct ringbuf *, uint8_t *);

uint16_t read_word(struct ringbuf *, uint16_t *);

#endif

