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

Reference: https://reference.epson-biz.com/modules/ref_escpos/index.php?content_id=72
------------------------------------------------------------------------*/

#include "ESC_POS_Printer.h"

// ASCII codes used by some of the printer config commands:
#define ASCII_TAB '\t' // Horizontal tab
#define ASCII_LF  '\n' // Line feed
#define ASCII_FF  '\f' // Form feed
#define ASCII_CR  '\r' // Carriage return
#define ASCII_EOT   4  // End of Transmission
#define ASCII_DLE  16  // Data Link Escape
#define ASCII_DC2  18  // Device control 2
#define ASCII_ESC  27  // Escape
#define ASCII_FS   28  // Field separator
#define ASCII_GS   29  // Group separator

// Constructor
ESC_POS_Printer::ESC_POS_Printer(Stream *s) :
    stream(s) {
    }

// The next four helper methods are used when issuing configuration
// commands, printing bitmaps or barcodes, etc.  Not when printing text.

void ESC_POS_Printer::writeBytes(uint8_t a) {
    stream->write(a);
}

void ESC_POS_Printer::writeBytes(uint8_t a, uint8_t b) {
    uint8_t cmd[2] = {a, b};
    stream->write(cmd, sizeof(cmd));
}

void ESC_POS_Printer::writeBytes(uint8_t a, uint8_t b, uint8_t c) {
    uint8_t cmd[3] = {a, b, c};
    stream->write(cmd, sizeof(cmd));
}

void ESC_POS_Printer::writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    uint8_t cmd[4] = {a, b, c, d};
    stream->write(cmd, sizeof(cmd));
}

// The underlying method for all high-level printing (e.g. println()).
// The inherited Print class handles the rest!
size_t ESC_POS_Printer::write(uint8_t c) {

    if(c != 0x13) { // Strip carriage returns
        stream->write(c);
        if((c == '\n') || (column == maxColumn)) { // If newline or wrap
            column = 0;
            c      = '\n'; // Treat wrap as newline on next pass
        } else {
            column++;
        }
        prevByte = c;
    }

    return 1;
}

void ESC_POS_Printer::begin() {

    // The printer can't start receiving data immediately upon power up --
    // it needs a moment to cold boot and initialize.  Allow at least 1/2
    // sec of uptime before printer can receive data.

    wake();
    reset();
}

// Reset printer to default state.
void ESC_POS_Printer::reset() {
    writeBytes(ASCII_ESC, '@'); // Init command
    prevByte      = '\n';       // Treat as if prior line is blank
    column        =    0;
    maxColumn     =   32;
    charHeight    =   24;
    lineSpacing   =    6;
    barcodeHeight =   50;
}

// Reset text formatting parameters.
void ESC_POS_Printer::setDefault(){
    online();
    justify('L');
    inverseOff();
    doubleHeightOff();
    setLineHeight(30);
    boldOff();
    underlineOff();
    setBarcodeHeight(50);
    setSize('s');
    setCharset();
    setCodePage();
}

void ESC_POS_Printer::test(){
    println(F("Hello World!"));
    feed(2);
}

void ESC_POS_Printer::testPage() {
    char commandTest[] = {ASCII_GS, '(', 'A', 2, 0, 0, 3};
    stream->write(commandTest, sizeof(commandTest));
}

void ESC_POS_Printer::setBarcodeHeight(uint8_t val) { // Default is 50
    if(val < 1) val = 1;
    barcodeHeight = val;
    // This does not work on my printer. It prints a '2' = 0x32 = 50
    //writeBytes(ASCII_GS, 'h', val);
}

void ESC_POS_Printer::printBarcode(const char *text, uint8_t type) {
    feed(1); // Recent firmware can't print barcode w/o feed first???
    writeBytes(ASCII_GS, 'H', 2);    // Print label below barcode
    writeBytes(ASCII_GS, 'w', 3);    // Barcode width 3 (0.375/1.0mm thin/thick)
    writeBytes(ASCII_GS, 'k', type); // Barcode type (listed in .h file)
    // Write text including the terminating '\0'
    stream->write(text, strlen(text)+1);
    prevByte = '\n';
}

// === Character commands ===

#define INVERSE_MASK       (1 << 1) // Not in 2.6.8 firmware (see inverseOn())
//#define UPDOWN_MASK        (1 << 2)
//#define BOLD_MASK          (1 << 3)
#define DOUBLE_HEIGHT_MASK (1 << 4)
#define DOUBLE_WIDTH_MASK  (1 << 5)
//#define STRIKE_MASK        (1 << 6)
#define UNDERLINE_MASK     (1 << 7)

void ESC_POS_Printer::setPrintMode(uint8_t mask) {
    printMode |= mask;
    writePrintMode();
    charHeight = (printMode & DOUBLE_HEIGHT_MASK) ? 48 : 24;
    maxColumn  = (printMode & DOUBLE_WIDTH_MASK ) ? 16 : 32;
}

void ESC_POS_Printer::unsetPrintMode(uint8_t mask) {
    printMode &= ~mask;
    writePrintMode();
    charHeight = (printMode & DOUBLE_HEIGHT_MASK) ? 48 : 24;
    maxColumn  = (printMode & DOUBLE_WIDTH_MASK ) ? 16 : 32;
}

void ESC_POS_Printer::writePrintMode() {
    writeBytes(ASCII_ESC, '!', printMode);
}

void ESC_POS_Printer::normal() {
    printMode = 0;
    writePrintMode();
    upsideDownOff();
}

void ESC_POS_Printer::inverseOn(){
    writeBytes(ASCII_GS, 'B', 1);
}

void ESC_POS_Printer::inverseOff(){
    writeBytes(ASCII_GS, 'B', 0);
}

void ESC_POS_Printer::upsideDownOn(){
    writeBytes(ASCII_ESC, '{', 1);
}

void ESC_POS_Printer::upsideDownOff(){
    writeBytes(ASCII_ESC, '{', 0);
}

void ESC_POS_Printer::doubleHeightOn(){
    setPrintMode(DOUBLE_HEIGHT_MASK);
}

void ESC_POS_Printer::doubleHeightOff(){
    unsetPrintMode(DOUBLE_HEIGHT_MASK);
}

void ESC_POS_Printer::doubleWidthOn(){
    setPrintMode(DOUBLE_WIDTH_MASK);
}

void ESC_POS_Printer::doubleWidthOff(){
    unsetPrintMode(DOUBLE_WIDTH_MASK);
}

void ESC_POS_Printer::strikeOn(){
    writeBytes(ASCII_ESC, 'G', 1);
}

void ESC_POS_Printer::strikeOff(){
    writeBytes(ASCII_ESC, 'G', 0);
}

void ESC_POS_Printer::boldOn(){
    writeBytes(ASCII_ESC, 'E', 1);
}

void ESC_POS_Printer::boldOff(){
    writeBytes(ASCII_ESC, 'E', 0);
}

void ESC_POS_Printer::justify(char value){
    uint8_t pos = 0;

    switch(toupper(value)) {
        case 'L': pos = 0; break;
        case 'C': pos = 1; break;
        case 'R': pos = 2; break;
    }

    writeBytes(ASCII_ESC, 'a', pos);
}

// Feeds by the specified number of lines
void ESC_POS_Printer::feed(uint8_t x) {
    writeBytes(ASCII_ESC, 'd', x);
    prevByte = '\n';
    column   =    0;
}

// Feeds by the specified number of individual pixel rows
void ESC_POS_Printer::feedRows(uint8_t rows) {
    writeBytes(ASCII_ESC, 'J', rows);
    prevByte = '\n';
    column   =    0;
}

void ESC_POS_Printer::flush() {
    writeBytes(ASCII_FF);
}

void ESC_POS_Printer::setSize(char value){
    uint8_t size;

    switch(toupper(value)) {
        default:  // Small: standard width and height
            size       = 0x00;
            charHeight = 24;
            maxColumn  = 32;
            break;
        case 'M': // Medium: double height
            size       = 0x01;
            charHeight = 48;
            maxColumn  = 32;
            break;
        case 'L': // Large: double width and height
            size       = 0x11;
            charHeight = 48;
            maxColumn  = 64;
            break;
    }

    writeBytes(ASCII_GS, '!', size);
    prevByte = '\n'; // Setting the size adds a linefeed
}

void ESC_POS_Printer::setSize(uint8_t height, uint8_t width) {
    uint8_t size = ((width & 0x7) << 3) | (height & 0x7);

    writeBytes(ASCII_GS, '!', size);
    prevByte = '\n'; // Setting the size adds a linefeed
}

// Underlines of different weights can be produced:
// 0 - no underline
// 1 - normal underline
// 2 - thick underline
void ESC_POS_Printer::underlineOn(uint8_t weight) {
    if(weight > 2) weight = 2;
    writeBytes(ASCII_ESC, '-', weight);
}

void ESC_POS_Printer::underlineOff() {
    writeBytes(ASCII_ESC, '-', 0);
}

// ASCII  ESC *   m nL nH d1...dk
// Hex    1B  2A  m nL nH d1...dk
// Dec    27  42  m nL nH d1...dk
// m = 0  single density vert, single horizontal
// m = 1  single density vert, double horiz
// m = 32 double density vert, single horizontal
// m = 33 double density vert, double horiz
void ESC_POS_Printer::printBitmap(
        int w, int h, const uint8_t *bitmap, int density) {
    uint8_t band_height;
    uint8_t bitmap_command[] = { 0x1b, '*', 0, 0, 0 };
    size_t w_bytes = w;

    switch (density) {
        default:
            density = 1;
            /* fall through */
        case 1:
            bitmap_command[2] = 0;          // m = single density
            band_height = 8;
            break;
        case 2:
            bitmap_command[2] = 33;         // m = double density
            band_height = 24;
            w_bytes *= 3;
            break;
    }
    bitmap_command[3] = w & 0xFF;       // nL = width LS byte
    bitmap_command[4] = (w >> 8) & 0xFF;// nH = width MS byte

    // Line spacing = 16 dots
    stream->write("\x1b\x33\x10\x1bU\x01");   // Unidirectional print mode on
    for (int row = 0; row < h; row += band_height) {
        stream->write(bitmap_command, sizeof(bitmap_command));
        stream->write(bitmap, w_bytes);
        stream->write('\n');
        bitmap += w_bytes;
    }
    stream->write("\x1b\x32\x1bU");     // Default line spacing
    stream->write((uint8_t)0);          // Unidirectional print mode off
    prevByte = '\n';
}

void ESC_POS_Printer::printBitmap_P(
        int w, int h, const uint8_t *bitmap, int density) {
    uint8_t band_height;
    uint8_t bitmap_command[] = { 0x1b, '*', 0, 0, 0 };
    size_t w_bytes = w;
    uint8_t buf[64];
    PGM_P p = reinterpret_cast<PGM_P>(bitmap);

    switch (density) {
        default:
            density = 1;
            /* fall through */
        case 1:
            bitmap_command[2] = 0;          // m = single density
            band_height = 8;
            break;
        case 2:
            bitmap_command[2] = 33;         // m = double density
            band_height = 24;
            w_bytes *= 3;
            break;
    }
    bitmap_command[3] = w & 0xFF;       // nL = width LS byte
    bitmap_command[4] = (w >> 8) & 0xFF;// nH = width MS byte

    // Line spacing = 16 dots
    stream->write("\x1b\x33\x10\x1bU\x01");   // Unidirectional print mode on
    for (int row = 0; row < h; row += band_height) {
        memcpy(buf, bitmap_command, sizeof(bitmap_command));
        size_t len = sizeof(bitmap_command);
        size_t outlen;
        for (size_t col = 0; col < w_bytes; col += outlen) {
            outlen = min(sizeof(buf)-len, w_bytes - col);
            memcpy_P(buf+len, p, outlen);
            len += outlen;
            p += outlen;
            stream->write(buf, len);
            len = 0;
        }
        stream->write('\n');
    }
    // The count correctly includes the trailing '\0'!
    stream->write("\x1b\x32\x1bU", 5);    // Default line spacing,
    // Unidirectional print mode off
    prevByte = '\n';
}

void ESC_POS_Printer::printBitmap(
        int w, int h, const uint8_t *bitmap, bool fromProgMem) {
    int rowBytes, rowBytesClipped, rowStart, chunkHeight, chunkHeightLimit,
        x, y, i;

    rowBytes        = (w + 7) / 8; // Round up to next byte boundary
    rowBytesClipped = (rowBytes >= 48) ? 48 : rowBytes; // 384 pixels max width

    chunkHeightLimit = 255; // Buffer doesn't matter, handshake!

    for(i=rowStart=0; rowStart < h; rowStart += chunkHeightLimit) {
        // Issue up to chunkHeightLimit rows at a time:
        chunkHeight = h - rowStart;
        if(chunkHeight > chunkHeightLimit) chunkHeight = chunkHeightLimit;

        writeBytes(ASCII_DC2, '*', chunkHeight, rowBytesClipped);

        for(y=0; y < chunkHeight; y++) {
            for(x=0; x < rowBytesClipped; x++, i++) {
                stream->write(fromProgMem ? pgm_read_byte(bitmap + i) : *(bitmap+i));
            }
            i += rowBytes - rowBytesClipped;
        }
    }
    prevByte = '\n';
}

void ESC_POS_Printer::printBitmap(int w, int h, Stream *fromStream) {
    int rowBytes, rowBytesClipped, rowStart, chunkHeight, chunkHeightLimit,
        x, y, i, c;

    rowBytes        = (w + 7) / 8; // Round up to next byte boundary
    rowBytesClipped = (rowBytes >= 48) ? 48 : rowBytes; // 384 pixels max width

    // Est. max rows to write at once, assuming 256 byte printer buffer.
    chunkHeightLimit = 255; // Buffer doesn't matter, handshake!

    for(rowStart=0; rowStart < h; rowStart += chunkHeightLimit) {
        // Issue up to chunkHeightLimit rows at a time:
        chunkHeight = h - rowStart;
        if(chunkHeight > chunkHeightLimit) chunkHeight = chunkHeightLimit;

        writeBytes(ASCII_DC2, '*', chunkHeight, rowBytesClipped);

        for(y=0; y < chunkHeight; y++) {
            for(x=0; x < rowBytesClipped; x++) {
                while((c = fromStream->read()) < 0);
                stream->write((uint8_t)c);
            }
            for(i = rowBytes - rowBytesClipped; i>0; i--) {
                while((c = fromStream->read()) < 0);
            }
        }
    }
    prevByte = '\n';
}

void ESC_POS_Printer::printBitmap(Stream *fromStream) {
    uint8_t  tmp;
    uint16_t width, height;

    tmp    =  fromStream->read();
    width  = (fromStream->read() << 8) + tmp;

    tmp    =  fromStream->read();
    height = (fromStream->read() << 8) + tmp;

    printBitmap(width, height, fromStream);
}

// Take the printer offline. Print commands sent after this will be
// ignored until 'online' is called.
void ESC_POS_Printer::offline(){
    writeBytes(ASCII_ESC, '=', 0);
}

// Take the printer back online. Subsequent print commands will be obeyed.
void ESC_POS_Printer::online(){
    writeBytes(ASCII_ESC, '=', 1);
}

// Put the printer into a low-energy state immediately.
void ESC_POS_Printer::sleep() {
    sleepAfter(1); // Can't be 0, that means 'don't sleep'
}

// Put the printer into a low-energy state after the given number
// of seconds.
void ESC_POS_Printer::sleepAfter(uint16_t seconds) {
    writeBytes(ASCII_ESC, '8', seconds, seconds >> 8);
}

// Wake the printer from a low-energy state.
void ESC_POS_Printer::wake() {
}

// Check the status of the paper using the printer's self reporting
// ability.  Returns true for paper, false for no paper.
// Might not work on all printers!
bool ESC_POS_Printer::hasPaper() {
    //  writeBytes(ASCII_DLE, ASCII_EOT, 4);
    writeBytes(ASCII_GS, 'r', 1);
    //  writeBytes(ASCII_ESC, 'v');

    int status = 0;
    for(uint8_t i=0; i<10; i++) {
        if(stream->available()) {
            status = stream->read();
            break;
        }
        delay(100);
    }

    return !(status & 0b00001100);
}

void ESC_POS_Printer::setLineHeight(int val) {
    if(val < 24) val = 24;
    lineSpacing = val - 24;

    // The printer doesn't take into account the current text height
    // when setting line height, making this more akin to inter-line
    // spacing.  Default line spacing is 30 (char height of 24, line
    // spacing of 6).
    writeBytes(ASCII_ESC, '3', val);
}

void ESC_POS_Printer::setMaxChunkHeight(int val) {
}

// Alters some chars in ASCII 0x23-0x7E range; see datasheet
void ESC_POS_Printer::setCharset(uint8_t val) {
    writeBytes(ASCII_ESC, 'R', val);
}

// Selects alt symbols for 'upper' ASCII values 0x80-0xFF
void ESC_POS_Printer::setCodePage(uint8_t val) {
    writeBytes(ASCII_ESC, 't', val);
}

void ESC_POS_Printer::tab() {
    writeBytes(ASCII_TAB);
    column = (column + 4) & 0b11111100;
}

void ESC_POS_Printer::setCharSpacing(int spacing) {
    writeBytes(ASCII_ESC, ' ', spacing);
}
