# AudioMoth-Project #
A modified version of the AudioMoth standard firmware. This software performs live classification of samples using a neural network and saves audio clips containing a specific sound. The neural network here is trained to detect [coquí frog](https://freesound.org/people/tombenedict/sounds/397998/) sounds, but it can be replaced with a NN trained to detect any sound.

### Usage ###
Clone this project and compile it similarly to the example [AudioMoth project](https://github.com/OpenAcousticDevices/AudioMoth-Project). 

To change the sound the NN detects, train a different neural network on the [Edge Impulse platform](https://edgeimpulse.com/). Go to the "Deployment" tab, build the C++ library, and download the ZIP file. 

Then replace the following files on the ei_library folder by the files from the ZIP file generated:
- model-parameters/dsp_blocks.h
- model-parameters/model_metadata.h
- tflite-model/tflite-trained.h

Compile the project on Simplicity Studio and upload to AudioMoth.

### Documentation ###

The software works similarly to the [standard AudioMoth firmware](https://github.com/OpenAcousticDevices/AudioMoth-Firmware-Basic/) except for the live classification. See the AudioMoth wiki for details.

### License ###

Copyright 2017 [Open Acoustic Devices](http://www.openacousticdevices.info/).

[MIT license](http://www.openacousticdevices.info/license).
