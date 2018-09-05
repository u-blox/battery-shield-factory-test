# Battery Shield Factory Test

This is the mbed test program for the C030 primary battery shield.  It instantiates a BQ35100 battery gauge and lights the LED on the C030 board as follows:

* Flashing amber: running but no conclusion yet.
* Flashing green: pass (BQ35100 is there and battery is at or above the threshold of 2.8 Volts)
* Flashing red:   fail (not able to enable gauging on BQ35100 or battery is below threshold)
* Solid red:      failed to even start.

Progress may also be monitored with a serial terminal running at 9600 baud.

To build this program, import it into your mbed environment, set the target to any UBLOX_C030 target and the toolchain to your favourite toolchain (e.g. `GCC_ARM`) and compile it.  Then drag and drop the binary onto the C030 board to which the primary battery shield is attached.

The program is not actually specific to UBLOX_C030 boards, any mbed board with an I2C interface and a three-colour LED may be used (obviously you will need to change `main.cpp` so that the `LED1`, `LED2`, `LED3`, `I2C_SDA` and `I2C_SCL` pins are correct for that board).

A pre-built binary that may be used on a UBLOX_C030_U201 board is also included.