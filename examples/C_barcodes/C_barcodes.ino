/*------------------------------------------------------------------------
  Demoonstrate barcode printing on ESC POS printers connecting to
  Teensy 3.6 USB host port.
  Based on Adafruit Thermal printer library.
  My cheap ESC POS printer does not print barcodes like this.
  ------------------------------------------------------------------------*/

#include "ESC_POS_Printer.h"
#include "USBHost_t36.h"
#include "USBPrinter_t36.h"

USBHost myusb;
USBPrinter uprinter(myusb);
ESC_POS_Printer printer(&uprinter);

// -----------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) delay(1);
  Serial.println(F("Barcode demo"));
  myusb.begin();
  Serial.println(F("Init USB"));
  uprinter.begin();
  Serial.println(F("Init USB printer"));
}

void print_barcode() {
  printer.justify('C');
  printer.boldOn();
  printer.println(F("BARCODE EXAMPLES\n"));
  printer.boldOff();
  printer.justify('L');

  // There seems to be some conflict between datasheet descriptions
  // of barcode formats and reality.  Try Wikipedia and/or:
  // http://www.barcodeisland.com/symbolgy.phtml

  // Also note that strings passed to printBarcode() are always normal
  // RAM-resident strings; PROGMEM strings (e.g. F("123")) are NOT used.

  // UPC-A: 12 digits
  printer.print(F("UPC-A:"));
  printer.printBarcode("123456789012", UPC_A);

  // UPC-E: 6 digits ???
/* Commented out because I can't get this one working yet
  printer.print(F("UPC-E:"));
  printer.printBarcode("123456", UPC_E);
*/

  // EAN-13: 13 digits (same as JAN-13)
  printer.print(F("EAN-13:"));
  printer.printBarcode("1234567890123", EAN13);

  // EAN-8: 8 digits (same as JAN-8)
  printer.print(F("EAN-8:"));
  printer.printBarcode("12345678", EAN8);

  // CODE 39: variable length w/checksum?, 0-9,A-Z,space,$%+-./:
  printer.print(F("CODE 39:"));
  printer.printBarcode("ESC POS", CODE39);

  // ITF: 2-254 digits (# digits always multiple of 2)
  printer.print(F("ITF:"));
  printer.printBarcode("1234567890", ITF);

  // CODABAR: variable length 0-9,A-D,%+-./:
  printer.print(F("CODABAR:"));
  printer.printBarcode("1234567890", CODABAR);

  // CODE 93: compressed version of Code 39?
  printer.print(F("CODE 93:"));
  printer.printBarcode("ESC POS", CODE93);

  // CODE 128: 2-255 characters (ASCII 0-127)
  printer.print(F("CODE128:"));
  printer.printBarcode("ESC POS", CODE128);

  printer.feed(2);
  printer.setDefault(); // Restore printer to defaults
}

void loop() {
  myusb.Task();

  // Make sure USB printer found and ready
  if (uprinter) {
    printer.begin();
    Serial.println(F("Init ESC POS printer"));

    print_barcode();
    // Do this one time to avoid wasting paper
    while (1) delay(1);
  }
}
