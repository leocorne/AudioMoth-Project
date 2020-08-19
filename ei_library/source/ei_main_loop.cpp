// #include <stdio.h>
// #include <iostream>
// #include <sstream>
#include <string>
#include "ei_run_classifier.h"
#include "ei_classifier_porting.h"
#include "audioMoth.h"

// Depending on how the EI model was trained, the "interesting word" might be at index 0 or 1
// We keep track of this with this variable
#define INTERESTING_SOUND_INDEX              1

// This must be a constant as we can't instantiate an array with variable size.
// TODO: Find a way to make this variable so it can work with any recording length.
// Alternative: make this very long and ignore the last values if not needed (but wastes space)
#define RECORDING_LENGTH_IN_BUFFERS          10                   

float prob_array[RECORDING_LENGTH_IN_BUFFERS];
int prob_index;

extern "C" void makeProbArray(){
    prob_index = 0;
    //prob_array = {};
}

extern "C" void printProbArray(){
    ei_printf_force("[");
    for (size_t ix = 0; ix < prob_index; ix++) {
        ei_printf_force_float(prob_array[ix]);

        if (ix != prob_index - 1) ei_printf_force(", ");
    }
    ei_printf_force("]\n");
}

extern "C" float * getProbArray(){
    return prob_array;
}

extern "C" float ei_classify(int16_t* raw_features, int raw_features_size, int signal_size, float* signal_start_address){

    // Convert raw inputs to float as this is what the Edge Impulse model takes in.
    // TODO: Edit EI code so it will take int16_t 

    float *float_features = signal_start_address;

    for(int i = 0; i < signal_size; ++i){
        float_features[i] = (float)raw_features[i];
    }

    //ei_printf_force("Generated float_features \n");

    ei_impulse_result_t result;
    signal_t signal;
    
    numpy::signal_from_buffer(&float_features[0], signal_size, &signal);
    //ei_printf_force("Generated signal \n");

    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

    //ei_printf_force("run_classifier returned: %d\n", res);

    //ei_printf_force("Begin output\n");

    // print the predictions
    //ei_printf_force("[");
    //for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        //ei_printf_force_float(result.classification[ix].value);

        //if (EI_CLASSIFIER_HAS_ANOMALY == 1) ei_printf_force(", ");
        //else if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) ei_printf_force(", ");
    //}

    //if (EI_CLASSIFIER_HAS_ANOMALY == 1) ei_printf_force_float(result.anomaly);

    //ei_printf_force("]\n");

    //ei_printf_force("End output\n");

    // Finally return the probability of having spotted the wanted sound to the AudioMoth system

    float detection_prob = result.classification[INTERESTING_SOUND_INDEX].value;

    prob_array[prob_index++] = detection_prob;
    return detection_prob;
}
