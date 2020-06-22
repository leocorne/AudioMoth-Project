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
#include "us_ticker_api.h"
#include "audioMoth.h"

using namespace rtos;

int BUFFER_LEN = 256;

void ei_printf(const char *format, ...) {
    va_list myargs;
    va_start(myargs, format);

    char buffer[BUFFER_LEN];
    vsprintf(buffer, format, myargs);

    AudioMoth_appendFile(LOGS_FILE);
    AudioMoth_writeToFile(buffer, BUFFER_LEN);
    AudioMoth_closeFile();  

    va_end(myargs);
}

void ei_printf_float(float f) {
    ei_printf("%f", f);
}

// extern "C" void DebugLog(const char* s) {
//     ei_printf("%s", s);
// }
