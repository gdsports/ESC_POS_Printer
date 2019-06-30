# ESC_POS_Printer
Arduino ESC POS printer library

This is an Arduino library for ESC/POS point of sale receipt printers. This
library is based on the Adafruit Thermal library but is not compatible with the
Adafruit thermal printer. This library has been tested with a USB thermal
receipt printer and the following library on a Teensy 3.6 USB host port.

This library works with the following USB Printer drivers for the following
Arduino USB host stacks.

* [Teensy 3.6 USB host](https://github.com/gdsports/USBPrinter_t36)
* [USB Host Shield 2.0](https://github.com/gdsports/USBPrinter_uhs2)
* [USB Host Library for SAMD](https://github.com/gdsports/USBPrinter_uhls)

SAMD boards include the Arduino Zero, the Arduino MKR family, many SparkFun
and Adafruit SAMD21, SAMD51, M0, and M4 boards. The SAMD processors have
built-in USB host and device hardware so a shield is not used.

The following is used as reference for ESC/POS commands.

https://reference.epson-biz.com/modules/ref_escpos/index.php

## Original text from the Adafruit Thermal Library

Adafruit invests time and resources providing this open source code.  Please
support Adafruit and open-source hardware by purchasing products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from
the open source community.  Originally based on Thermal library from bildr.org
MIT license, all text above must be included in any redistribution.
