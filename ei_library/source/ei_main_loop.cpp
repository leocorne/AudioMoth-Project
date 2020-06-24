// #include <stdio.h>
// #include <iostream>
// #include <sstream>
#include <string>
#include "ei_run_classifier.h"
#include "ei_classifier_porting.h"
#include "audioMoth.h"

// std::string trim(const std::string& str) {
//     size_t first = str.find_first_not_of(' ');
//     if (std::string::npos == first)
//     {
//         return str;
//     }
//     size_t last = str.find_last_not_of(' ');
//     return str.substr(first, (last - first + 1));
// }

extern "C" void mainloop(){

    ei_printf("Hello world %d, %f, %s", 9, 3.2f, "hiii\n");
    ei_printf_float(23.4345f);

    AudioMoth_enableExternalSRAM();

    float* raw_features = (float*)AM_EXTERNAL_SRAM_START_ADDRESS;


    int signal_size = 16000;


    for (int i =0; i<signal_size; i++){
        raw_features[i] = 0.001f * i;
    }

    ei_printf("Generated raw_features \n");

    ei_impulse_result_t result;

    signal_t signal;
    numpy::signal_from_buffer(&raw_features[0], signal_size, &signal);

    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, true);

    ei_printf("run_classifier returned: %d\n", res);

    ei_printf("Begin output\n");

    // print the predictions
    ei_printf("[");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf_float(result.classification[ix].value);

        if (EI_CLASSIFIER_HAS_ANOMALY == 1) ei_printf(", ");
        else if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) ei_printf(", ");
    }

    if (EI_CLASSIFIER_HAS_ANOMALY == 1) ei_printf_float(result.anomaly);

    ei_printf("]\n");

    ei_printf("End output\n");
}
