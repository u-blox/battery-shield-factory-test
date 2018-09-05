/* mbed Microcontroller Library
 * Copyright (c) 2017 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "battery_gauge_bq35100.h"

/* This is the test program for the C030 primary battery
 * shield.  It instantiates a BQ35100 battery gauge and 
 * lights the LED on the C030 board as follows:
 *
 * Flashing amber: running but no conclusion yet.
 * Flashing green: pass (BQ35100 is there and battery is at or
 *                 above threshold)
 * Flashing red:   fail (not able to enable gauging on BQ35100
 *                 or battery is below threshold)
 * Solid red:      failed to even start.
 *
 * Progress may also be monitored with a serial terminal running
 * at 9600 baud.
 */

// The pass threshold
#define BATTERY_VOLTAGE_PASS_THRESHOLD_MV 2800

// The period of the LED blink
#define BLINK_PERIOD_MS 1000

// Possible states
typedef enum {
    STATE_NULL,
    STATE_RUNNING,
    STATE_PASS,
    STATE_FAIL
} State;

// LEDs
DigitalOut ledRed(LED1, 1);
DigitalOut ledGreen(LED2, 1);
DigitalOut ledBlue(LED3, 1);

// State
State state = STATE_NULL;

// Ticker to blink LEDs
Ticker ticker;

void blinkLed()
{
    switch (state) {
        case STATE_NULL:
            // All off
            ledRed = 1;
            ledGreen = 1;
            ledBlue = 1;
        break;
        case STATE_RUNNING:
            ledBlue = 1;

            // Flashing amber
            ledGreen = !ledGreen;
            ledRed = !ledRed;
        break;
        case STATE_PASS:
            ledBlue = 1;
            ledRed = 1;

            // Flashing green
            ledGreen = !ledGreen;
        break;
        case STATE_FAIL:
            ledBlue = 1;
            ledGreen = 1;

            // Flashing red
            ledRed = !ledRed;
        break;
        default:
            // Should never get here
            ledGreen = 1;
            ledBlue = 1;
            ledRed = 0;
            MBED_ASSERT(false);
        break;
    }
}

int main()
{
    I2C i2C(I2C_SDA, I2C_SCL);
    BatteryGaugeBq35100 gauge;
    int32_t reading;

    printf ("Starting up...\n");
    ticker.attach_us(blinkLed, BLINK_PERIOD_MS * 1000 / 2);
    if (gauge.init(&i2C)) {
        state = STATE_RUNNING;
        printf ("BQ35100 battery gauge chip initialised.\n");
        while (1) {
            gauge.enableGauge();
            if (gauge.isGaugeEnabled()) {
                if (gauge.getVoltage(&reading)) {
                    printf("Battery voltage: %.3f V (pass threshold %.3f V).\n",
                           ((float) reading) / 1000,
                           ((float) BATTERY_VOLTAGE_PASS_THRESHOLD_MV) / 1000);
                    if (reading >= BATTERY_VOLTAGE_PASS_THRESHOLD_MV) {
                        state = STATE_PASS;
                    } else {
                        state = STATE_FAIL;
                    }
                } else {
                    printf("Unable to read battery voltage.\n");
                    state = STATE_FAIL;
                }
            } else {
                printf("Battery gauging could not be enabled.\n");
                state = STATE_FAIL;
            }
            wait_ms(BLINK_PERIOD_MS);
        }
    }
    state = STATE_FAIL;
    printf("Should never get here.\n");
    while(1) {}
}

// End Of File