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
void SPI_WriteByte( register uint8_t data ) {
#if FAST_SPI
    //Pretty fast software SPI 8-bit transfer code
    //Several ideas taken from the fastest non-assembly implementation at:
    //http://nerdralph.blogspot.com/2015/03/fastest-avr-software-spi-in-west.html
    //
    //Pre-calculate the value that will drive clk and data low in one cycle.
    //(Note that this is not interrupt safe if an ISR is writing to the same port)
    /*
        register uint8_t force_clk_and_data_low = ( SPIPORT & ~( MOSI_MASK | SCK_MASK ) ); // Pre-calculate the value that will drive clk and data low in one operation.
        // Now clock the 8 bits of data out. It needs the delays.
        // _delay_loop_2 needs: #include <util/delay.h>
        SPIPORT = force_clk_and_data_low; if( data & 0x80 ) { SPITOGGLE = MOSI_MASK; } _delay_loop_2(3);
        SPITOGGLE = SCK_MASK;                                                          _delay_loop_2(3);
        SPIPORT = force_clk_and_data_low; if( data & 0x40 ) { SPITOGGLE = MOSI_MASK; } _delay_loop_2(3);
        SPITOGGLE = SCK_MASK;                                                          _delay_loop_2(3);
        SPIPORT = force_clk_and_data_low; if( data & 0x20 ) { SPITOGGLE = MOSI_MASK; } _delay_loop_2(3);
        SPITOGGLE = SCK_MASK;                                                          _delay_loop_2(3);
        SPIPORT = force_clk_and_data_low; if( data & 0x10 ) { SPITOGGLE = MOSI_MASK; } _delay_loop_2(3);
        SPITOGGLE = SCK_MASK;                                                          _delay_loop_2(3);
        SPIPORT = force_clk_and_data_low; if( data & 0x08 ) { SPITOGGLE = MOSI_MASK; } _delay_loop_2(3);
        SPITOGGLE = SCK_MASK;                                                          _delay_loop_2(3);
        SPIPORT = force_clk_and_data_low; if( data & 0x04 ) { SPITOGGLE = MOSI_MASK; } _delay_loop_2(3);
        SPITOGGLE = SCK_MASK;                                                          _delay_loop_2(3);
        SPIPORT = force_clk_and_data_low; if( data & 0x02 ) { SPITOGGLE = MOSI_MASK; } _delay_loop_2(3);
        SPITOGGLE = SCK_MASK;                                                          _delay_loop_2(3);
        SPIPORT = force_clk_and_data_low; if( data & 0x01 ) { SPITOGGLE = MOSI_MASK; } _delay_loop_2(3);
        SPITOGGLE = SCK_MASK;                                                          _delay_loop_2(3);
    */
    register uint8_t i = 8, portbits = ( SPIPORT & ~( MOSI_MASK | SCK_MASK ) );
    do {
        SPIPORT = portbits; // both SCK and MOSI low
        if( data & 0x80 ) {
            SPIPORT |= MOSI_MASK;
        }
        _delay_loop_2( 4 ); // needs: #include <util/delay.h>
        SPIPORT |= SCK_MASK;
        _delay_loop_2( 4 );
        data <<= 1;
    } while( --i );
    SPIPORT = portbits; // end with both SCK and MOSI low
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
/*
    void write_data( register uint8_t data ) {
    SET_SS; // Select the chip, starting a 24-bit SPI transfer
    SPI_WriteByte( 0xFA ); // send data header
    SPI_WriteByte( data & 0xF0 ); // send high nibble
    SPI_WriteByte( ( data << 4 ) ); // send low nibble
    CLR_SS; // Release the chip, ending the SPI transfer
    }
*/
//-----------------------------------------------------------------------------
void write_command( register uint8_t command ) {
    SET_SS; // Select the chip, starting a 24-bit SPI transfer
    SPI_WriteByte( 0xF8 ); // send command header
    SPI_WriteByte( command & 0xF0 ); // send high nibble
    SPI_WriteByte( ( command << 4 ) ); // send low nibble
    CLR_SS; // Release the chip, ending the SPI transfer
}
//-----------------------------------------------------------------------------
// X is only adressible to the word, a grouping of 16 horizontal pixels:
// 0-17 => 18 * 8 = 144
void Set_Graphics_Mode_Address( register uint8_t x_word, register uint8_t y ) {
    write_command( 0x34 );          // LCD_EXTEND,  Access extended functions register 
    write_command( 0x80 | y );      // LCD_ADDR,    Set Y pixel address
    write_command( 0x80 | x_word ); // LCD_ADDR,    Set X byte address
    write_command( 0x36 );          // LCD_GFXMODE, Exit extended functions register, stay in graphics mode
}
//-----------------------------------------------------------------------------
void Init_LCD() {
    write_command( 0x30 ); // LCD_BASIC, set 8 bit operation and basic instruction set
    write_command( 0x30 ); // repeat LCD_BASIC
    write_command( 0x01 ); // LCD_CLS
    _delay_ms( 2 );
    write_command( 0x06 ); // LCD_ADDRINC
    write_command( 0x0C ); // LCD_DISPLAYON
    write_command( 0x34 ); // LCD_EXTEND
    write_command( 0x36 ); // LCD_GFXMODE

    Fill_FrameBuffer( 0x00 ); // clear the framebuffer
    Send_FrameBuffer_To_LCD();
}
//-----------------------------------------------------------------------------
void Fill_FrameBuffer( uint8_t fill_data ) {
    //Set the FrameBuffer to a given data.
    for( uint8_t j = 0; j < FRAME_HEIGHT_PIXELS; j++ ) {
        for( uint8_t i = 0; i < FRAME_WIDTH_BYTES; i++ ) {
            framebuffer[j][i] = fill_data;
        }
    }
}
//-----------------------------------------------------------------------------
void Send_FrameBuffer_To_LCD( void ) {
    register uint8_t i, j, b;
    // Reducing the number of redundant writes by not repeating chip select and 0xFA
    // for every byte. Doing that per pixel line instead.
    //write_command( 0x34 );  // LCD_TXTMODE
/*
    write_command( 0x03 );  // LCD_SCROLL
    write_command( 0x40+32 ); // LCD_SCROLLADDR
    for( j = 0; j < FRAME_HEIGHT_PIXELS; j++ ) {
        Set_Graphics_Mode_Address( 0, j ); // Set Y address to this line, reset X address
        SET_SS;
        SPI_WriteByte( 0xFA ); // data
        for( i = 0; i < FRAME_WIDTH_BYTES; i++ ) {
            b = framebuffer[j][i];
            SPI_WriteByte( b & 0xF0 );  
            SPI_WriteByte( b << 4 );
        }
        CLR_SS;
    }
    write_command( 0x03 ); // LCD_SCROLL
    write_command( 0x40 ); // LCD_SCROLLADDR    
    for( j = FRAME_HEIGHT_PIXELS; j < (FRAME_HEIGHT_PIXELS<<1); j++ ) {
        Set_Graphics_Mode_Address( 0, j+32 ); // Set Y address to this line, reset X address
        SET_SS;
        SPI_WriteByte( 0xFA ); // data
        for( i = 0; i < FRAME_WIDTH_BYTES; i++ ) {
            b = framebuffer[j][i];
            SPI_WriteByte( b & 0xF0 );  
            SPI_WriteByte( b << 4 );
        }
        CLR_SS;
    }
    //write_command( 0x36 );      // LCD_GFXMODE
*/

    for( j = 0; j < FRAME_HEIGHT_PIXELS; j++ ) {
        Set_Graphics_Mode_Address( 0, j ); // Set Y address to this line, reset X address
        SET_SS;
        SPI_WriteByte( 0xFA ); // data
        for( i = 0; i < FRAME_WIDTH_BYTES; i++ ) {
            b = framebuffer[j][i];
            SPI_WriteByte( b & 0xF0 );  
            SPI_WriteByte( b << 4 );
        }
        CLR_SS;
    }
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
                    case '.': b = pgm_read_byte( &SmallDot[    j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '0': b = pgm_read_byte( &Small_0[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '1': b = pgm_read_byte( &Small_1[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '2': b = pgm_read_byte( &Small_2[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '3': b = pgm_read_byte( &Small_3[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '4': b = pgm_read_byte( &Small_4[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '5': b = pgm_read_byte( &Small_5[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '6': b = pgm_read_byte( &Small_6[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '7': b = pgm_read_byte( &Small_7[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '8': b = pgm_read_byte( &Small_8[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '9': b = pgm_read_byte( &Small_9[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case 'A': b = pgm_read_byte( &Small_A[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case 'V': b = pgm_read_byte( &Small_V[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case 'W': b = pgm_read_byte( &Small_W[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case 'm': b = pgm_read_byte( &Small_m[     j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '{': b = pgm_read_byte( &Caption_V[   j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '}': b = pgm_read_byte( &Caption_A[   j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '[': b = pgm_read_byte( &Caption_mV[  j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case ']': b = pgm_read_byte( &Caption_mA[  j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    case '#': b = pgm_read_byte( &Caption_mW[  j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b; break;
                    default:  b = pgm_read_byte( &SmallSpace [ j * w + i + 2 ] ); *LCD_Memory_1 &= b; *LCD_Memory_1 |= b;
                }
            }
        }
        x = x + w;
    }
}
//-----------------------------------------------------------------------------
