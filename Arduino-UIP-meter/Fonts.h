#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
//-----------------------------------------------------------------------------
extern void PrintLarge( uint8_t left, uint8_t top, const char * );
extern void PrintSmall( uint8_t left, uint8_t top, const char * );
extern uint8_t framebuffer[32][18];
extern void write_command( uint8_t command );
extern void Init_LCD();
extern void Fill_FrameBuffer( uint8_t fill_data );
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
#define SS_MASK (0x04)
#define CLR_SS  (PORTB &= ~(SS_MASK))
#define SET_SS  (PORTB |=  (SS_MASK))
#define SPI_SS_PIN (10)

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
    0b11110000,
    0b11110000,
    0b11110000,
    0b11110000
};
const uint8_t Large_0[46] PROGMEM = { // ASCII dec:48 hex:30
    2, 22, // width in bytes, height in pixels

    0b00001111, 0b10000000,
    0b00011111, 0b11000000,
    0b00111111, 0b11100000,
    0b01111111, 0b11110000,
    0b01111000, 0b11110000,
    0b01111000, 0b11110000,
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    
    0b01111000, 0b11110000,
    0b01111000, 0b11110000,
    0b01111111, 0b11110000,
    0b00111111, 0b11100000,
    0b00011111, 0b11000000,
    0b00001111, 0b10000000
};
const uint8_t Large_1[46] PROGMEM = { // ASCII dec:48 hex:30
    2, 22, // width in bytes, height in pixels

    0b00000111, 0b10000000,
    0b01111111, 0b11000000,
    0b01111111, 0b11000000,
    0b01111111, 0b11000000,
    0b01111111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,

    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b11000000,

    0b00000111, 0b11000000,
    0b00000111, 0b11000000,
    0b11111111, 0b11111100,
    0b11111111, 0b11111100,
    0b11111111, 0b11111100,
    0b11111111, 0b11111100
};
const uint8_t Large_2[46] PROGMEM = { // ASCII dec:49 hex:31
    2, 22, // width in bytes, height in pixels

    0b00111111, 0b10000000,
    0b11111111, 0b11100000,
    0b11111111, 0b11100000,
    0b11111111, 0b11110000,
    0b11000001, 0b11110000,
    0b00000000, 0b11110000,
    0b00000000, 0b11110000,
    0b00000000, 0b11110000,

    0b00000000, 0b11110000,
    0b00000000, 0b11110000,
    0b00000001, 0b11110000,
    0b00000011, 0b11100000,
    0b00000111, 0b11000000,
    0b00001111, 0b10000000,
    0b00011111, 0b00000000,
    0b00111110, 0b00000000,

    0b01111100, 0b00000000,
    0b11111100, 0b00000000,
    0b11111111, 0b11110000,
    0b11111111, 0b11110000,
    0b11111111, 0b11110000,
    0b11111111, 0b11110000

};
const uint8_t Large_3[46] PROGMEM = { // ASCII dec:51 hex:33
    2, 22, // width in bytes, height in pixels

    0b00111111, 0b10000000,
    0b11111111, 0b11100000,
    0b11111111, 0b11110000,
    0b11111111, 0b11110000,
    0b00000001, 0b11111000,
    0b00000000, 0b11111000,
    0b00000000, 0b11111000,
    0b00000000, 0b11110000,

    0b00000001, 0b11110000,
    0b00011111, 0b11100000,
    0b00011111, 0b11000000,
    0b00011111, 0b11110000,
    0b00011111, 0b11110000,
    0b00000001, 0b11111000,
    0b00000000, 0b11111000,
    0b00000000, 0b11111000,

    0b00000000, 0b11111000,
    0b10000001, 0b11111000,
    0b11111111, 0b11111000,
    0b11111111, 0b11110000,
    0b11111111, 0b11100000,
    0b01111111, 0b10000000
};
const uint8_t Large_4[46] PROGMEM = { // ASCII dec:52 hex:34
    2, 22, // width in bytes, height in pixels

    0b00000001, 0b11110000,
    0b00000011, 0b11110000,
    0b00000011, 0b11110000,
    0b00000111, 0b11110000,
    0b00000111, 0b11110000,
    0b00001111, 0b11110000,
    0b00001110, 0b11110000,
    0b00011100, 0b11110000,

    0b00011100, 0b11110000,
    0b00111000, 0b11110000,
    0b01111000, 0b11110000,
    0b01110000, 0b11110000,
    0b11110000, 0b11110000,
    0b11100000, 0b11110000,
    0b11111111, 0b11111100,
    0b11111111, 0b11111100,

    0b11111111, 0b11111100,
    0b11111111, 0b11111100,
    0b00000000, 0b11110000,
    0b00000000, 0b11110000,
    0b00000000, 0b11110000,
    0b00000000, 0b11110000
};
const uint8_t Large_5[46] PROGMEM = { // ASCII dec:53 hex:35
    2, 22, // width in bytes, height in pixels

    0b01111111, 0b11100000,
    0b01111111, 0b11100000,
    0b01111111, 0b11100000,
    0b01111111, 0b11100000,
    0b01110000, 0b00000000,
    0b01110000, 0b00000000,
    0b01110000, 0b00000000,
    0b01110110, 0b00000000,

    0b01111111, 0b10000000,
    0b01111111, 0b11000000,
    0b01111111, 0b11100000,
    0b01000011, 0b11110000,
    0b00000001, 0b11110000,
    0b00000000, 0b11110000,
    0b00000000, 0b11110000,
    0b00000000, 0b11110000,

    0b00000000, 0b11110000,
    0b11000001, 0b11110000,
    0b11111111, 0b11100000,
    0b11111111, 0b11100000,
    0b11111111, 0b11000000,
    0b00111111, 0b00000000
};
const uint8_t Large_6[46] PROGMEM = { // ASCII dec:54 hex:36
    2, 22, // width in bytes, height in pixels

    0b00000111, 0b11110000,
    0b00001111, 0b11110000,
    0b00011111, 0b11110000,
    0b00111111, 0b11110000,
    0b00111100, 0b00000000,
    0b01111000, 0b00000000,
    0b01111000, 0b00000000,
    0b01110011, 0b10000000,

    0b01111111, 0b11100000,
    0b11111111, 0b11110000,
    0b11111111, 0b11110000,
    0b11111100, 0b11111000,
    0b11111000, 0b01111000,
    0b11111000, 0b01111000,
    0b01111000, 0b01111000,
    0b01111000, 0b01111000,

    0b01111000, 0b01111000,
    0b01111100, 0b11111000,
    0b00111111, 0b11110000,
    0b00111111, 0b11110000,
    0b00011111, 0b11100000,
    0b00000111, 0b10000000
};
const uint8_t Large_7[46] PROGMEM = { // ASCII dec:55 hex:37
    2, 22, // width in bytes, height in pixels

    0b11111111, 0b11110000,
    0b11111111, 0b11110000,
    0b11111111, 0b11110000,
    0b11111111, 0b11110000,
    0b00000001, 0b11100000,
    0b00000001, 0b11100000,
    0b00000001, 0b11100000,
    0b00000011, 0b11100000,

    0b00000011, 0b11000000,
    0b00000011, 0b11000000,
    0b00000111, 0b11000000,
    0b00000111, 0b10000000,
    0b00000111, 0b10000000,
    0b00001111, 0b10000000,
    0b00001111, 0b00000000,
    0b00001111, 0b00000000,

    0b00011111, 0b00000000,
    0b00011110, 0b00000000,
    0b00011110, 0b00000000,
    0b00111100, 0b00000000,
    0b00111100, 0b00000000,
    0b00111000, 0b00000000
};
const uint8_t Large_8[46] PROGMEM = { // ASCII dec:56 hex:38
    2, 22, // width in bytes, height in pixels

    0b00011111, 0b10000000,
    0b01111111, 0b11100000,
    0b01111111, 0b11100000,
    0b11111111, 0b11110000,
    0b11111001, 0b11110000,
    0b11110000, 0b11110000,
    0b11110000, 0b11110000,
    0b11110000, 0b11110000,

    0b01111001, 0b11100000,
    0b00111111, 0b11000000,
    0b00111111, 0b11000000,
    0b01111111, 0b11100000,
    0b11111001, 0b11110000,
    0b11110000, 0b11110000,
    0b11110000, 0b11110000,
    0b11110000, 0b11110000,

    0b11110000, 0b11110000,
    0b11111001, 0b11110000,
    0b11111111, 0b11110000,
    0b01111111, 0b11100000,
    0b01111111, 0b11100000,
    0b00011111, 0b10000000
};
const uint8_t Large_9[46] PROGMEM = { // ASCII dec:57 hex:39
    2, 22, // width in bytes, height in pixels

    0b00001111, 0b10000000,
    0b00111111, 0b11000000,
    0b00111111, 0b11100000,
    0b01111111, 0b11110000,
    0b01111000, 0b11110000,
    0b11110000, 0b01110000,
    0b11110000, 0b01111000,
    0b11110000, 0b01111000,

    0b11110000, 0b01111000,
    0b11110000, 0b01111000,
    0b01111000, 0b11111000,
    0b01111111, 0b11111000,
    0b00111111, 0b11111000,
    0b00011111, 0b11111000,
    0b00000111, 0b01111000,
    0b00000000, 0b01111000,

    0b00000000, 0b01110000,
    0b01000000, 0b11110000,
    0b01111111, 0b11100000,
    0b01111111, 0b11100000,
    0b01111111, 0b11000000,
    0b00111111, 0b00000000
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
const uint8_t Small_A[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels

    0b00000000,
    0b00011000,
    0b00011000,
    0b00111100,
    0b00111100,
    0b01100110,
    0b01100110,
    0b11111111,
    0b11000011,
    0b11000011
};
const uint8_t Small_m[23] PROGMEM = { // ASCII dec:32 hex:20
    2, 10, // width in bytes, height in pixels

    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00011011, 0b10011100,
    0b00011100, 0b11100110,
    0b00011000, 0b11000110,
    0b00011000, 0b11000110,
    0b00011000, 0b11000110,
    0b00011000, 0b11000110,
    0b00011000, 0b11000110
};
const uint8_t Small_V[13] PROGMEM = { // ASCII dec:32 hex:20
    1, 10, // width in bytes, height in pixels

    0b00000000,
    0b11000011,
    0b11000011,
    0b11000011,
    0b01100110,
    0b01100110,
    0b00111100,
    0b00111100,
    0b00011000,
    0b00011000
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
    0b11000011, 0b00000000, 0b00000000,
    0b11000011, 0b00000000, 0b00000000,
    0b11000011, 0b00000000, 0b00000000,
    0b01100110, 0b00000000, 0b00000000,
    0b01100110, 0b00000000, 0b00000000,
    0b00111100, 0b00000000, 0b00000000,
    0b00111100, 0b00000000, 0b00000000,
    0b00011000, 0b00000000, 0b00000000,
    0b00011000, 0b00000000, 0b00000000
};
const uint8_t Caption_A[33] PROGMEM = { // ASCII dec:32 hex:20
    3, 10, // width in bytes, height in pixels
    
    0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b11000000, 
    0b00000000, 0b00000000, 0b11000000, 
    0b00000000, 0b00000001, 0b11100000, 
    0b00000000, 0b00000001, 0b11100000, 
    0b00000000, 0b00000011, 0b00110000, 
    0b00000000, 0b00000011, 0b00110000, 
    0b00000000, 0b00000111, 0b11111000, 
    0b00000000, 0b00000110, 0b00011000, 
    0b00000000, 0b00000110, 0b00011000
};
const uint8_t Caption_mV[33] PROGMEM = { // ASCII dec:32 hex:20
    3, 10, // width in bytes, height in pixels
    
    0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000011, 0b00001100,
    0b00000000, 0b00000011, 0b00001100,
    0b11011100, 0b11100011, 0b00001100,
    0b11100111, 0b00110001, 0b10011000,
    0b11000110, 0b00110001, 0b10011000,
    0b11000110, 0b00110000, 0b11110000,
    0b11000110, 0b00110000, 0b11110000,
    0b11000110, 0b00110000, 0b01100000,
    0b11000110, 0b00110000, 0b01100000
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
#ifdef __cplusplus
}
#endif // __cplusplus
