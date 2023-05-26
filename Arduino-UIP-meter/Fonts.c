#include <util/delay.h>
#include "Arduino.h"
#include "Fonts.h"
//-----------------------------------------------------------------------------
#define FRAME_WIDTH_BYTES (18)
#define FRAME_WIDTH_PIXELS (FRAME_WIDTH_BYTES*8)
#define FRAME_HEIGHT_PIXELS (32)
//-----------------------------------------------------------------------------
uint8_t framebuffer[FRAME_HEIGHT_PIXELS][FRAME_WIDTH_BYTES];
//-----------------------------------------------------------------------------
void SPI_WriteByte( uint8_t data ) {
#if FAST_SPI
    //Pretty fast software SPI 8-bit transfer code
    //Several ideas taken from the fastest non-assembly implementation at:
    //http://nerdralph.blogspot.com/2015/03/fastest-avr-software-spi-in-west.html
    //
    //Pre-calculate the value that will drive clk and data low in one cycle.
    //(Note that this is not interrupt safe if an ISR is writing to the same port)
    register uint8_t
    force_clk_and_data_low;

    //Pre-calculate the value that will drive clk and data low in one
    //operation.
    force_clk_and_data_low = ( SPIPORT & ~( MOSI_MASK | SCK_MASK ) );

    //Now clock the 8 bits of data out. It needs the delays.
    SPIPORT = force_clk_and_data_low;
    if( data & 0x80 )
        SPITOGGLE = MOSI_MASK;
    _delay_us( 1 );
    SPITOGGLE = SCK_MASK;
    _delay_us( 2 );
    SPIPORT = force_clk_and_data_low;
    if( data & 0x40 )
        SPITOGGLE = MOSI_MASK;
    _delay_us( 1 );
    SPITOGGLE = SCK_MASK;
    _delay_us( 2 );
    SPIPORT = force_clk_and_data_low;
    if( data & 0x20 )
        SPITOGGLE = MOSI_MASK;
    _delay_us( 1 );
    SPITOGGLE = SCK_MASK;
    _delay_us( 2 );
    SPIPORT = force_clk_and_data_low;
    if( data & 0x10 )
        SPITOGGLE = MOSI_MASK;
    _delay_us( 1 );
    SPITOGGLE = SCK_MASK;
    _delay_us( 2 );
    SPIPORT = force_clk_and_data_low;
    if( data & 0x08 )
        SPITOGGLE = MOSI_MASK;
    _delay_us( 1 );
    SPITOGGLE = SCK_MASK;
    _delay_us( 2 );
    SPIPORT = force_clk_and_data_low;
    if( data & 0x04 )
        SPITOGGLE = MOSI_MASK;
    _delay_us( 1 );
    SPITOGGLE = SCK_MASK;
    _delay_us( 2 );
    SPIPORT = force_clk_and_data_low;
    if( data & 0x02 )
        SPITOGGLE = MOSI_MASK;
    _delay_us( 1 );
    SPITOGGLE = SCK_MASK;
    _delay_us( 2 );
    SPIPORT = force_clk_and_data_low;
    if( data & 0x01 )
        SPITOGGLE = MOSI_MASK;
    _delay_us( 1 );
    SPITOGGLE = SCK_MASK;
    _delay_us( 1 );

#else
    //Straight-forward software SPI 8-bit transfer code, perhaps
    //easier to understand, possibly more portable. Certainly slower.

    //(bits 0-7) Push each of the 8 data bits out.
    for( uint8_t mask = 0x80; mask; mask >>= 1 ) {
        //Set the MOSI pin high or low depending on if our mask
        //corresponds to a 1 or 0 in the data.
        if( mask & data ) {
            digitalWrite( SPI_MOSI_PIN, HIGH );
        } else {
            digitalWrite( SPI_MOSI_PIN, LOW );
        }
        //Clock it in.
        digitalWrite( SPI_SCK_PIN, HIGH );
        digitalWrite( SPI_SCK_PIN, LOW );
    }
#endif
}
//-----------------------------------------------------------------------------
void write_data( uint8_t data ) {
    //Select the chip, starting a 24-bit SPI transfer
#if FAST_SPI
    SET_SS;
#else
    digitalWrite( SPI_SS_PIN, HIGH );
#endif
    SPI_WriteByte( 0xFA ); // send data header
    SPI_WriteByte( data & 0xF0 ); // send high nibble
    SPI_WriteByte( ( data << 4 ) & 0xF0 ); // send low nibble
    //Release the chip, ending the SPI transfer
#if FAST_SPI
    CLR_SS;
#else
    digitalWrite( SPI_SS_PIN, LOW );
#endif
}
//-----------------------------------------------------------------------------
//X is only adressible to the word, a grouping of 16 horizontal pixels:
//  0-17 => 18 * 8 = 144
void Set_Graphics_Mode_Address( uint8_t x_word, uint8_t y ) {
    // Access extended functions register, graphics mode
    write_command( 0x34 );
    //Set Y pixel address
    write_command( 0x80 | y );
    //Set X byte address
    write_command( 0x80 | x_word );
    // Exit extended functions register, stay in graphics mode
    write_command( 0x36 );
}
//-----------------------------------------------------------------------------
void Fill_FrameBuffer( uint8_t fill_data ) {
    register uint8_t i;
    register uint8_t j;
    //Set the FrameBuffer to a given data.
    for( j = 0; j < FRAME_HEIGHT_PIXELS; j++ ) {
        for( i = 0; i < FRAME_WIDTH_BYTES; i++ ) {
            framebuffer[j][i] = fill_data;
        }
    }
}
//-----------------------------------------------------------------------------
void Send_FrameBuffer_To_LCD( void ) {
    register uint8_t i;
    register uint8_t j;
    //
    //Setting to graphics mode commented out, display is kept in graphics mode all the time.
    //Commands were moved to the Arduino setup function
    //
    //write_command( 0x06 ); // entry mode -- cursor moves right, address counter increased by 1
    //write_command( 0x34 ); // extended function
    //write_command( 0x36 ); // graphic mode on
    //
    //Send the FrameBuffer RAM data to the LCD.
    for( j = 0; j < FRAME_HEIGHT_PIXELS; j++ ) {
        //Set Y address to this line, reset X address
        Set_Graphics_Mode_Address( 0, j );
        for( i = 0; i < FRAME_WIDTH_BYTES; i++ ) {
            write_data( framebuffer[j][i] );
        }
    }
}
//-----------------------------------------------------------------------------
void Init_LCD() {
    write_command( 0x30 );  // standard function set
    write_command( 0x0C );  // cursor on, not blinking
    write_command( 0x01 );  // clear text screen
    _delay_ms( 2 );
    //clear the framebuffer send to the graphics screen
    Fill_FrameBuffer( 0x00 );
    Send_FrameBuffer_To_LCD();
}
//-----------------------------------------------------------------------------
void write_command( uint8_t command ) {
    //Select the chip, starting a 24-bit SPI transfer
#if FAST_SPI
    SET_SS;
#else
    digitalWrite( SPI_SS_PIN, HIGH );
#endif
    SPI_WriteByte( 0xF8 ); // send command header
    SPI_WriteByte( command & 0xF0 ); // send high nibble
    SPI_WriteByte( ( command << 4 ) & 0xF0 ); // send low nibble
    //Release the chip, ending the SPI transfer
#if FAST_SPI
    CLR_SS;
#else
    digitalWrite( SPI_SS_PIN, LOW );
#endif
}
//-----------------------------------------------------------------------------
void PrintLarge( uint8_t left_byte, uint8_t top_pixel, const char *text ) {
    register uint8_t b, c, i, j, w, h, y = top_pixel, x = left_byte;
    for( register uint8_t p = 0; p < strlen( text ); p++ ) {
        c = text[p];
        switch( c ) { // Read the dimensions of the character
            case '.': w = pgm_read_byte( &LargeDot[   0 ] ); h = pgm_read_byte( &LargeDot[   1 ] ); break;
            case '0': w = pgm_read_byte( &Large_0[    0 ] ); h = pgm_read_byte( &Large_0[    1 ] ); break;
            case '1': w = pgm_read_byte( &Large_1[    0 ] ); h = pgm_read_byte( &Large_1[    1 ] ); break;
            case '2': w = pgm_read_byte( &Large_2[    0 ] ); h = pgm_read_byte( &Large_2[    1 ] ); break;
            case '3': w = pgm_read_byte( &Large_3[    0 ] ); h = pgm_read_byte( &Large_3[    1 ] ); break;
            case '4': w = pgm_read_byte( &Large_4[    0 ] ); h = pgm_read_byte( &Large_4[    1 ] ); break;
            case '5': w = pgm_read_byte( &Large_5[    0 ] ); h = pgm_read_byte( &Large_5[    1 ] ); break;
            case '6': w = pgm_read_byte( &Large_6[    0 ] ); h = pgm_read_byte( &Large_6[    1 ] ); break;
            case '7': w = pgm_read_byte( &Large_7[    0 ] ); h = pgm_read_byte( &Large_7[    1 ] ); break;
            case '8': w = pgm_read_byte( &Large_8[    0 ] ); h = pgm_read_byte( &Large_8[    1 ] ); break;
            case '9': w = pgm_read_byte( &Large_9[    0 ] ); h = pgm_read_byte( &Large_9[    1 ] ); break;
            default:  w = pgm_read_byte( &LargeSpace[ 0 ] ); h = pgm_read_byte( &LargeSpace[ 1 ] );
        }
        for( j = 0; j < h; j++ ) {
            for( i = 0; i < w; i++ ) {
                register uint8_t *LCD_Memory_1 = &framebuffer[j + y][( x * 8 + i * 8 ) >> 3];
                switch( c ) { // Print the character in framebuffer.
                    case '.': b = pgm_read_byte( &LargeDot[   j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '0': b = pgm_read_byte( &Large_0[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '1': b = pgm_read_byte( &Large_1[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '2': b = pgm_read_byte( &Large_2[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '3': b = pgm_read_byte( &Large_3[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '4': b = pgm_read_byte( &Large_4[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '5': b = pgm_read_byte( &Large_5[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '6': b = pgm_read_byte( &Large_6[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '7': b = pgm_read_byte( &Large_7[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '8': b = pgm_read_byte( &Large_8[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '9': b = pgm_read_byte( &Large_9[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    default:  b = pgm_read_byte( &LargeSpace[ j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b;
                }
            }
        }
        x = x + w;
    }
    Send_FrameBuffer_To_LCD();
}
//-----------------------------------------------------------------------------
void PrintSmall( uint8_t left_byte, uint8_t top_pixel, const char *text ) {
    register uint8_t b, c, i, j, w, h, y = top_pixel, x = left_byte;
    for( register uint8_t p = 0; p < strlen( text ); p++ ) {
        c = text[p];
        switch( c ) { // Read the dimensions of the character
            case '.': w = pgm_read_byte( &SmallDot[   0 ] ); h = pgm_read_byte( &SmallDot[   1 ] ); break;
            case '0': w = pgm_read_byte( &Small_0[    0 ] ); h = pgm_read_byte( &Small_0[    1 ] ); break;
            case '1': w = pgm_read_byte( &Small_1[    0 ] ); h = pgm_read_byte( &Small_1[    1 ] ); break;
            case '2': w = pgm_read_byte( &Small_2[    0 ] ); h = pgm_read_byte( &Small_2[    1 ] ); break;
            case '3': w = pgm_read_byte( &Small_3[    0 ] ); h = pgm_read_byte( &Small_3[    1 ] ); break;
            case '4': w = pgm_read_byte( &Small_4[    0 ] ); h = pgm_read_byte( &Small_4[    1 ] ); break;
            case '5': w = pgm_read_byte( &Small_5[    0 ] ); h = pgm_read_byte( &Small_5[    1 ] ); break;
            case '6': w = pgm_read_byte( &Small_6[    0 ] ); h = pgm_read_byte( &Small_6[    1 ] ); break;
            case '7': w = pgm_read_byte( &Small_7[    0 ] ); h = pgm_read_byte( &Small_7[    1 ] ); break;
            case '8': w = pgm_read_byte( &Small_8[    0 ] ); h = pgm_read_byte( &Small_8[    1 ] ); break;
            case '9': w = pgm_read_byte( &Small_9[    0 ] ); h = pgm_read_byte( &Small_9[    1 ] ); break;
            case 'A': w = pgm_read_byte( &Small_A[    0 ] ); h = pgm_read_byte( &Small_A[    1 ] ); break;
            case 'V': w = pgm_read_byte( &Small_V[    0 ] ); h = pgm_read_byte( &Small_V[    1 ] ); break;
            case 'W': w = pgm_read_byte( &Small_W[    0 ] ); h = pgm_read_byte( &Small_W[    1 ] ); break;
            case 'm': w = pgm_read_byte( &Small_m[    0 ] ); h = pgm_read_byte( &Small_m[    1 ] ); break;
            case '{': w = pgm_read_byte( &Caption_V[  0 ] ); h = pgm_read_byte( &Caption_V[  1 ] ); break;
            case '}': w = pgm_read_byte( &Caption_A[  0 ] ); h = pgm_read_byte( &Caption_A[  1 ] ); break;
            case '[': w = pgm_read_byte( &Caption_mA[ 0 ] ); h = pgm_read_byte( &Caption_mA[ 1 ] ); break;
            case ']': w = pgm_read_byte( &Caption_mV[ 0 ] ); h = pgm_read_byte( &Caption_mV[ 1 ] ); break;
            case '#': w = pgm_read_byte( &Caption_mW[ 0 ] ); h = pgm_read_byte( &Caption_mW[ 1 ] ); break;
            default:  w = pgm_read_byte( &SmallSpace[ 0 ] ); h = pgm_read_byte( &SmallSpace[ 1 ] );
        }
        for( j = 0; j < h; j++ ) {
            for( i = 0; i < w; i++ ) {
                register uint8_t *LCD_Memory_1 = &framebuffer[j + y][( x * 8 + i * 8 ) >> 3];
                switch( c ) { // Print the character in framebuffer.
                    case '.': b=pgm_read_byte( &SmallDot[    j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '0': b=pgm_read_byte( &Small_0[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '1': b=pgm_read_byte( &Small_1[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '2': b=pgm_read_byte( &Small_2[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '3': b=pgm_read_byte( &Small_3[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '4': b=pgm_read_byte( &Small_4[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '5': b=pgm_read_byte( &Small_5[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '6': b=pgm_read_byte( &Small_6[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '7': b=pgm_read_byte( &Small_7[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '8': b=pgm_read_byte( &Small_8[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '9': b=pgm_read_byte( &Small_9[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case 'A': b=pgm_read_byte( &Small_A[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case 'V': b=pgm_read_byte( &Small_V[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case 'W': b=pgm_read_byte( &Small_W[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case 'm': b=pgm_read_byte( &Small_m[     j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '{': b=pgm_read_byte( &Caption_V[   j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '}': b=pgm_read_byte( &Caption_A[   j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '[': b=pgm_read_byte( &Caption_mV[  j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case ']': b=pgm_read_byte( &Caption_mA[  j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '#': b=pgm_read_byte( &Caption_mW[  j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    default:  b=pgm_read_byte( &SmallSpace [ j * w + i + 2 ] );*LCD_Memory_1 &= b; *LCD_Memory_1 |= b;
                }
            }
        }
        x = x + w;
    }
    Send_FrameBuffer_To_LCD();
}
//-----------------------------------------------------------------------------
