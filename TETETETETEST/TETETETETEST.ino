//LATEST VERSION (LOL, thats a stupid thing to put at the top of a file)
/*
  CLOCK TO TAP
  Uses STM32DUINO and arpuss USBMIDI library, oh and MIDI library (by Francois Best)
*/


//#include <ButtonDebounce.h>

#include <MIDI.h>
#include <USBComposite.h>

//#include <USBMIDI.h>

#define tapIn PB10
byte clockLengths[4] = { 24, 24, 24, 24 }; //24 = 4/4   16 = triplets
unsigned long clockIncrement = 0;
byte clockDivisors[4] = { 1, 1, 1, 1 };
bool taps[4] = { false, false, false, false };
bool isRunning = false;
bool clockUpdated = false;
bool intClock = true;
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
const byte littleButtPins[4] = { PB9, PA2, PA6, PB0 };
const byte bigButtPins[4] = { PB8,  PB7,PB6, PA10 };
#define gateA PA3
#define gateB PA0
const byte gateOuts[4] = { gateA, gateB, gateA, gateB }; // we are only using last two entries, lazy
byte waitLedSelect = 0;
unsigned long waitTimer = 0;
//int tapTimer = 0;
unsigned long tapTimer = 0;
unsigned long intClockTimer = 0;
int tock = 0;
bool overrideMidiClock = false;

void clockTick() {

    clockIncrement++;
    handleTaps();
}

void handleStop() {
    //Serial.println("STOP RESET");
    isRunning = false;
    allLedsOff();
}

void handleStart() {
    notReceivedClockSinceBoot = false;
    isRunning = true;
    clockIncrement = 0;
    handleTaps();
}

void MIDIClockTick() {
    if (!overrideMidiClock) {
        intClock = false;
        notReceivedClockSinceBoot = false;
        clockTick();
    }

}




void MIDIStop() {
    if (!overrideMidiClock) {
        notReceivedClockSinceBoot = false;
        handleStop();
    }
}

void MIDIStart() {
    if (!overrideMidiClock) {
        intClock = false;
        notReceivedClockSinceBoot = false;
        handleStart();
    }
}

class myMidi : public USBMIDI {
    //MIDI.setHandleClock(MIDIClockTick);
    //MIDI.setHandleStart(MIDIStart);
    //MIDI.setHandleStop(handleStop);

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


MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI);




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
    pinMode(ledA, OUTPUT);
    
    USBComposite.setProductId(0x0030);
    USBComposite.setProductString("clockTap");
    umidi.registerComponent();
    CompositeSerial.registerComponent();
    USBComposite.begin();
    handleStart();
    

    MIDI.setHandleClock(MIDIClockTick);
    MIDI.setHandleStart(MIDIStart);
    MIDI.setHandleStop(handleStop);
    MIDI.begin(MIDI_CHANNEL_OMNI); // Initiate MIDI communications, listen to all channels

    for (int i = 0; i < 4; i++) {
        pinMode(outs[i], OUTPUT);
        pinMode(LEDs[i], OUTPUT);
        pinMode(littleButtPins[i], INPUT_PULLUP);
        pinMode(bigButtPins[i], INPUT_PULLUP);
        pinMode(gateOuts[i], OUTPUT);
        digitalWrite(LEDs[i], HIGH);
        if (i > 0) {
            digitalWrite(LEDs[i - 1], LOW);
        }
        delay(100);
    }
    delay(500);
    allLedsOff();
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
        //if (millis() - intClockTimer > tapTimer>>3) {
        if (millis() - intClockTimer > clockStepTimer) {
            //tock++;
            ////Serial.println(tock);
            clockTick();
            intClockTimer = millis();
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





byte tapBlinkLength = 3;
int localTapTimer[4] = { 0, 0, 0, 0 };
void handleTaps() {
    for (int i = 0; i < 4; i++) {

        localTapTimer[i] = clockIncrement % clockLengths[i];
        //  //Serial.println(taps[i]);


        if (localTapTimer[i] == 0) {
            taps[i] = true;
            //CompositeSerial.print("TAP! ");
            //CompositeSerial.println(i);
            digitalWrite(outs[i], !inversion[i]);
            if (i > 1) {
                digitalWrite(gateOuts[i], HIGH);
            }
            if (!bigButtStates[i]) {
                digitalWrite(LEDs[i], !inversion[i]);
            }
        }
        else if (localTapTimer[i] == tapBlinkLength) {
            taps[i] = false;
            digitalWrite(outs[i], inversion[i]);
            if (i > 1) {
                digitalWrite(gateOuts[i], LOW);
            }
            if (!bigButtStates[i]) {
                digitalWrite(LEDs[i], inversion[i]);
            }
        }
    }
    delay(1000);
   // CompositeSerial.println("OK!");
   // CompositeSerial.flush();
}


int aliveCounter = 0;
void loop() {
    aliveCounter++;
    if (notReceivedClockSinceBoot) {
        //waiting4clock();
    }

    if (intClock) {
        handleIntClock();
    }

    MIDI.read();
    umidi.poll();


    handleButts();
    handleBlinks();
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






