// midi input/output HW interfaceing and processing 

#include <stm32f10x.h> 

#include "ringbuffer.h"
#include "lcd.h"
#include "spi_dac.h"
#include "synth_core.h"
#include "global.h"

// global define values and data structures

// MIDI state defines
#define MIDI_STATUS_NONE                0

#define MIDI_STATUS_KEYON               1
#define MIDI_STATUS_KEYOFF              2
#define MIDI_WAIT_FOR_BYTE1             3
#define MIDI_WAIT_FOR_BYTE2             4
#define MIDI_STATUS_CTRL				5
#define MIDI_STATUS_PITCH_WHEEL			6
#define MIDI_STATUS_UNIMPLEMENTED       255

// MIDI command codes (channel info stripped; bits 4-7 only)
#define MIDI_KEYOFF_MSG                 0x80
#define MIDI_KEYON_MSG                  0x90
#define MIDI_AFTERTOUCH                 0xa0
#define MIDI_CTRL_MSG                   0xb0
#define MIDI_PRG_CHANGE                 0xc0
#define MIDI_CHAN_PRESSURE              0xd0
#define MIDI_PITCH_WHEEL                0xe0

// Sound event codes; internal events
#define MIDI_GOT_KEYON                  50
#define MIDI_GOT_KEYOFF                 51
#define MIDI_GOT_OTHER                  52
#define MIDI_GOT_CTRL					53
#define MIDI_GOT_PITCH_WHEEL			54
#define MIDI_INCOMPLETE                 55


// size of midi recieve buffer
#define MIDI_REC_BUF_SIZE 128	
// MIDI input data buffer control struct
struct ringbuf midi_recbuf_str; 
// the actual data buffer
uint8_t midi_recbuf[MIDI_REC_BUF_SIZE];


// STM32 USART1 setup
void midi_setup_hw(void) {
	// USART1 reciever setup
	USART1->CR1 |= USART_CR1_UE ;	// enable USART1
	USART1->BRR	= 0x900 ;	// set data rate 31250 bits/s
	USART1->CR1 |= USART_CR1_RE ;	// enable USART1 reciever
}

// initialize MIDI data buffer
void midi_setup_buffers(void) {
	rb_buffer_init(&midi_recbuf_str, MIDI_REC_BUF_SIZE);
}

// put a byte in the MIDI buffer
inline void midi_buffer_byte(uint8_t byte) {
	if (rb_is_writeable(&midi_recbuf_str)) {
		rb_write_8(&midi_recbuf_str, &midi_recbuf[0], byte);
	} else {
		global_indicate_error(MIDI_BUF_NOT_WR);
	}
}

// MIDI data input processor
inline void midi_process_buffer(void) {
		
	// MIDI global state keeper:
	static uint8_t midi_main_state;
	static uint8_t midi_sub_state;
	uint8_t key_state = MIDI_GOT_OTHER;

	static uint8_t byte1;
	static uint8_t byte2;
	// static uint8_t channel;
//	uint8_t i;
	uint8_t last_in;
// 	uint8_t last_in=uart_in;
	
	uint8_t maxframes=8 ;	// max bytes to process in one go.
	
	while (rb_is_readable(&midi_recbuf_str)) {
		__disable_irq();	// make sure we have exclusive access to buffer while reading it.
		last_in=rb_read_8(&midi_recbuf_str,&midi_recbuf[0]);
		__enable_irq();
		maxframes -- ;
// 		if ( last_in != 0xfe ) {  // MIDI event counter. Skip "active sensing" messages (if any).
// 			midi_event_counter ++;
// 		}

		cpu_load_led_on();	
	
		// lcd_place_cursor(0,0);
		// lcd_write_hex8(last_in);
		
		if ( last_in & 0x80 ) {	// If this is a status message... :
			// channel = last_in & 0x0f ;
			switch ( last_in & 0xf0 ) {	// which voice/channel message is it?
				case MIDI_KEYON_MSG :	// Key On
					midi_main_state = MIDI_STATUS_KEYON;
					midi_sub_state = MIDI_WAIT_FOR_BYTE1;
					key_state = MIDI_INCOMPLETE;
					break;
				case MIDI_KEYOFF_MSG :	// Key Off
					midi_main_state = MIDI_STATUS_KEYOFF;
					midi_sub_state = MIDI_WAIT_FOR_BYTE1;
					key_state = MIDI_INCOMPLETE;
					break;
				case MIDI_PITCH_WHEEL :	// MIDI pitch wheel
					midi_main_state = MIDI_STATUS_PITCH_WHEEL;
					midi_sub_state = MIDI_WAIT_FOR_BYTE1;
					key_state = MIDI_INCOMPLETE;
					break;			
				case MIDI_AFTERTOUCH :	// MIDI aftertouch
				case MIDI_CTRL_MSG :	// MIDI controller
					midi_main_state = MIDI_STATUS_CTRL;
					midi_sub_state = MIDI_WAIT_FOR_BYTE1;
					key_state = MIDI_INCOMPLETE;
					break;
				case MIDI_PRG_CHANGE :	// MIDI program change
				case MIDI_CHAN_PRESSURE :	// MIDI channel pressure
					midi_main_state = MIDI_STATUS_UNIMPLEMENTED ;
					key_state = MIDI_INCOMPLETE ;
					break;
				case 0xf0 :	// catch-all for system messages
					switch ( last_in & 0xf8 ) {
						case 0xf8 :	// 0xf8-0xff  system realtime msg, don't cancel any running status
							key_state = MIDI_INCOMPLETE ;
							break ;
						case 0xf0 :	// 0xf0-0xf7  system common msg, reset running status etc.
							midi_main_state = MIDI_STATUS_UNIMPLEMENTED ;
							key_state = MIDI_INCOMPLETE ;
							break ;
					}
			}
		}
		else	// no, other type of byte
		{
			switch ( midi_sub_state ) {
				case MIDI_WAIT_FOR_BYTE1 :
					byte1 = last_in ;
					midi_sub_state = MIDI_WAIT_FOR_BYTE2 ;
					key_state = MIDI_INCOMPLETE ;
					break;
				case MIDI_WAIT_FOR_BYTE2 :
					byte2 = last_in ;
					midi_sub_state = MIDI_WAIT_FOR_BYTE1 ;
					// OK, here we should have a complete note on/off command
					if ( midi_main_state == MIDI_STATUS_KEYON ) { // key on
						key_state = MIDI_GOT_KEYON ;
					}
					if ( midi_main_state == MIDI_STATUS_KEYOFF ) { // key off
						key_state = MIDI_GOT_KEYOFF ;
					}
					if ( midi_main_state == MIDI_STATUS_KEYON && byte2 == 0 ) {
						// key on with velocity=0 is really a key off
						key_state = MIDI_GOT_KEYOFF ;
					}
					if ( midi_main_state == MIDI_STATUS_CTRL ) { // ctrl msg
						key_state = MIDI_GOT_CTRL ;
					}
					if ( midi_main_state == MIDI_STATUS_PITCH_WHEEL ) { // pitch wheel msg
						key_state = MIDI_GOT_PITCH_WHEEL ;
					}
					break;

			}
		}

		switch ( key_state ) {
			case MIDI_GOT_KEYON :
				
				key_on(byte1, byte2);

				byte1 = 0;
				byte2 = 0;

				break;

			case MIDI_GOT_KEYOFF :

				key_off(byte1, byte2);

				byte1 = 0;
				byte2 = 0;

				break;
			case MIDI_GOT_CTRL :
				// lcd_place_cursor(0,3);

				TIM4->CCR1 = byte2*2 ;		// just testin'
				TIM4->CCR2 = 255-byte2*2 ;
				
				// midi_set_ctrl(byte1,byte2);	// not really byte1 etc, but you get the idea... :)
				break;
			case MIDI_GOT_PITCH_WHEEL :
				byte1 = 0;
				byte2 = 0;
				break;
		}
		key_state = MIDI_GOT_OTHER;
		if ( maxframes == 0 ) {
			break;
		}
	}
	cpu_load_led_off();
}


