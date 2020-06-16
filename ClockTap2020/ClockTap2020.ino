//EVEN MOST LATEST VERSION (LOL, thats a stupid thing to put at the top of a file)
/*
  CLOCK TO TAP
  Uses STM32DUINO and arpuss USB library (now included in STM32DUINO CORE), and MIDI library (by Francois Best)
*/


//#include <ButtonDebounce.h>

#include <U8x8lib.h>
#include <MIDI.h>
#include <USBComposite.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//PINS

#define ENC_CLK PB8
#define ENC_DATA PB5
#define BUTTON  PB9
#define tapIn PA8
//const byte littleButtPins[4] = { PB9, PA2, PA6, PB0 };
//const byte bigButtPins[4] = { PB8, PB7,PB6, PA10 };
const byte bigButtPins[4] = { PA2, PB4,PA15, PA10 };
const byte rotaryClick = PB5;
#define gateA PA3
#define gateB PA0
const byte gateOuts[4] = { gateA, gateB, gateA, gateB }; // we are only using last two entries, lazy


//VARIABLES


char subDivStrings[6][6] = {
                         "1",
                         "2",
                         "4",
                         "8",
                         "16",
                         "32"
};

char tripSubDivStrings[6][6] = {
                         "1t",
                         "2t",
                         "4t",
                         "8t",
                         "16t",
                         "32t"
};

char settingsNames[10][10] = {
                         "tempo",
                         "midi",
                         "unused",
                         "unused",
                         "unused",
                         "unused",
                         "unused",
                         "unused",
                         "unused",
                         "unused"
};

bool sendHWclock = true;
bool sendUSBclock = true;
int settingsValues[10] = { 120,0,0,0,0,0,0,0,0,0 };
int oldSettingsValues[10] = { 120,0,0,0,0,0,0,0,0,0 };
int settingsRanges[10] = { 666,4,0,0,0,0,0,0,0,0 };
int currentSetting = 0;
int encoderCount = 500;
#define tempo 0
//int tempo = 120;
int displayedTempo = 0;
bool intClock = true;
long int timeSinceLastMidiMessage = 5000;
bool notReceivedClockSinceBoot = true;
bool littleButtStates[4] = { true, true, true, true };
bool oldLittleButtStates[4] = { false, false, false, false };
bool bigButtStates[5] = { true, true, true, true, true }; // last one is tapIn
bool oldBigButtStates[5] = { false, false, false, false, false };
unsigned long lastTimeOfTap = 0;
unsigned long timeOfTap = 0;
int clockStepTimer = 0;
bool flippedTrips[5] = { false, false, false, false, false }; //last one is actualy override MIDI CLOCK
unsigned long tripTimer[5] = { 0, 0, 0, 0, 0 }; // last on used for taptempo hold
bool triplets[4] = { false, false, false, false };
//unsigned int bigButtTimer[4] = { 0,0,0,0 };
byte waitLedSelect = 0;
unsigned long waitTimer = 0;
//int tapTimer = 0;
unsigned long tapTimer = 0;
unsigned long intClockTimer = 0;
int tock = 0;
bool overrideMidiClock = false;
unsigned long oldExtClockTimer;
unsigned long extClockTimer;



void MIDIClockTick() {
    if (!overrideMidiClock) {
        intClock = false;
        notReceivedClockSinceBoot = false;
        clockTick();
    }
    timeSinceLastMidiMessage = millis();
}

void MIDIStop() {
    if (!overrideMidiClock) {
        notReceivedClockSinceBoot = false;
        handleStop();
    }
    timeSinceLastMidiMessage = millis();
}

void MIDIStart() {
    if (!overrideMidiClock) {
        intClock = false;
        notReceivedClockSinceBoot = false;
        handleStart();
    }
    timeSinceLastMidiMessage = millis();
}

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, HWMIDI);

class myMidi : public USBMIDI {

    virtual void handleNoteOn() {
        //HWMIDI.sendNoteOn(70,127,1);
    }
    virtual void handleSync() {
        MIDIClockTick();
    }
    virtual void handleStart() {
        MIDIStart();
    }
    virtual void handleContinue() {
        MIDIStart();
    }

    virtual void handleStop() {
        MIDIStop();
    }
    //virtual void handleReset(void);
};

myMidi umidi;
USBCompositeSerial CompositeSerial;



//virtual void handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity);
//virtual void handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);
//virtual void handleVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity);
//virtual void handleControlChange(unsigned int channel, unsigned int controller, unsigned int value);
//virtual void handleProgramChange(unsigned int channel, unsigned int program);
//virtual void handleAfterTouch(unsigned int channel, unsigned int velocity);
//virtual void handlePitchChange(unsigned int pitch);
//virtual void handleSongPosition(unsigned int position);
//virtual void handleSongSelect(unsigned int song);
//virtual void handleTuneRequest(void);
//virtual void handleSync(void);
//virtual void handleStart(void);
//virtual void handleContinue(void);
//virtual void handleStop(void);
//virtual void handleActiveSense(void);
//virtual void handleReset(void);

// TO BE CALLED FROM SYSTICK INTERRUPT:
void mySystick(void)
{
    volatile static uint8_t ABs = 0;
    ABs = (ABs << 2) & 0x0f; //left 2 bits now contain the previous AB key read-out;
    ABs |= (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DATA);
    switch (ABs)
    {
    case 0x0d:
        settingsValues[currentSetting]++;
        settingsValues[currentSetting] = settingsValues[currentSetting] % settingsRanges[currentSetting];
        break;
    case 0x0e:
        settingsValues[currentSetting]--;
        settingsValues[currentSetting] = settingsValues[currentSetting] % settingsRanges[currentSetting];
        break;
    }
    if (intClock) {
        handleIntClock();
    }
}

void checkCompiler() {
    allLedsOn();
}



//these need fixing
#define outA PA1 //A0
#define outB PA4 //15
#define outC PA5 //14
#define outD PB1 //A3
byte outs[4] = { outA, outB, outC, outD };

#define ledA PB15 //D3
#define ledB PB14 //D3
#define ledC PB13 //D3
#define ledD PB12 //D3
byte LEDs[4] = { ledA, ledB, ledC, ledD };

//tapInDebounceTimer = 0;
unsigned long bigDebounceTimers[5] = { 0, 0, 0, 0, 0 };
unsigned long smallDebounceTimers[4] = { 0, 0, 0, 0 };
bool bigDebounceReady[5] = { true, true, true, true, true };
bool smallDebounceReady[4] = { true, true, true, true };
const int debounceThresh = 10;
bool inversion[5] = { false, false, false, false, false };



void setup() {
    //ROTARY ENCODER
    pinMode(ENC_CLK, INPUT_PULLUP);
    pinMode(ENC_DATA, INPUT_PULLUP);
    encoderCount = 100;
    systick_attach_callback(&mySystick); // attach encoder_read to the systick interrupt

    //USB
    USBComposite.setProductId(0x0030);
    USBComposite.setProductString("clockTap");
    umidi.registerComponent();
    CompositeSerial.registerComponent();
    USBComposite.begin();
    HWMIDI.setHandleClock(MIDIClockTick);
    HWMIDI.setHandleStart(MIDIStart);
    HWMIDI.setHandleStop(handleStop);
    HWMIDI.begin(MIDI_CHANNEL_OMNI); // Initiate MIDI communications, listen to all channels
    HWMIDI.turnThruOff();
    
    //TIMEKEEPING
    handleStart(); //sets up clockTimers and stuff
    for (int i = 0; i < 4; i++) {
        pinMode(outs[i], OUTPUT);
        pinMode(LEDs[i], OUTPUT);
        //pinMode(littleButtPins[i], INPUT_PULLUP);
        pinMode(bigButtPins[i], INPUT_PULLUP);
        pinMode(gateOuts[i], OUTPUT);
        digitalWrite(LEDs[i], HIGH);
        if (i > 0) {
            digitalWrite(LEDs[i - 1], LOW);
        }
        delay(100);
    }
    pinMode(tapIn, INPUT_PULLUP);
    delay(100);
    allLedsOff();

    //DISPLAY
    u8g2.begin();
    //u8g2.setPowerSave(0);
    CompositeSerial.println("reddy");
}

void allLedsOn() {
    for (int i = 0; i < 4; i++) {
        digitalWrite(LEDs[i], HIGH);
    }
}

void allLedsOff() {
    for (int i = 0; i < 4; i++) {
        digitalWrite(LEDs[i], LOW);
    }
}

void waiting4clock() {
    if (tapTimer > 0) {
        //intClock = true;
        handleIntClock();
    }
}

void handleIntClock() {
    if (tapTimer > 0) {
        unsigned long NOW = millis();
        if (NOW - intClockTimer > clockStepTimer) {
            byte diff = (NOW - intClockTimer - clockStepTimer);
            clockTick();
            intClockTimer = millis()-diff;  //compensate for catching it late!
        }
    }
}

void lightScroll() {
    if (millis() - waitTimer > 60) {
        waitTimer = millis();
        digitalWrite(LEDs[waitLedSelect], LOW);
        waitLedSelect++;
        waitLedSelect = waitLedSelect % 4;
        digitalWrite(LEDs[waitLedSelect], HIGH);
    }
}

byte clockLengths[4] = { 24, 24, 24, 24 }; //24 = 4/4   16 = triplets
unsigned long clockIncrement = 0;
byte clockDivisors[4] = { 0, 0, 0, 0 };
byte displayedClockDivisors[4] = { 1, 1, 1, 1 };
bool taps[4] = { false, false, false, false };
bool isRunning = false;
bool clockUpdated = false;

unsigned int oldExtClockPeriods[4] = { 500, 500, 500, 500 };
void clockTick() {
    clockIncrement++;
    sendMidiClockTick();
    
    if (!intClock && (clockIncrement%12 == 0)) {
        oldExtClockTimer = extClockTimer;
        extClockTimer = millis();
        //make a list of the previous clock periods
        for (int i = 3; i > 0; i--) {
            oldExtClockPeriods[i] = oldExtClockPeriods[i-1];
        }
        oldExtClockPeriods[0] = extClockTimer - oldExtClockTimer;
        /*
        CompositeSerial.print(oldExtClockPeriods[3]);
        CompositeSerial.print(" ");
        CompositeSerial.print(oldExtClockPeriods[2]);
        CompositeSerial.print(" ");
        CompositeSerial.print(oldExtClockPeriods[1]);
        CompositeSerial.print(" ");
        CompositeSerial.print(oldExtClockPeriods[0]);
        CompositeSerial.println(" ");
        */

        // average them
        unsigned int periodsSum = 0;
        for (int i = 0; i < 4; i++) {
            periodsSum += oldExtClockPeriods[i];
        }
        tapTimer = periodsSum >> 2; // divide by four  for averaged resultaveraging
        settingsValues[tempo] = 30000 / tapTimer;
        
    }
    handleTapOut();
}



void sendMidiClockTick() {
    if (sendHWclock) {
        HWMIDI.sendRealTime(midi::Clock);
    }
    if (sendUSBclock) {
        umidi.sendSync();
    }
    
    
}

void sendMidiStart() {
    HWMIDI.sendRealTime(midi::Start);
    umidi.sendStart();
}

void handleStop() {
    //Serial.println("STOP RESET");
    isRunning = false;
    allLedsOff();
}

void handleStart() {  //sets up clockTimers and stuff
    notReceivedClockSinceBoot = false;
    isRunning = true;
    clockIncrement = 0;
    handleTapOut();
    sendMidiStart();
}

byte tapBlinkLength = 3;
int localTapTimer[4] = { 0, 0, 0, 0 };


int aliveCounter = 0;



void loop() {
    aliveCounter++;
    if (notReceivedClockSinceBoot) {
      //  waiting4clock();
    }
// handle int clock moved to systick!
    HWMIDI.read();
    umidi.poll();
    handleButts();
    handleRotaryEncoder();
    //handleBlinks();
    if (aliveCounter == 10000) {
        //u8g2.clearDisplay();
    }

    handleDisplay();
}




int temp = 0;
void setClockLengths(byte tap) {
    if (!triplets[tap]) {
        clockLengths[tap] = 48 >> clockDivisors[tap];
    }
    else {
        clockLengths[tap] = 32 >> clockDivisors[tap];
    }
}

void debounce(bool bigSmall, byte number) {
    /*//Serial.print("deBouncing ");
      //Serial.print(bigSmall);
      //Serial.print(" - ");
      //Serial.print(number);
      //Serial.print(" - ");
      //Serial.print(smallDebounceTimers[number]);
      //Serial.print(" - ");
      //Serial.println(millis());
    */

    if (bigSmall) {  //if its a big button

        if (millis() - bigDebounceTimers[number] > debounceThresh) {
            bigDebounceReady[number] = true;
            ////Serial.println("big debounce DONE");
        }
    }
    else {
        if (millis() - smallDebounceTimers[number] > debounceThresh) {
            smallDebounceReady[number] = true;
            ////Serial.println("small debounce DONE");
        }
    } 
}

unsigned long blinkTimers[4];
byte blinkCounter[4] = { 0, 0, 0, 0 };

void blink(byte led, byte times) {
    blinkCounter[led] = times + 1;
    blinkTimers[led] = millis();
}

int desiredBlinkPeriod = 100;
//bool onTimerSetButNotReached[4] = { false, false, false, false };
void handleBlinks() {
    for (int i = 0; i < 4; i++) {
        if (blinkCounter[i] > 0) {
            if (millis() - blinkTimers[i] < desiredBlinkPeriod >> 1) { //if we are b4 halfway point
                digitalWrite(LEDs[i], HIGH);
            }
            else if (millis() - blinkTimers[i] < desiredBlinkPeriod) {
                digitalWrite(LEDs[i], LOW);
                //Serial.println(blinkCounter[i]);
            }
            else {
                blinkCounter[i]--;
                blinkTimers[i] = millis();
            }
        }
    }
}