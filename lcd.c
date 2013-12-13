// LCD setup and communication management (header file)
// Does display init and queues commands & data to
// ring buffer that is emptied in TIM4 ISR

#include <stm32f10x.h> 

#include "ringbuffer.h"
#include "global.h"

// global defines & declarations

#define LCD_BUF_SIZE 128	// LCD character/command buffer (only buffers half bytes!)

// control struct for LCD buffer
struct ringbuf lcd_buf_str;
// the actual data
uint8_t lcd_buf[LCD_BUF_SIZE];

// LCD hardware setup (put display in
// known state and 4-bit data mode)
void lcd_setup_hw(void) {
	// put initialisation codes into lcd output buffer
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x03);
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x03);
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x03);
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x02);
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x02); // display function set, 4bit
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x0a); // 0x08
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x00); // display off
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x08); 
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x00); // display clear
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x01);
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x00); // cursor control / display on/off cmd
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x0c);
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x00); // address increment mode
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x06);
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x00); // home command
	write_byte(&lcd_buf_str, &lcd_buf[0], 0x02);
}

// initialize LCD data buffer
void lcd_setup_buffer(void) {
	buffer_init(&lcd_buf_str, LCD_BUF_SIZE);
}

// check if there is stuff in the LCD buffer, then copy it to GPIO port.
// should only be called from main ISR; no need to protect buffer read from interrupts.
inline void lcd_hw_write(void) {
	uint8_t byte;
	if (is_readable(&lcd_buf_str)) {	
		byte = read_byte(&lcd_buf_str, &lcd_buf[0]);
		GPIOC->BRR = 0x001f ; // clear GPIOC output bits 0-4
		GPIOC->BSRR = byte & 0x001f ;
		// set GPIOC bit 5, this is the E pin on the display
		GPIOC->BSRR = 0x0020 ;
	}
}

// do a H->L transition on the E pin on the display
// any lcd_hw_write call must be followed by this shortly after
inline void lcd_hw_write_finalize(void) {
	// make sure GPIOC bit 5 is L.
	// this H->L transition clocks data into the display.
    GPIOC->BRR = 0x0020 ;
}

// put a byte in the LCD data buffer (unless it's full!)
inline void lcd_byte_to_buffer(uint8_t byte) {
	if (is_writeable(&lcd_buf_str)) {
		write_byte(&lcd_buf_str, &lcd_buf[0], byte);
	} else {
		global_indicate_error(LCD_BUF_NOT_WR);
	}
}

// put a character in the data buffer
inline void lcd_write_char(uint8_t byte) {
	uint8_t msb=(byte & 0xf0);
	uint8_t lsb=(byte & 0x0f);
	msb >>= 4;
	// data mode -> bit 4 = H
	msb |= 0x10 ;
	lsb |= 0x10 ;
	lcd_byte_to_buffer(msb);
	lcd_byte_to_buffer(lsb);	
}

// put a command in the data buffer
inline void lcd_write_cmd(uint8_t byte) {
	uint8_t msb=(byte & 0xf0);
	uint8_t lsb=(byte & 0x0f);
	msb >>= 4;
	lcd_byte_to_buffer(msb);
	lcd_byte_to_buffer(lsb);
}

// print a hex value 0xNN
inline void lcd_write_hex8(uint8_t byte) {
	uint8_t msb=(byte & 0xf0);
	uint8_t lsb=(byte & 0x0f);
	msb >>= 4;
	// msb+= 0x30;
	// lsb+= 0x30;
	msb = msb < 10 ? msb+48 : msb+97-10 ;
	lsb = lsb < 10 ? lsb+48 : lsb+97-10 ;
	lcd_write_char(msb);
	lcd_write_char(lsb);
}

// print a hex value 0xNNNN
inline void lcd_write_hex16(uint16_t word) {
	uint8_t msb=(uint8_t)((word & 0xff00) >> 8);
	uint8_t lsb=(uint8_t)(word & 0x00ff);
	lcd_write_hex8(msb);
	lcd_write_hex8(lsb);
}

// print a hex value 0xNNNNNNNN
inline void lcd_write_hex32(uint32_t longword) {
	uint16_t msw=(uint16_t)((longword & 0xffff0000) >> 16);
	uint16_t lsw=(uint16_t)(longword & 0x0000ffff);
	lcd_write_hex16(msw);
	lcd_write_hex16(lsw);
}

// find out lcd display memory address of an x,y position
uint8_t lcd_get_addr(uint8_t x, uint8_t y) {
    // return display address of specified x,y coordinate
    uint8_t disp_addr=0x00;
    
    switch (y) {
            case 0x00 :
                    disp_addr=0x00;
                    break;
            case 0x01 :
                    disp_addr=0x40;
                    break;
            case 0x02 :
                    disp_addr=0x14;
                    break;
            case 0x03 :
                    disp_addr=0x54;
                    break;
    }
    disp_addr=disp_addr+x;
    return(disp_addr); 
}

// put the display cursor in a specific place
inline void lcd_place_cursor(uint8_t x, uint8_t y) {
    // put cursor on a specific address on the display
	uint8_t addr = lcd_get_addr(x, y);
    addr=addr+0x80;  // add address select bit to cmd code
	lcd_write_cmd(addr);
}

// load the custom charater table in display
void lcd_write_custom_chars() {
	uint8_t i;
	uint8_t charbits[] = { 	0b10000000 ,	// character 0. Mod/ctrl.
							0b10000000 ,
							0b00001000 ,
							0b00010101 ,
							0b00000010 ,
							0b10000000 ,
							0b10000000 ,
							0b10000000 ,
							0b10000000 ,	// character 1. KeyOn.
							0b00010101 ,
							0b00010101 ,
							0b00010101 ,
							0b00011111 ,
							0b00011111 ,
							0b00000000 ,
							0b00000000 ,
							0b00000000 ,	// character 2. KeyOff.
							0b00010001 ,
							0b00001010 ,
							0b00000100 ,
							0b00001010 ,
							0b00010001 ,
							0b00000000 ,
							0b00000000 ,
							0xff } ;		// end-of-table marker.
	uint8_t cgm_addr=0;
	lcd_write_cmd(cgm_addr | 0x40);	// set cgram address command
	for ( i=0 ; charbits[i] != 0xff ; i++ ) {
		lcd_write_char(charbits[i]);
	}
	
}


