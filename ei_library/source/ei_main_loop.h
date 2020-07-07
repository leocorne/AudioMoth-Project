// Header file for ei_main_loop. Only contains declaration of main EI method

#ifdef __cplusplus
extern "C" {
#endif

float ei_classify(int16_t* raw_features, int raw_features_size, int signal_size, float* signal_start_address);

#ifdef __cplusplus
}
#endif