//-----------------------------------------------------------------------------
extern void PrintLarge( uint8_t left, uint8_t top, const char * );
extern void PrintSmall( uint8_t left, uint8_t top, const char * );
extern uint8_t framebuffer[32][18];
extern void write_command( uint8_t command );
//extern void write_data( uint8_t data );
extern void Init_LCD();
extern void Fill_FrameBuffer( uint8_t fill_data );
extern void Send_FrameBuffer_To_LCD( void );
//-----------------------------------------------------------------------------
#define FAST_SPI 1
//
//FAST_SPI = 0: The SS pin is high for ~429uS (24-bits)
//              The clock frequency is ~57KHz
//              Full graphic update takes ~285mS
//FAST_SPI = 1: With no delays, the SS pin is high for ~12uS (24-bits)
//              The clock frequency is ~2MHz
//              But that is too fast for the ST7920, so slow it down
//              with the additional _delay_us(1) calls, which makes:
//              The SS pin is high for ~39uS (24-bits)
//              The clock frequency is ~615KHz
//              Full graphic update takes ~22.5mS
#if FAST_SPI
#define SET_SS  (PORTB |=  (SS_MASK))
#define CLR_SS  (PORTB &= ~(SS_MASK))
#else
#define SET_SS  digitalWrite( SPI_SS_PIN, HIGH ); // Select the chip, starting a 24-bit SPI transfer
#define CLR_SS  digitalWrite( SPI_SS_PIN, LOW ); // Release the chip, ending the SPI transfer
#endif
//-----------------------------------------------------------------------------
//Software SPI (10-bit transfers, difficult to do using the hardware SPI)
#define SPIPORT (PORTB)
#define SPITOGGLE (PINB)
// PB5 (0x20) is SCK  (output) green  OLED pin 12
#define SPI_SCK_PIN (13)
#define SCK_MASK (0x20)
#define CLR_SCK (PORTB &= ~(SCK_MASK))
#define SET_SCK (PORTB |=  (SCK_MASK))
// PB4 (0x10) is MISO (input)  blue   OLED pin 13
//(reference only, it is an input)
#define SPI_MISO_PIN (12)
#define MISO_MASK (0x10)
#define CLR_MISO (PORTB &= ~(MISO_MASK))
#define SET_MISO (PORTB |=  (MISO_MASK))
// PB3 (0x08) is MOSI (output) violet OLED pin 14
#define SPI_MOSI_PIN (11)
#define MOSI_MASK (0x08)
#define CLR_MOSI (PORTB &= ~(MOSI_MASK))
#define SET_MOSI (PORTB |=  (MOSI_MASK))
// DB2 (0x04) is SS   (output) gray   OLED pin 15
#define SPI_SS_PIN (10)
#define SS_MASK (0x04)
//######################################################################################
// Instruction Set 1: (RE=0: Basic Instruction)
#define DISPLAY_CLEAR           0x01 // Fill DDRAM with "20H" and set DDRAM address counter (AC) to "00H"
#define RETURN_HOME             0x02 // Set DDRAM address counter (AC) to "00H", and put cursor
// to origin &#65533;Gthe content of DDRAM are not changed
#define ENTRY_MODE_SET          0x04 // Set cursor position and display shift when doing write or read
// operation
#define DISPLAY_CONTROL         0x08 // D=1: Display ON, C=1: Cursor ON, B=1: Character Blink ON
#define CURSOR_DISPLAY_CONTROL  0x10 // Cursor position and display shift control; the content of
// DDRAM are not changed
#define FUNCTION_SET            0x20 // DL=1 8-bit interface, DL=0 4-bit interface
// RE=1: extended instruction, RE=0: basic instruction
#define SET_CGRAM_ADDRESS       0x40 // Set CGRAM address to address counter (AC)
// Make sure that in extended instruction SR=0
#define SET_DDRAM_ADDRESS       0x80 // Set DDRAM address to address counter (AC)
// AC6 is fixed to 0
//######################################################################################
// Instruction set 2: (RE=1: extended instruction)
#define STANDBY                 0x01 // Enter standby mode, any other instruction can terminate.
// COM1&#65533;c32 are halted
#define SCROLL_OR_RAM_ADDR_SEL  0x02 // SR=1: enable vertical scroll position
// SR=0: enable CGRAM address (basic instruction)
#define REVERSE_BY_LINE         0x04 // Select 1 out of 4 line (in DDRAM) and decide whether to
// reverse the display by toggling this instruction
// R1,R0 initial value is 0,0
#define EXTENDED_FUNCTION_SET   0x20 // DL=1 :8-bit interface, DL=0 :4-bit interface
// RE=1: extended instruction, RE=0: basic instruction
#define SET_SCROLL_ADDRESS      0x40 // G=1 :graphic display ON, G=0 :graphic display OFF
#define SET_GRAPHIC_RAM_ADDRESS 0x80 // Set GDRAM address to address counter (AC)
// Set the vertical address first and followed the horizontal
// address by consecutive writings
// Vertical address range: AC5&#65533;cAC0, Horizontal address range: AC3&#65533;cAC0
//######################################################################################
// Parameters regarding Instruction Sets 1 & 2
#define DISPLAY_SHIFT_S         0x01 // Set 1, ENTRY_MODE_SET
#define INCREASE_DECREASE_ID    0x02 // Set 1, ENTRY_MODE_SET
#define CURSOR_BLINK_ON_B       0x01 // Set 1, DISPLAY_CONTROL
#define CURSOR_ON_C             0x02 // Set 1, DISPLAY_CONTROL
#define DISPLAY_ON_D            0x04 // Set 1, DISPLAY_CONTROL
#define SHIFT_RL                0x04 // Set 1, CURSOR_DISPLAY_CONTROL
#define CURSOR_SC               0x08 // Set 1, CURSOR_DISPLAY_CONTROL
#define EXTENDED_INSTRUCTION_RE 0x04 // Set 1, FUNCTION_SET; Set 2, EXTENDED_FUNTION_SET
#define DATA_LENGTH_DL          0x10 // Set 1, FUNCTION_SET; Set 2, EXTENDED_FUNTION_SET
#define REVERSE_BY_LINE_R0      0x01 // Set 2, REVERSE_BY_LINE
#define REVERSE_BY_LINE_R1      0x02 // Set 2, REVERSE_BY_LINE
#define EN_VERTICAL_SCROLL_SR   0x01 // Set 2, SCROLL_OR_RAM_ADDR_SEL
#define GRAPHIC_ON_G            0x02 // Set 2, EXTENDED_FUNTION_SET

#define BUSY_FLAG_BF            0x80
//######################################################################################
const uint8_t LargeSpace[24] PROGMEM = { // ASCII dec:32 hex:20
    1, 22, // width in bytes, height in pixels

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
};
const uint8_t LargeDot[24] PROGMEM = { // ASCII dec:46 hex:2E
    1, 22, // width in bytes, height in pixels

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b01111000,
    0b01111000,
    0b01111000,
    0b01111000
};
const uint8_t Large_0[46] PROGMEM = { // ASCII dec:48 hex:30
    2, 22, // width in bytes, height in pixels

    0b00000111, 0b11000000,
    0b00001111, 0b11100000,
    0b00011111, 0b11110000,
    0b00111111, 0b11111000,
    0b00111100, 0b01111000,
    0b00111100, 0b01111000,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
                  
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
                  
    0b00111100, 0b01111000,
    0b00111100, 0b01111000,
    0b00111111, 0b11111000,
    0b00011111, 0b11110000,
    0b00001111, 0b11100000,
    0b00000111, 0b11000000
};
const uint8_t Large_1[46] PROGMEM = { // ASCII dec:48 hex:30
    2, 22, // width in bytes, height in pixels

    0b00000011, 0b11000000,
    0b00111111, 0b11100000,
    0b00111111, 0b11100000,
    0b00111111, 0b11100000,
    0b00111111, 0b11100000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
                  
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
                  
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b01111111, 0b11111100,
    0b01111111, 0b11111100,
    0b01111111, 0b11111100,
    0b01111111, 0b11111100
};
const uint8_t Large_2[46] PROGMEM = { // ASCII dec:49 hex:31
    2, 22, // width in bytes, height in pixels

    0b00011111, 0b11000000, 
    0b01111111, 0b11110000, 
    0b01111111, 0b11111000, 
    0b01111111, 0b11111000, 
    0b01110000, 0b11111000, 
    0b01100000, 0b01111100, 
    0b00000000, 0b01111100, 
    0b00000000, 0b01111100, 
    
    0b00000000, 0b01111000, 
    0b00000000, 0b11111000, 
    0b00000000, 0b11111000, 
    0b00000001, 0b11110000, 
    0b00000001, 0b11110000, 
    0b00000011, 0b11100000, 
    0b00000111, 0b11000000, 
    0b00001111, 0b10000000, 
    
    0b00011111, 0b00000000, 
    0b00111110, 0b00000000, 
    0b01111111, 0b11111100, 
    0b01111111, 0b11111100, 
    0b01111111, 0b11111100, 
    0b01111111, 0b11111100

};
const uint8_t Large_3[46] PROGMEM = { // ASCII dec:51 hex:33
    2, 22, // width in bytes, height in pixels

    0b00011111, 0b11000000,
    0b01111111, 0b11110000,
    0b01111111, 0b11111000,
    0b01111111, 0b11111000,
    0b00000000, 0b11111100,
    0b00000000, 0b01111100,
    0b00000000, 0b01111100,
    0b00000000, 0b01111000,
                  
    0b00000000, 0b11111000,
    0b00001111, 0b11110000,
    0b00001111, 0b11100000,
    0b00001111, 0b11111000,
    0b00001111, 0b11111000,
    0b00000000, 0b11111100,
    0b00000000, 0b01111100,
    0b00000000, 0b01111100,
                  
    0b00000000, 0b01111100,
    0b01000000, 0b11111100,
    0b01111111, 0b11111100,
    0b01111111, 0b11111000,
    0b01111111, 0b11110000,
    0b00111111, 0b11000000
};
const uint8_t Large_4[46] PROGMEM = { // ASCII dec:52 hex:34
    2, 22, // width in bytes, height in pixels

    0b00000000, 0b11111000,
    0b00000001, 0b11111000,
    0b00000001, 0b11111000,
    0b00000011, 0b11111000,
    0b00000011, 0b11111000,
    0b00000111, 0b11111000,
    0b00000111, 0b01111000,
    0b00001110, 0b01111000,
                  
    0b00001110, 0b01111000,
    0b00011100, 0b01111000,
    0b00111100, 0b01111000,
    0b00111000, 0b01111000,
    0b01111000, 0b01111000,
    0b01110000, 0b01111000,
    0b01111111, 0b11111110,
    0b01111111, 0b11111110,
                  
    0b01111111, 0b11111110,
    0b01111111, 0b11111110,
    0b00000000, 0b01111000,
    0b00000000, 0b01111000,
    0b00000000, 0b01111000,
    0b00000000, 0b01111000
};
const uint8_t Large_5[46] PROGMEM = { // ASCII dec:53 hex:35
    2, 22, // width in bytes, height in pixels

    0b00111111, 0b11111100, 
    0b00111111, 0b11111100, 
    0b00111111, 0b11111100, 
    0b00111111, 0b11111100, 
    0b00111100, 0b00000000, 
    0b00111100, 0b00000000, 
    0b00111100, 0b00000000, 
    0b00111100, 0b00000000, 
    
    0b00111111, 0b11100000, 
    0b00111111, 0b11110000, 
    0b00111111, 0b11111000, 
    0b00111111, 0b11111100, 
    0b00000000, 0b01111100, 
    0b00000000, 0b01111100, 
    0b00000000, 0b00111100, 
    0b00000000, 0b00111100, 
    
    0b01000000, 0b01111100, 
    0b01110000, 0b11111000, 
    0b01111111, 0b11111000, 
    0b01111111, 0b11110000, 
    0b01111111, 0b11100000, 
    0b00011111, 0b11000000
};
const uint8_t Large_6[46] PROGMEM = { // ASCII dec:54 hex:36
    2, 22, // width in bytes, height in pixels

    0b00000011, 0b11111000,
    0b00000111, 0b11111000,
    0b00001111, 0b11111000,
    0b00011111, 0b11111000,
    0b00011110, 0b00000000,
    0b00111100, 0b00000000,
    0b00111100, 0b00000000,
    0b00111001, 0b11000000,
                  
    0b00111111, 0b11110000,
    0b01111111, 0b11111000,
    0b01111111, 0b11111000,
    0b01111110, 0b01111100,
    0b01111100, 0b00111100,
    0b01111100, 0b00111100,
    0b00111100, 0b00111100,
    0b00111100, 0b00111100,
                  
    0b00111100, 0b00111100,
    0b00111110, 0b01111100,
    0b00011111, 0b11111000,
    0b00011111, 0b11111000,
    0b00001111, 0b11110000,
    0b00000011, 0b11000000
};
const uint8_t Large_7[46] PROGMEM = { // ASCII dec:55 hex:37
    2, 22, // width in bytes, height in pixels

    0b01111111, 0b11111100,
    0b01111111, 0b11111100,
    0b01111111, 0b11111100,
    0b01111111, 0b11111100,
    0b00000000, 0b01111100,
    0b00000000, 0b01111100,
    0b00000000, 0b11111000,
    0b00000000, 0b11111000,
    
    0b00000000, 0b11110000,
    0b00000001, 0b11110000,
    0b00000001, 0b11110000,
    0b00000011, 0b11100000,
    0b00000011, 0b11100000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b10000000,
    
    0b00001111, 0b10000000,
    0b00001111, 0b00000000,
    0b00011111, 0b00000000,
    0b00011111, 0b00000000,
    0b00111110, 0b00000000,
    0b00111110, 0b00000000
};
const uint8_t Large_8[46] PROGMEM = { // ASCII dec:56 hex:38
    2, 22, // width in bytes, height in pixels

    0b00000111, 0b11100000,
    0b00011111, 0b11110000,
    0b00111111, 0b11111000,
    0b00111111, 0b11111000,
    0b00111100, 0b01111100,
    0b01111000, 0b00111100,
    0b01111100, 0b00111000,
    0b00111110, 0b01111000,
    
    0b00111111, 0b01111000,
    0b00011111, 0b11110000,
    0b00011111, 0b11110000,
    0b00011111, 0b11111000,
    0b00111101, 0b11111000,
    0b00111000, 0b11111100,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
    
    0b01111000, 0b00111100,
    0b01111100, 0b01111100,
    0b00111111, 0b11111000,
    0b00111111, 0b11111000,
    0b00011111, 0b11110000,
    0b00001111, 0b11100000
};
const uint8_t Large_9[46] PROGMEM = { // ASCII dec:57 hex:39
    2, 22, // width in bytes, height in pixels

    0b00000111, 0b11000000,
    0b00011111, 0b11100000,
    0b00011111, 0b11110000,
    0b00111111, 0b11111000,
    0b00111100, 0b01111000,
    0b01111000, 0b00111000,
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
                  
    0b01111000, 0b00111100,
    0b01111000, 0b00111100,
    0b00111100, 0b01111100,
    0b00111111, 0b11111100,
    0b00011111, 0b11111100,
    0b00001111, 0b11111100,
    0b00000011, 0b10111100,
    0b00000000, 0b00111100,
                  
    0b00000000, 0b00111000,
    0b00100000, 0b01111000,
    0b00111111, 0b11110000,
    0b00111111, 0b11110000,
    0b00111111, 0b11100000,
    0b00011111, 0b10000000
};
//#############################################################
const uint8_t SmallSpace[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
};
const uint8_t Small_W[23] PROGMEM = { // ASCII dec:32 hex:20
    2, 10, // width in bytes, height in pixels

    0b00000000, 0b00000000,
    0b11000110, 0b00110000, 
    0b11000110, 0b00110000, 
    0b11000110, 0b00110000, 
    0b01100110, 0b01100000, 
    0b01101111, 0b01100000, 
    0b01101001, 0b01100000, 
    0b00111001, 0b11000000, 
    0b00110000, 0b11000000, 
    0b00110000, 0b11000000
};
const uint8_t Small_0[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b01111100,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b01111100
};
const uint8_t Small_1[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b00011000,
    0b01111000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b01111110
};
const uint8_t Small_2[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b01111100,
    0b11000110,
    0b11000110,
    0b00000110,
    0b00001100,
    0b00011000,
    0b00110000,
    0b01100000,
    0b11111110
};
const uint8_t Small_3[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b01111100,
    0b11000110,
    0b11000110,
    0b00000110,
    0b00011100,
    0b00000110,
    0b11000110,
    0b11000110,
    0b01111100
};
const uint8_t Small_4[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b00001100,
    0b00011100,
    0b00101100,
    0b01001100,
    0b10001100,
    0b11111110,
    0b00001100,
    0b00001100,
    0b00001100
};
const uint8_t Small_5[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b01111110,
    0b01100000,
    0b01100000,
    0b01111100,
    0b00000110,
    0b00000110,
    0b11000110,
    0b11000110,
    0b01111100
};
const uint8_t Small_6[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b00111100,
    0b01100000,
    0b11000000,
    0b11111100,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b01111100
};
const uint8_t Small_7[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b11111110,
    0b00000110,
    0b00000110,
    0b00001100,
    0b00001100,
    0b00011000,
    0b00011000,
    0b00110000,
    0b00110000 
};
const uint8_t Small_8[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b01111100,
    0b11000110,
    0b11000110,
    0b11000110,
    0b01111100,
    0b11000110,
    0b11000110,
    0b11000110,
    0b01111100 
};
const uint8_t Small_9[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b01111100,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b01111110,
    0b00000110,
    0b00001100,
    0b01111000
};
const uint8_t SmallDot[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels
    
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000
};
const uint8_t Caption_V[33] PROGMEM = { // ASCII dec:32 hex:20
    3, 10, // width in bytes, height in pixels
    
    0b00000000, 0b00000000, 0b00000000,
    0b01100001, 0b10000000, 0b00000000,
    0b01100001, 0b10000000, 0b00000000,
    0b01100001, 0b10000000, 0b00000000,
    0b00110011, 0b00000000, 0b00000000,
    0b00110011, 0b00000000, 0b00000000,
    0b00011110, 0b00000000, 0b00000000,
    0b00011110, 0b00000000, 0b00000000,
    0b00001100, 0b00000000, 0b00000000,
    0b00001100, 0b00000000, 0b00000000
};
const uint8_t Caption_A[33] PROGMEM = { // ASCII dec:32 hex:20
    3, 10, // width in bytes, height in pixels
    
    0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b01100000, 
    0b00000000, 0b00000000, 0b01100000, 
    0b00000000, 0b00000000, 0b11110000, 
    0b00000000, 0b00000000, 0b11110000, 
    0b00000000, 0b00000001, 0b10011000, 
    0b00000000, 0b00000001, 0b10011000, 
    0b00000000, 0b00000011, 0b11111100, 
    0b00000000, 0b00000011, 0b00001100, 
    0b00000000, 0b00000011, 0b00001100
};
const uint8_t Caption_mV[33] PROGMEM = { // ASCII dec:32 hex:20
    3, 10, // width in bytes, height in pixels
    
    0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000001, 0b10000110,
    0b00000000, 0b00000001, 0b10000110,
    0b01101110, 0b01110001, 0b10000110,
    0b01110011, 0b10011000, 0b11001100,
    0b01100011, 0b00011000, 0b11001100,
    0b01100011, 0b00011000, 0b01111000,
    0b01100011, 0b00011000, 0b01111000,
    0b01100011, 0b00011000, 0b00110000,
    0b01100011, 0b00011000, 0b00110000
};
const uint8_t Caption_mA[33] PROGMEM = { // ASCII dec:32 hex:20
    3, 10, // width in bytes, height in pixels
    
    0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b01100000,
    0b00000000, 0b00000000, 0b01100000,
    0b11011100, 0b11100000, 0b11110000,
    0b11100111, 0b00110000, 0b11110000,
    0b11000110, 0b00110001, 0b10011000,
    0b11000110, 0b00110001, 0b10011000,
    0b11000110, 0b00110011, 0b11111100,
    0b11000110, 0b00110011, 0b00001100,
    0b11000110, 0b00110011, 0b00001100
};
const uint8_t Caption_mW[43] PROGMEM = { // ASCII dec:32 hex:20
    4, 10, // width in bytes, height in pixels
    
    0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000011, 0b00011000, 0b11000000,
    0b00000000, 0b00000011, 0b00011000, 0b11000000,
    0b11011100, 0b11100011, 0b00011000, 0b11000000,
    0b11100111, 0b00110001, 0b10011001, 0b10000000,
    0b11000110, 0b00110001, 0b10111101, 0b10000000,
    0b11000110, 0b00110001, 0b10100101, 0b10000000,
    0b11000110, 0b00110000, 0b11100111, 0b00000000,
    0b11000110, 0b00110000, 0b11000011, 0b00000000,
    0b11000110, 0b00110000, 0b11000011, 0b00000000
};
//-----------------------------------------------------------------------------
