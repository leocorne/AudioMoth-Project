/* Edge Impulse inferencing library
 * Copyright (c) 2020 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "audioMoth.h"
#include "ei_classifier_porting.h"

__attribute__((weak)) EI_IMPULSE_ERROR ei_run_impulse_check_canceled() {
    return EI_IMPULSE_OK;
}

int BUFFER_LEN = 256;

/**
 * Cancelable sleep, can be triggered with signal from other thread
 */
__attribute__((weak)) EI_IMPULSE_ERROR ei_sleep(int32_t time_ms) {
    AudioMoth_delay(time_ms);
    return EI_IMPULSE_OK;
}

uint64_t ei_read_timer_ms() {
    uint32_t s;
    uint16_t ms;  

    AudioMoth_getTime(&s, &ms);

    return (uint64_t) ((s * 1000) + ms);
}

uint64_t ei_read_timer_us() {
    // TODO: find out if we can actually get microseconds
    uint32_t s;
    uint16_t ms;  

    AudioMoth_getTime(&s, &ms);

    return ((uint64_t)(s * 1000000)) + ((uint64_t)(ms * 1000));
}

void ei_printf(const char *format, ...) {
    // va_list myargs;
    // va_start(myargs, format);

    // char msgbuffer[BUFFER_LEN];
    // vsprintf(msgbuffer, format, myargs);

    // AudioMoth_appendFile(LOGS_FILE);
    // AudioMoth_writeToFile(msgbuffer, strlen(msgbuffer));
    // AudioMoth_closeFile();  

    // va_end(myargs);
}

void ei_printf_float(float f) {
    // char str[100];

    // char *tmpSign = (f < 0) ? "-" : "";
    // float tmpVal = (f < 0) ? -f : f;

    // int tmpInt1 = tmpVal;                  // Get the integer.
    // float tmpFrac = tmpVal - tmpInt1;      // Get fraction.
    // int tmpInt2 = (int)(tmpFrac * 10000);  // Turn into integer.

    // // Print as parts, note that you need 0-padding for fractional bit.

    // sprintf(str, "%s%d.%04d\n", tmpSign, tmpInt1, tmpInt2);

    // ei_printf("%s", str);
}

void ei_printf_force(const char *format, ...) {
    va_list myargs;
    va_start(myargs, format);

    char msgbuffer[BUFFER_LEN];
    vsprintf(msgbuffer, format, myargs);

    AudioMoth_appendFile(LOGS_FILE);
    AudioMoth_writeToFile(msgbuffer, strlen(msgbuffer));
    AudioMoth_closeFile();  

    va_end(myargs);
}

void ei_printf_force_float(float f) {
    char str[100];

    char *tmpSign = (f < 0) ? "-" : "";
    float tmpVal = (f < 0) ? -f : f;

    int tmpInt1 = tmpVal;                  // Get the integer.
    float tmpFrac = tmpVal - tmpInt1;      // Get fraction.
    int tmpInt2 = (int)(tmpFrac * 10000);  // Turn into integer.

    // Print as parts, note that you need 0-padding for fractional bit.

    sprintf(str, "%s%d.%04d\n", tmpSign, tmpInt1, tmpInt2);

    ei_printf_force("%s", str);
}

// extern "C" void DebugLog(const char* s) {
//     ei_printf("%s", s);
// }
