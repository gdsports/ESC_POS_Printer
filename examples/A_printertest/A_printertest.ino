/*------------------------------------------------------------------------
  Example sketch for Adafruit Thermal Printer library for Arduino.
  Demonstrates a few text styles & layouts, bitmap printing, etc.

  IMPORTANT: DECLARATIONS DIFFER FROM PRIOR VERSIONS OF THIS LIBRARY.
  This is to support newer & more board types, especially ones that don't
  support SoftwareSerial (e.g. Arduino Due).  You can pass any Stream
  (e.g. Serial1) to the printer constructor.  See notes below.

  You may need to edit the PRINTER_FIRMWARE value in Adafruit_Thermal.h
  to match your printer (hold feed button on powerup for test page).
  ------------------------------------------------------------------------*/

#include "USBHost_t36.h"
#include "USBPrinter_t36.h"
#include "ESC_POS_Printer.h"
#include "qrcode.h"

USBHost myusb;
USBPrinter uprinter(myusb);
ESC_POS_Printer printer(&uprinter);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) delay(1);
  Serial.println(F("Hello printer"));
  myusb.begin();
  Serial.println(F("Init USB"));
  uprinter.begin();
  Serial.println(F("Init USB printer"));
}

void printer_test()
{
  printer.reset();
  printer.setDefault(); // Restore printer to defaults

  // Test inverse on & off
  printer.inverseOn();
  printer.println(F("Inverse ON"));
  printer.inverseOff();

  // Test character double-height on & off
  printer.doubleHeightOn();
  printer.println(F("Double Height ON"));
  printer.doubleHeightOff();

  // Set text justification (right, center, left) -- accepts 'L', 'C', 'R'
  printer.justify('R');
  printer.println(F("Right justified"));
  printer.justify('C');
  printer.println(F("Center justified"));
  printer.justify('L');
  printer.println(F("Left justified"));

  // Test more styles
  printer.boldOn();
  printer.println(F("Bold text"));
  printer.boldOff();

  printer.underlineOn();
  printer.println(F("Underlined text"));
  printer.underlineOff();

  printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
  printer.println(F("Large"));
  printer.setSize('M');
  printer.println(F("Medium"));
  printer.setSize('S');
  printer.println(F("Small"));

  printer.justify('C');
  printer.println(F("normal\nline\nspacing"));
  printer.setLineHeight(50);
  printer.println(F("Taller\nline\nspacing"));
  printer.setLineHeight(); // Reset to default
  printer.justify('L');

  // Print the QR code in graphics mode
  // See the ./bitmap/mkqrcode program to generate qrcode.h
  Serial.print(F("qrcode_data size = ")); Serial.println(sizeof(qrcode_data));
  printer.printBitmap(qrcode_width, qrcode_height, qrcode_data, qrcode_density);
  printer.println(F("ESC_POS_Printer"));

  printer.feed(2);
}

void loop() {
  myusb.Task();

  // Make sure USB printer found and ready
  if (uprinter) {
    printer.begin();
    Serial.println(F("Init ESC POS printer"));

    printer_test();
    // Do this one time to avoid wasting paper
    while (1) delay(1);
  }
}
