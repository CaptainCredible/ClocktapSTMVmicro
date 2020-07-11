//EVEN MOST LATEST VERSION (LOL, thats a stupid thing to put at the top of a file)
/*
  CLOCK TO TAP
  Uses STM32DUINO and arpuss USB library (now included in STM32DUINO CORE), and MIDI library (by Francois Best)
*/
//byte seqCursor = 1;
byte seqACursor = 0;
byte seqBCursor = 0;

#define fullClockLength 48
#define DEBUG 1
#if DEBUG
#define PRINTS(s)   { CompositeSerial.print(F(s)); }
#define PRINT(x)  CompositeSerial.print(x)
#define PRINTLN(x)  CompositeSerial.println(x)

#endif

#define gate1pin PB0 // need to change these?
#define gate2pin PA7 // add transistor on gate out?
#define gateInPin PA9 //
bool asyncHandleStart = false;

#include <U8x8lib.h>
#include <MIDI.h>
#include <USBComposite.h>
#include <U8g2lib.h>
#include <EEPROM.h>


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
const byte bigButtPins[4] = { PA2, PB4,PA15, PA10 };
const byte rotaryClick = PB9;
#define gateA PA3
#define gateB PA0
//const byte gateOuts[4] = { gateA, gateB, gateA, gateB }; // we are only using last two entries, lazy


//VARIABLES
uint16_t mySequences[4] = { 0b1000100010010010, 0b1110101011010110, 0b1111000000000000, 0b1001001001001001 };

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
#define numberOfSettingsValues 15
int oldSettingsValues[15] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
int settingsValues[15] = { 120,0,0,0,0,0,1,1,1,0,0,0,0 };
int settingsRanges[15] = { 667,9,5,4,2,5,8,8,3,4,5,17,17};


char settingsNames[13][15] = {
                         "tempo",       //0
                         "menu cursor", //1
                         "clock in",    //2  0=auto, 1=USB 2=DIN 3=OFF 4=CLK
                         "clock out",   //3  0 = both, 1=USB, 2=DIN
                         "foot mode",   //4  0= tap, 1=resync
                         "tap cursor",  //5  cursor to set inversion inverter
                         "gate 1",      //6  0 = slave to tapout 1 , 5 = always 1/1
                         "gate 2",      //7
                         "gate sel",    //8  select what gate to edit
                         "g in mode",    //9  set clockpulses per BPM //
                         "seqSelex",
                         "seqStepEdit",
                         "seqStepEdit2"
};

#define settingsValueTempo 0
#define settingsValueMenuCursor 1
#define settingsValueClockIn 2
#define settingsValueClockOut 3
#define settingsValueFootMode 4
#define settingsValueTapOutBITS 5
#define settingsValueGate1 6
#define settingsValueGate2 7
#define ClockSettingBoth 0 
#define ClockSettingUSB 1 
#define ClockSettingDIN 2 
#define ClockSettingOFF 3 
#define ClockSettingCLK 4 
#define settingsValueGateSelect 8
#define settingsValueGate1 6
#define settingsValueGate2 7
#define settingsValueGateInMode 9 //0=2ppq  1=4ppq  2=24pRAW 3=48pRAW
#define settingsValueseqSelex 10  //0-3 = sequences 4=edit 5=back
#define settingsValueSeqStepEdit 12

byte page = 0;

#define FOOTMODETAP 0
#define FOOTMODESYNC 1

/*
IO STATUS

EXTCLOCK AUTO        0
EXTCLOCK USB         1
EXCLOCK HW           2
SEND MIDICLOCK USB   3
SEND MIDICLOCK DIN   4

*/

//#define EXTCLOCKAUTOBIT 0
//#define EXTCLOCKUSBBIT 1
//#define EXTCLOCKDINBIT 2
//#define SENDUSBBIT 3
//#define SENDDINBIT 4

//bool extClockAUto = true;
//bool extUSBclock = false;
//bool extDINclock = false;
//bool sendUSBclock = false;
//bool sendDINclock = false;
bool updateDisplay = false;
byte oldIOSettings = 0;
byte IOSettings = 0b00000010;


int currentSetting = 0;
int encoderCount = 500;
bool asyncCalculateTempo = false;
bool asyncCalculateClockStepTimer = false;
int displayedTempo = 0;

bool enableCLKin = false;
bool enableUSBclockIN;
bool enableDINclockIN;
bool nextSyncIsStart = false;
bool enableUSBclockOUT;
bool enableDINclockOUT;
bool alternate = false;
bool intClock = true;
long int timeOfLastClockMessage = 5000;
bool notReceivedClockSinceBoot = true;
bool littleButtStates[4] = { true, true, true, true };
bool oldLittleButtStates[4] = { false, false, false, false };
bool bigButtStates[5] = { true, true, true, true, true }; // last one is tapIn
bool oldBigButtStates[5] = { false, false, false, false, false };
unsigned long lastTimeOfTap = 0;
unsigned long timeOfTap = 0;
unsigned long clockStepTimer = 0;
bool flippedTrips[5] = { false, false, false, false, false }; //last one is actualy override MIDI CLOCK
unsigned long tripTimer[5] = { 0, 0, 0, 0, 0 }; // last on used for taptempo hold
bool triplets[4] = { false, false, false, false };
//unsigned int bigButtTimer[4] = { 0,0,0,0 };
byte waitLedSelect = 0;
unsigned long waitTimer = 0;
//int tapTimer = 0;
unsigned long tapTimer = 0;
unsigned long tempoDivisor = 0;
unsigned long intClockTimer = 0;
int tock = 0;
bool overrideMidiClock = false;
unsigned long oldExtClockTimer;
unsigned long extClockTimer;
//int gtcnt = 0;
unsigned long delta = 0;
unsigned long oldMillis = 0;
unsigned int clockStepTimerDenominator = 6;
unsigned int tempoNumerator = 30000000;


void MIDIClockTick() {
    if (enableDINclockIN) {
        if (!overrideMidiClock) {
            intClock = false;
            notReceivedClockSinceBoot = false;
            clockTick();
        }
        timeOfLastClockMessage = millis();
    }
    
}

void MIDIStop() {
    if (enableDINclockIN) {
        if (!overrideMidiClock) {
            notReceivedClockSinceBoot = false;
            handleStop();
        }
    }
    
    timeOfLastClockMessage = millis();
}

void MIDIStart() {
    if (enableDINclockIN) {
        if (!overrideMidiClock) {
            intClock = false;
            notReceivedClockSinceBoot = false;
            handleStart();
        }

        timeOfLastClockMessage = millis();
    }
    
}

void uMIDIClockTick() {
    if (enableUSBclockIN) {
        if (!overrideMidiClock) {
            intClock = false;
            notReceivedClockSinceBoot = false;
            clockTick();
        }
        timeOfLastClockMessage = millis();
    }
}

void uMIDIStop() {
    if (enableUSBclockIN) {
        if (!overrideMidiClock) {
            notReceivedClockSinceBoot = false;
            handleStop();
        }
        timeOfLastClockMessage = millis();
    }
    
}

void uMIDIStart() {
    if (enableUSBclockIN) {
        if (!overrideMidiClock) {
            intClock = false;
            notReceivedClockSinceBoot = false;
            handleStart();
        }
        timeOfLastClockMessage = millis();
    }
    
}

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, HWMIDI);

class myMidi : public USBMIDI {

    virtual void handleNoteOn() {
        //HWMIDI.sendNoteOn(70,127,1);
    }
    virtual void handleSync() {
        uMIDIClockTick();
    }
    virtual void handleStart() {
        uMIDIStart();
    }
    virtual void handleContinue() {
        uMIDIStart();
    }

    virtual void handleStop() {
        uMIDIStop();
    }
    //virtual void handleReset(void);
};




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
        if (settingsValues[currentSetting] < 0) {
            settingsValues[currentSetting] = settingsRanges[currentSetting] - 1;
        }
        break;
    }
    if (intClock) {
        handleIntClock();
    }
}


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

unsigned long bigDebounceTimers[5] = { 0, 0, 0, 0, 0 };
unsigned long smallDebounceTimers[4] = { 0, 0, 0, 0 };
bool bigDebounceReady[5] = { true, true, true, true, true };
bool smallDebounceReady[4] = { true, true, true, true };
const int debounceThresh = 10;
bool inversion[5] = { false, false, false, false, false };
bool click = false;

myMidi umidi;

USBCompositeSerial CompositeSerial;


void setup() {

    //SCRIM
    u8g2.begin();
    u8g2.setFont(u8g2_font_crox5h_tf);
    u8g2.drawStr(13, 40, "clockTAP");
    u8g2.setFont(u8g2_font_DigitalDisco_tf);
    u8g2.drawStr(70, 54, "v0.1");
    u8g2.sendBuffer();

    //ROTARY ENCODER
    pinMode(ENC_CLK, INPUT_PULLUP);
    pinMode(ENC_DATA, INPUT_PULLUP);
    pinMode(rotaryClick,INPUT_PULLUP);
    encoderCount = 100;
    

    //GATE PINS
    pinMode(gate1pin, OUTPUT);
    pinMode(gate2pin, OUTPUT);
    pinMode(gateInPin, INPUT);

    
    //DIN MIDI
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
        pinMode(bigButtPins[i], INPUT_PULLUP);
        //pinMode(gateOuts[i], OUTPUT);
        digitalWrite(LEDs[i], HIGH);
        if (i > 0) {
            digitalWrite(LEDs[i - 1], LOW);
        }
        delay(200);
    }
    pinMode(tapIn, INPUT_PULLUP);
    delay(100);
    allLedsOff();

    //load settings
    load();
    attachInterrupt(digitalPinToInterrupt(gateInPin), gateInTrig, RISING);
    systick_attach_callback(&mySystick); // attach encoder_read to the systick interrupt

        //USB
    USBComposite.setProductId(0x0030);
    USBComposite.setProductString("clockTap");
    umidi.registerComponent();

#//if DEBUG
    CompositeSerial.registerComponent();
#//endif // DEBUG

    
    USBComposite.begin();
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

unsigned long diff = 0;
unsigned long cnt = 1;
bool timeToHandleClockTick = false;
void handleIntClock() {
    if (tapTimer > 0) {
        unsigned long NOW = micros();
        if (NOW - intClockTimer > clockStepTimer) {
            diff = ((NOW - intClockTimer) - clockStepTimer);
            //intClockTimer = micros();
            cnt++;
            if (cnt % 10 == 0) {
            }
            
            intClockTimer = micros() - diff;  //compensate for catching it late!
            clockTick();
            
        }
    }
}


/*void handleIntClock() {
    if (tapTimer > 0) {
        unsigned long NOW = millis();
        if (NOW - intClockTimer > clockStepTimer) {
            byte diff = (NOW - intClockTimer - clockStepTimer);
            clockTick();
            intClockTimer = millis()-diff;  //compensate for catching it late!
        }
    }
}
*/

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

        // average them
        unsigned int periodsSum = 0;
        for (int i = 0; i < 4; i++) {
            periodsSum += oldExtClockPeriods[i];
        }
        tapTimer = periodsSum >> 2; // divide by four  for averaged resultaveraging
        tempoNumerator = 30000;
        tempoDivisor = tapTimer;
        asyncCalculateTempo = true;

        //settingsValues[settingsValueTempo] = 30000 / tapTimer;
        //CompositeSerial.print("x");
    }
    handleTapOut();
}



void sendMidiClockTick() {

    
    if (!nextSyncIsStart) {
        if (enableDINclockOUT) {
            HWMIDI.sendRealTime(midi::Clock);
        }
        if (enableUSBclockOUT) {
            umidi.sendSync();
        }
    } else {
        sendMidiStart();
        nextSyncIsStart = false;
    }
    
    
}

void sendMidiStart() {
    if (enableDINclockOUT) {
        HWMIDI.sendRealTime(midi::Start);
    }
    if (enableUSBclockOUT) {
        umidi.sendStart();
    }
}

void sendMidiStop() {
    if (enableDINclockOUT) {
        HWMIDI.sendRealTime(midi::Stop);
    }
    if (enableUSBclockOUT) {
        umidi.sendStop();
    }
}

void handleStop() {
    isRunning = false;
    allLedsOff();
}

void handleStart() {  //sets up clockTimers and stuff   RESETS SHIT
    notReceivedClockSinceBoot = false;
    isRunning = true;
    clockIncrement = 47;
    seqACursor = 0;
    seqBCursor = 0;
    sendMidiStop();
    sendMidiStart();
    //nextSyncIsStart = true;
}

byte tapBlinkLength = 2;
int localTapTimer[4] = { 0, 0, 0, 0 };


int aliveCounter = 0;

int highestDiff = 0;


//bool sendDelta = false;

unsigned int loopCnt = 0;
unsigned int loopCntCnt = 0;
unsigned long lastRIGHTNOW = 0;
void loop() {
    loopCnt++;
    if (loopCnt == 1000) {
        //settingsValues[12] = 123;
        unsigned long RIGHTNOW = millis();
        unsigned int differoonie = RIGHTNOW - lastRIGHTNOW;
        lastRIGHTNOW = RIGHTNOW;
        loopCnt = 0;
        for (byte i = 0; i < 15; i++) {
            CompositeSerial.print(settingsValues[i]);
            CompositeSerial.print(" ");
        }
        CompositeSerial.println(currentSetting);
        loopCntCnt++;
        //PRINTS(loopCntCnt);
    }
    
    if (asyncHandleStart) {  //lets us send start messages and shit outside the interrupt
    handleStart();
    asyncHandleStart = false;
    }
    if (asyncCalculateClockStepTimer) {
        clockStepTimer = delta / clockStepTimerDenominator;
        //CompositeSerial.println("pingo");
    }
    if (asyncCalculateTempo) {  // make timer to only let this happen 4 times per sec ?
        settingsValues[settingsValueTempo] = tempoNumerator / tempoDivisor;
    }

    HWMIDI.read();
    umidi.poll();
    handleButts();
    handleRotaryEncoder();
    //handleBlinks();    
    handlePage();

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


    if (bigSmall) {  //if its a big button

        if (millis() - bigDebounceTimers[number] > debounceThresh) {
            bigDebounceReady[number] = true;

        }
    }
    else {
        if (millis() - smallDebounceTimers[number] > debounceThresh) {
            smallDebounceReady[number] = true;
 
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

            }
            else {
                blinkCounter[i]--;
                blinkTimers[i] = millis();
            }
        }
    }
}

void saveData() { //this is what is called when save is clicked
    //redirects to save page, save page calls "save" function
    page = 123;    //do the stuff on save page instead
}

void save() {
    //save settingsValues
    byte myAddress = 0;
    for (int i = 0; i < numberOfSettingsValues; i++) {
        EEPROM.write(myAddress + i, settingsValues[i]);
    }

    

    //save inversions
    myAddress = 20;
    for (int i = 0; i < 5; i++) {
        EEPROM.write(myAddress + i, inversion[i]);
    }

    //save subDivs
    myAddress = 30;
    for (int i = 0; i < 4; i++) {
        EEPROM.write(myAddress + i, clockDivisors[i]);
    }

    //save triplets
    myAddress = 35;
    for (int i = 0; i < 4; i++) {
        EEPROM.write(myAddress + i, triplets[i]);
    }

    //save sequences
    myAddress = 40;
    for (int i = 0; i < 4; i++) {
        EEPROM.write(myAddress + i, triplets[i]);
    }

    //flag settings a stored
    EEPROM.write(100, 123);

}

void load() {
    if (EEPROM.read(100) == 123) {//check if anything to load:
        //load settingsvalues
        byte myAddress = 0;
        for (int i = 0; i < 10; i++) {
            settingsValues[i] = EEPROM.read(myAddress + i);
        }

        //load inversions
        myAddress = 20;
        for (int i = 0; i < 5; i++) {
            inversion[i] = EEPROM.read(myAddress + i);
        }

        //load subDivs
        myAddress = 30;
        for (int i = 0; i < 4; i++) {
            clockDivisors[i] = EEPROM.read(myAddress + i);
        }

        //load triplets
        myAddress = 35;
        for (int i = 0; i < 4; i++) {
            triplets[i] = EEPROM.read(myAddress + i);
        }
    }

    applyIOsettings();  // some booleans need to be set based on clock input / output settings
    for (int i = 0; i < 4; i++) {
        setClockLengths(i);
    }
    
                        // ALSO NEEDS TO APPLY TRIPLETS AND TIMESIGS!!!!
}

