/*------------------------------------------------------------------------
  Teensy 3.6 USB host port with USB printer driver and ESC POS library
  Print one line on printer.
  ------------------------------------------------------------------------*/

#include "USBHost_t36.h"
#include "USBPrinter_t36.h"
#include "ESC_POS_Printer.h"

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

void loop() {
  myusb.Task();

  // Make sure USB printer found and ready
  if (uprinter) {
    printer.begin();
    Serial.println(F("Init ESC POS printer"));

    printer.setSize('L');   // L for large
    printer.println(F("Hello printer"));
    // Do this one time to avoid wasting paper
    while (1) delay(1);
  }
}
