// LCD setup and communication management (header file)
// Does display init and queues commands & data to
// ring buffer that is emptied in TIM4 ISR

#ifndef _LCD_H_

#define _LCD_H_

// initial setup functions
void lcd_setup_buffer(void);	// must be called first
void lcd_setup_hw(void);

// get buffer data and put it on GPIO port
void lcd_hw_write(void);
void lcd_hw_write_finalize(void);

// put a character or a command byte in buffer
void lcd_write_char(uint8_t);
void lcd_write_cmd(uint8_t);

// print hex values if different sizes to the buffer
void lcd_write_hex8(uint8_t);
void lcd_write_hex16(uint16_t);
void lcd_write_hex32(uint32_t);

// put display cursor in a specified place
void lcd_place_cursor(uint8_t, uint8_t);

// load custom characters into the display
void lcd_write_custom_chars();

#endif