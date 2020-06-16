/****************************************************************************
 * main.c
 * openacousticdevices.info
 * June 2017
 *****************************************************************************/

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "audioMoth.h"
#include "one.h"

#include "tensorflow/lite/experimental/micro/examples/micro_speech/test_interface.h"
#include "tensorflow/lite/experimental/micro/examples/micro_speech/audio_provider.h"

/* Logs file */
#define STARTUP_MESSAGE                     "Loop started\n"

/* Useful time constants */

#define SECONDS_IN_MINUTE                   60
#define SECONDS_IN_HOUR                     (60 * SECONDS_IN_MINUTE)
#define SECONDS_IN_DAY                      (24 * SECONDS_IN_HOUR)

/* Useful type constants */

#define BITS_PER_BYTE                       8
#define UINT32_SIZE_IN_BITS                 32
#define UINT32_SIZE_IN_BYTES                4

/* USB configuration constant */

#define MAX_START_STOP_PERIODS              5

/* USB configuration data structure */

#pragma pack(push, 1)

typedef struct {
    uint16_t startMinutes;
    uint16_t stopMinutes;
} startStopPeriod_t;

typedef struct {
    uint32_t time;
    uint8_t gain;
    uint8_t clockDivider;
    uint8_t acquisitionCycles;
    uint8_t oversampleRate;
    uint32_t sampleRate;
    uint8_t sampleRateDivider;
    uint16_t sleepDuration;
    uint16_t recordDuration;
    uint8_t enableLED;
    uint8_t activeStartStopPeriods;
    startStopPeriod_t startStopPeriods[MAX_START_STOP_PERIODS];
    int8_t timezoneHours;
    uint8_t enableBatteryCheck;
    uint8_t disableBatteryLevelDisplay;
    int8_t timezoneMinutes;
} configSettings_t;

#pragma pack(pop)

static const configSettings_t defaultConfigSettings = {
    .time = 0,
    .gain = 2,
    .clockDivider = 4,
    .acquisitionCycles = 16,
    .oversampleRate = 1,
    .sampleRate = 384000,
    .sampleRateDivider = 8,
    .sleepDuration = 0,
    .recordDuration = 60,
    .enableLED = 1,
    .activeStartStopPeriods = 0,
    .startStopPeriods = {
        {.startMinutes = 60, .stopMinutes = 120},
        {.startMinutes = 300, .stopMinutes = 420},
        {.startMinutes = 540, .stopMinutes = 600},
        {.startMinutes = 720, .stopMinutes = 780},
        {.startMinutes = 900, .stopMinutes = 960}
    },
    .timezoneHours = 0,
    .enableBatteryCheck = 0,
    .disableBatteryLevelDisplay = 0,
    .timezoneMinutes = 0
};

static configSettings_t *configSettings = (configSettings_t*)(AM_BACKUP_DOMAIN_START_ADDRESS + 12);

/* Recording state */

static volatile bool switchPositionChanged;

/* Current recording file name */

static char fileName[20];

/* Firmware version and description */

static uint8_t firmwareVersion[AM_FIRMWARE_VERSION_LENGTH] = {1, 3, 0};

static uint8_t firmwareDescription[AM_FIRMWARE_DESCRIPTION_LENGTH] = "AudioMoth-Firmware-Basic";

/* Functions of copy to and from the backup domain */

static void copyFromBackupDomain(uint8_t *dst, uint32_t *src, uint32_t length) {

    for (uint32_t i = 0; i < length; i += 1) {
        *(dst + i) = *((uint8_t*)src + i);
    }

}

static void copyToBackupDomain(uint32_t *dst, uint8_t *src, uint32_t length) {

    uint32_t value = 0;

    for (uint32_t i = 0; i < length / 4; i += 1) {
        *(dst + i) = *((uint32_t*)src + i);
    }

    for (uint32_t i = 0; i < length % 4; i += 1) {
        value = (value << 8) + *(src + length - 1 - i);
    }

    *(dst + length / 4) = value;

}

static void logMsg(char * msg){
    AudioMoth_appendFile(LOGS_FILE);
    AudioMoth_writeToFile(msg, strlen(msg));
    AudioMoth_closeFile(); 
}

static void initialiseLogFile(){
    AudioMoth_enableFileSystem();
    logMsg(STARTUP_MESSAGE);
}

/* Main function */
int main(void) {

    /* Initialise device */

    AudioMoth_initialise();

    /* Check the switch position */

    AM_switchPosition_t switchPosition = AudioMoth_getSwitchPosition();

    // if (AudioMoth_isInitialPowerUp()) {
    // }

    if (switchPosition == AM_SWITCH_USB) {

        /* Handle the case that the switch is in USB position. Waits in low energy state until USB disconnected or switch moved  */

        AudioMoth_handleUSB();

    } 
    
    else {

        /* Set up log file */
        initialiseLogFile();

        /* Check the Simplicity Studio has been configured properly */
        if(one() == 1){logMsg("C++ compiler working \n");}
        else{logMsg("C compiler only \n");}

        uint32_t startTime;
        uint16_t startMillis;
        AudioMoth_getTime(&startTime, &startMillis);
        
        /* Call setup function from tflite */
        mainfun();

        /* Calculate duration of loop */
        uint32_t endTime;
        uint16_t endMillis;
        AudioMoth_getTime(&endTime, &endMillis);
        uint32_t duration = 1000 * (endTime - startTime) + endMillis - startMillis;
        char duration_message[20];
        sprintf(duration_message, "Loop took %d \n", duration);
        logMsg(duration_message);

        /* Flash LEDs to indicate we are done */
        AudioMoth_setBothLED(true);
        AudioMoth_delay(100);
        AudioMoth_setBothLED(false);
    }

    /* Power down and wake up in one second */

    AudioMoth_powerDownAndWake(1, true);

}

/* Time zone handler */

inline void AudioMoth_timezoneRequested(int8_t *timezoneHours, int8_t *timezoneMinutes) {

    *timezoneHours = configSettings->timezoneHours;

    *timezoneMinutes = configSettings->timezoneMinutes;

}


/* AudioMoth interrupt handlers */
inline void AudioMoth_handleSwitchInterrupt() {

    switchPositionChanged = true;
    
}

#define MICROPHONE_BUFFER_SIZE          512
int16_t microphone_buffer[MICROPHONE_BUFFER_SIZE];
int sample_index = 0;
//char samplesMsg[50];

inline void AudioMoth_handleMicrophoneInterrupt(int16_t sample) { 
    
    microphone_buffer[sample_index] = sample;
    sample_index++;

    if (sample_index == MICROPHONE_BUFFER_SIZE){

        //sprintf(samplesMsg, "Captured %d samples \n",sample_index);
        //logMsg(samplesMsg);
        sample_index = 0;

        // Give samples to TFLite Audio Provider
        CaptureSamples(microphone_buffer);
    }
}

inline void AudioMoth_handleDirectMemoryAccessInterrupt(bool isPrimaryBuffer, int16_t **nextBuffer) {}


/* AudioMoth USB message handlers */

inline void AudioMoth_usbFirmwareVersionRequested(uint8_t **firmwareVersionPtr) {

    *firmwareVersionPtr = firmwareVersion;

}

inline void AudioMoth_usbFirmwareDescriptionRequested(uint8_t **firmwareDescriptionPtr) {

    *firmwareDescriptionPtr = firmwareDescription;

}

inline void AudioMoth_usbApplicationPacketRequested(uint32_t messageType, uint8_t *transmitBuffer, uint32_t size) {

    /* Copy the current time to the USB packet */

    uint32_t currentTime;

    AudioMoth_getTime(&currentTime, NULL);

    memcpy(transmitBuffer + 1, &currentTime, 4);

    /* Copy the unique ID to the USB packet */

    memcpy(transmitBuffer + 5, (uint8_t*)AM_UNIQUE_ID_START_ADDRESS, AM_UNIQUE_ID_SIZE_IN_BYTES);

    /* Copy the battery state to the USB packet */

    AM_batteryState_t batteryState = AudioMoth_getBatteryState();

    memcpy(transmitBuffer + 5 + AM_UNIQUE_ID_SIZE_IN_BYTES, &batteryState, 1);

    /* Copy the firmware version to the USB packet */

    memcpy(transmitBuffer + 6 + AM_UNIQUE_ID_SIZE_IN_BYTES, firmwareVersion, AM_FIRMWARE_VERSION_LENGTH);

    /* Copy the firmware description to the USB packet */

    memcpy(transmitBuffer + 6 + AM_UNIQUE_ID_SIZE_IN_BYTES + AM_FIRMWARE_VERSION_LENGTH, firmwareDescription, AM_FIRMWARE_DESCRIPTION_LENGTH);

}

inline void AudioMoth_usbApplicationPacketReceived(uint32_t messageType, uint8_t* receiveBuffer, uint8_t *transmitBuffer, uint32_t size) {

    /* Copy the USB packet contents to the back-up register data structure location */

    copyToBackupDomain((uint32_t*)configSettings,  receiveBuffer + 1, sizeof(configSettings_t));

    /* Copy the back-up register data structure to the USB packet */

    copyFromBackupDomain(transmitBuffer + 1, (uint32_t*)configSettings, sizeof(configSettings_t));

    /* Set the time */

    AudioMoth_setTime(configSettings->time, 0);

}

void AudioMoth_enableMicrophoneDefaultSettings(){
    AudioMoth_enableMicrophone(configSettings->gain, configSettings->clockDivider, configSettings->acquisitionCycles, configSettings->oversampleRate);
}


