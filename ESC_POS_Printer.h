/*------------------------------------------------------------------------
  An Arduino library for the USB Thermal Printer using Epson ESC POS commands

  These printers use USB to communicate.

  This library is based on the Adafruit Thermal Printer Library.

  Adafruit invests time and resources providing this open source code.
  Please support USB and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries, with
  contributions from the open source community.  Originally based on
  Thermal library from bildr.org
  MIT license, all text above must be included in any redistribution.
  ------------------------------------------------------------------------*/

#ifndef ESC_POS_PRINTER_H
#define ESC_POS_PRINTER_H

#include "Arduino.h"

// Barcode types and charsets
#define UPC_A              65
#define UPC_E              66
#define EAN13              67
#define EAN8               68
#define CODE39             69
#define ITF                70
#define CODABAR            71
#define CODE93             72
#define CODE128            73
#define GS1_128            74
#define GS1_DATABAR_OMNI   75
#define GS1_DATABAR_TRUNC  76
#define GS1_DATABAR_LIMTD  77
#define GS1_DATABAR_EXPAN  78

#define CHARSET_USA                0
#define CHARSET_FRANCE             1
#define CHARSET_GERMANY            2
#define CHARSET_UK                 3
#define CHARSET_DENMARK1           4
#define CHARSET_SWEDEN             5
#define CHARSET_ITALY              6
#define CHARSET_SPAIN1             7
#define CHARSET_JAPAN              8
#define CHARSET_NORWAY             9
#define CHARSET_DENMARK2          10
#define CHARSET_SPAIN2            11
#define CHARSET_LATINAMERICA      12
#define CHARSET_KOREA             13
#define CHARSET_SLOVENIA          14
#define CHARSET_CROATIA           14
#define CHARSET_CHINA             15
#define CHARSET_VIETNAM           16
#define CHARSET_ARABIA            17
#define CHARSET_INDIA_DEVANAGARI  66
#define CHARSET_INDIA_BENGALI     67
#define CHARSET_INDIA_TAMIL       68
#define CHARSET_INDIA_TELUGU      69
#define CHARSET_INDIA_ASSAMESE    70
#define CHARSET_INDIA_ORIYA       71
#define CHARSET_INDIA_KANNANDA    72
#define CHARSET_INDIA_MALAYALAM   73
#define CHARSET_INDIA_GUJARATI    74
#define CHARSET_INDIA_PUNJABI     75
#define CHARSET_INDIA_MARATHI     82

#define CODEPAGE_CP437        0 // USA, Standard Europe
#define CODEPAGE_KATAKANA     1
#define CODEPAGE_CP850        2 // Multilingual
#define CODEPAGE_CP860        3 // Portuguese
#define CODEPAGE_CP863        4 // Canadian-French
#define CODEPAGE_CP865        5 // Nordic
#define CODEPAGE_WCP1251      6 // Cyrillic
#define CODEPAGE_CP866        7 // Cyrillic #2
#define CODEPAGE_MIK          8 // Cyrillic/Bulgarian
#define CODEPAGE_CP755        9 // East Europe, Latvian 2
#define CODEPAGE_IRAN        10
#define CODEPAGE_CP862       15 // Hebrew
#define CODEPAGE_WCP1252     16 // Latin 1
#define CODEPAGE_WCP1253     17 // Greek
#define CODEPAGE_CP852       18 // Latin 2
#define CODEPAGE_CP858       19 // Multilingual Latin 1 + Euro
#define CODEPAGE_IRAN2       20
#define CODEPAGE_LATVIAN     21
#define CODEPAGE_CP864       22 // Arabic
#define CODEPAGE_ISO_8859_1  23 // West Europe
#define CODEPAGE_CP737       24 // Greek
#define CODEPAGE_WCP1257     25 // Baltic
#define CODEPAGE_THAI        26
#define CODEPAGE_CP720       27 // Arabic
#define CODEPAGE_CP855       28
#define CODEPAGE_CP857       29 // Turkish
#define CODEPAGE_WCP1250     30 // Central Europe
#define CODEPAGE_CP775       31
#define CODEPAGE_WCP1254     32 // Turkish
#define CODEPAGE_WCP1255     33 // Hebrew
#define CODEPAGE_WCP1256     34 // Arabic
#define CODEPAGE_WCP1258     35 // Vietnam
#define CODEPAGE_ISO_8859_2  36 // Latin 2
#define CODEPAGE_ISO_8859_3  37 // Latin 3
#define CODEPAGE_ISO_8859_4  38 // Baltic
#define CODEPAGE_ISO_8859_5  39 // Cyrillic
#define CODEPAGE_ISO_8859_6  40 // Arabic
#define CODEPAGE_ISO_8859_7  41 // Greek
#define CODEPAGE_ISO_8859_8  42 // Hebrew
#define CODEPAGE_ISO_8859_9  43 // Turkish
#define CODEPAGE_ISO_8859_15 44 // Latin 3
#define CODEPAGE_THAI2       45
#define CODEPAGE_CP856       46
#define CODEPAGE_CP874       47

class ESC_POS_Printer : public Print {

    public:

        // IMPORTANT: constructor syntax has changed from prior versions
        // of this library.  Please see notes in the example code!
        ESC_POS_Printer(Stream *s=&Serial);

        size_t
            write(uint8_t c);
        void
            begin(),
            boldOff(),
            boldOn(),
            doubleHeightOff(),
            doubleHeightOn(),
            doubleWidthOff(),
            doubleWidthOn(),
            feed(uint8_t x=1),
            feedRows(uint8_t),
            flush(),
            inverseOff(),
            inverseOn(),
            justify(char value),
            offline(),
            online(),
            printBarcode(const char *text, uint8_t type),
            printBitmap(int w, int h, const uint8_t *bitmap, int density=1),
            printBitmap_P(int w, int h, const uint8_t *bitmap, int density=1),
            printBitmap(int w, int h, const uint8_t *bitmap, bool fromProgMem=true),
            printBitmap(int w, int h, Stream *fromStream),
            printBitmap(Stream *fromStream),
            normal(),
            reset(),
            setBarcodeHeight(uint8_t val=50),
            setCharSpacing(int spacing=0),
            setCharset(uint8_t val=0),
            setCodePage(uint8_t val=0),
            setDefault(),
            setLineHeight(int val=30),
            setMaxChunkHeight(int val=256),
            setSize(char value),
            setSize(uint8_t height, uint8_t width),
            setTimes(unsigned long, unsigned long),
            sleep(),
            sleepAfter(uint16_t seconds),
            strikeOff(),
            strikeOn(),
            tab(),
            test(),
            testPage(),
            underlineOff(),
            underlineOn(uint8_t weight=1),
            upsideDownOff(),
            upsideDownOn(),
            wake();
        bool
            hasPaper();

    private:

        Stream
            *stream;
        uint8_t
            printMode,
            prevByte,      // Last character issued to printer
            column,        // Last horizontal column printed
            maxColumn,     // Page width (output 'wraps' at this point)
            charHeight,    // Height of characters, in 'dots'
            lineSpacing,   // Inter-line spacing (not line height), in dots
            barcodeHeight, // Barcode height in dots, not including text
            maxChunkHeight;
        void
            writeBytes(uint8_t a),
            writeBytes(uint8_t a, uint8_t b),
            writeBytes(uint8_t a, uint8_t b, uint8_t c),
            writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d),
            setPrintMode(uint8_t mask),
            unsetPrintMode(uint8_t mask),
            writePrintMode();

};

#endif // ESC_POS_PRINTER_H
