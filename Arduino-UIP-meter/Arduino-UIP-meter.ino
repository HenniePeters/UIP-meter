//=============================================================================
#include "Arduino.h"
#include "Fonts.h"
//=============================================================================
void setup() {
    //General setup, port directions.
    // PB5 (0x20) is SCK  (output) green  OLED pin 12
    pinMode( SPI_SCK_PIN, OUTPUT );
    // PB4 (0x10) is MISO (input)  blue   OLED pin 13
    //(reference only, it is an input)
    pinMode( SPI_MISO_PIN, INPUT );
    // PB3 (0x08) is MOSI (output) violet OLED pin 14
    pinMode( SPI_MOSI_PIN, OUTPUT );
    // DB2 (0x04) is SS   (output) gray   OLED pin 16
    pinMode( SPI_SS_PIN, OUTPUT );

    //SPI Clock is idle low  (SPI data is don't care)
    digitalWrite( SPI_SCK_PIN, LOW );
    //device not selected
    digitalWrite( SPI_SS_PIN, LOW );

    Init_LCD();
}
//===========================================================================
char *formatVoltage( char *buf, double value ) {
    double f = value;
    if( f <= 0.999 ) {
        f *= 1000.0;
        f += 0.5;
        if( f < 10.0 ) {
            sprintf( buf, "%1d        ", ( int )( f ) );
        } else if( f < 100.0 ) {
            sprintf( buf, "%2d      ", ( int )( f ) );
        } else {
            sprintf( buf, "%3d    ", ( int )( f ) );
        }
    } else if( f >= 100.0 ) {
        f += 0.5;
        sprintf( buf, "%d    ", ( int )( f ) );
    } else if( f >= 10.0 ) {
        f += 0.05;
        sprintf( buf, "%d.%01d   ", ( int )( f ), ( int )( 10.0 * f ) % 10 );
    } else {
        f += 0.005;
        sprintf( buf, "%d.%02d   ", ( int )( f ), ( int )( 100.0 * f ) % 100 );
    }
    return buf;
}
//===========================================================================
char *formatCurrent( char *buf, double value ) {
    double f = value;
    if( f <= 0.999 ) {
        f *= 1000.0;
        f += 0.5;
        if( f < 10.0 ) {
            sprintf( buf, "      %1d", ( int )( f ) );
        } else if( f < 100.0 ) {
            sprintf( buf, "    %2d", ( int )( f ) );
        } else {
            sprintf( buf, "  %3d", ( int )( f ) );
        }

    } else if( f >= 100.0 ) {
        f += 0.5;
        sprintf( buf, "  %d", ( int )( f ) );
    } else if( f >= 10.0 ) {
        f += 0.05;
        sprintf( buf, " %d.%01d", ( int )( f ), ( int )( 10.0 * f ) % 10 );
    } else {
        f += 0.005;
        sprintf( buf, " %d.%02d", ( int )( f ), ( int )( 100.0 * f ) % 100 );
    }
    return buf;
}
//===========================================================================
char *formatPower( char *buf, double value ) {
    double f = value;
    if( f <= 0.999 ) {
        f *= 1000.0;
        f += 0.5;
        if( f < 10.0 ) {
            sprintf( buf, "%3d#   ", ( int )( f ) );
        } else if( f < 100.0 ) {
            sprintf( buf, "%3d#   ", ( int )( f ) );
        } else {
            sprintf( buf, " %3d#  ", ( int )( f ) );
        }
    } else if( f >= 100 ) {
        f += 0.5;
        sprintf( buf, " %ldW  ", ( uint32_t )( f ) );
    } else {
        f += 0.05;
        sprintf( buf, "%3d.%dW  ", ( int )( f ), ( int )( 10.0 * f ) % 10 );
    }
    return buf;
}
//===========================================================================
void loop() {
    static char Voltage[8], Current[8], buf[16];
    static double U, I;
    double add = 0.05;
    uint32_t next = 0, period = 500/*ms*/;
    U = 0;
    while( 1 ) {
        if( ( uint32_t )( millis() - next ) > period ) {
            next = millis();
            
            // Debug data  
            U += add;
            I = U / 2.75;
            if( U >= 2000 ) {
                break;
            } else if( U >= 1000 ) {
                add = 100;
            } else if( U >= 200 ) {
                add = 50;
            } else if( U >= 100 ) {
                add = 10;
            } else if( U >= 20 ) {
                add = 5;
            } else if( U >= 10 ) {
                add = 1;
            } else if( U >= 0.9995 ) {
                add = 0.5;
            }
            
            formatVoltage( Voltage, U );
            PrintLarge( 0, 0, Voltage );

            formatCurrent( Current, I );
            PrintLarge( 10, 0, Current );

            if( U <= 0.999 ) {
                PrintSmall( 0, 22, "[  " );  // "mV  "
            } else {
                PrintSmall( 0, 22, "{  " );  // "V   "
            }

            formatPower( buf, I * U );     // + "mW/W"
            PrintSmall( 5, 22, buf );

            if( I <= 0.999 ) {
                PrintSmall( 15, 22, "]" ); // " mA"
            } else {
                PrintSmall( 15, 22, "}" ); // "  A"
            }

            Send_FrameBuffer_To_LCD();
        }
        //Send_FrameBuffer_To_LCD(); // maximum visible distortion by rewriting
        //_delay_ms(50);             // the whole display every 50ms 
    }
}
//===========================================================================
