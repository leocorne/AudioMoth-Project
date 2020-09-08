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


### Code ###
Most of the folders here are simply embedded libraries we are using.

The src folder has the AudioMoth-specific code, including the main recording loop and several AudioMoth functions.

The ei_library folder has all of the Edge Impulse code. Within this, the model-parameters folder has the parameters for our specific EI model. The tflite-model folder holds the weights of the TensorFlow Lite neural network. The source folder has the source code for the code developed to interact with the EI SDK. The edge-impulse-sdk folder has various libraries needed to run our model, such as DSP libraries, CMSIS code and the TFLite source code. The porting_audiomoth folder within this has AudioMoth-specific implementations of some generic EI methods, such as logging and getting the time. 

The GNU ARM v4.9.3 - Release folder contains the binary files that allow us to deploy the code to AudioMoth. Specifically, AudioMoth-Project.bin can be deployed on AudioMoth through the AudioMoth Flash App. This pre-generated binary is running Conv1D_S on the coquí dataset, so it will save coquí recordings if it hears any (to test this, you can find several coquí recordings on YouTube or freesound.org)