
uint16_t mySequences[4] = { 0b1000100010010010, 0b1110101011010110, 0b1111000000000000, 0b1001001001001001 };

//  16 steps
//  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #   
//  ################################################
//  48 clock increments
byte seqCursor = 1;
byte seqTimeSig = 6; //3 is 4/4, 2 is trip make this part of settingsvalues!
bool seqGateState = false;
bool oldSeqGateState = false;
bool aSeqIsRunning = false;

void handleSeqGate() {
    if (settingsValues[settingsValueGate1] == 4 || settingsValues[settingsValueGate2] == 4) {  //SKIP THIS IF NO SEQ IS RUNNING
        aSeqIsRunning = true;
    }
    else {
        aSeqIsRunning = false;
    }

    if (aSeqIsRunning){
        uint16_t currentSeq = mySequences[0];  //make SEQ selectable
        byte mySeqClock = clockIncrement % seqTimeSig;
        if (mySeqClock == 0) {
            if (bitRead(mySequences[0], 15 - seqCursor)) {
                seqGateState = true;
            }
            seqCursor++;
            seqCursor = seqCursor % 16;
        }
        else {
            seqGateState = false;
        }

        if (seqGateState && !oldSeqGateState) {
            oldSeqGateState = true;
            if (settingsValues[settingsValueGate1] == 4) {
                digitalWrite(gate1pin, HIGH);
            }
            if (settingsValues[settingsValueGate2] == 4) {
                digitalWrite(gate2pin, HIGH);
            }


        } else if (oldSeqGateState && !seqGateState){
            oldSeqGateState = false;
            if (settingsValues[settingsValueGate1] == 4) {
                digitalWrite(gate1pin, LOW);
            }
            if (settingsValues[settingsValueGate2] == 4) {
                digitalWrite(gate2pin, LOW);
            }
        }

    }
}

void handleTapOut() {    
    handleSeqGate();
    handleBasicGateOut();
    for (int i = 0; i < 4; i++) {

        localTapTimer[i] = clockIncrement % clockLengths[i];
        
        
        
        if (localTapTimer[i] == 0) {
            taps[i] = true;

            tapOutToGateOuts(true, i);

            

            digitalWrite(outs[i], !inversion[i]);
            /*
            if (i > 1) {
                digitalWrite(gateOuts[i], HIGH);  //OLD GATE OUT CODE CAN FUCK OFF
            }
            */
            if (!bigButtStates[i]) {
                digitalWrite(LEDs[i], !inversion[i]);
            }
        }


        else if (localTapTimer[i] == tapBlinkLength) {
            taps[i] = false;
            
            tapOutToGateOuts(false, i);

            digitalWrite(outs[i], inversion[i]);
            /*if (i > 1) {
                digitalWrite(gateOuts[i], LOW); //OLD GATE OUT CODE
            }*/
            if (!bigButtStates[i]) {
                digitalWrite(LEDs[i], inversion[i]);
            }
        }
    }
}


void tapOutToGateOuts(bool onOff, byte tapNumber) {
    
    if (onOff) {  //if this is gate on
        if (tapNumber < 4) {  //if it is assigned to one of the 4 tapouts
            if (tapNumber == settingsValues[settingsValueGate1]) { //check if gate out 1 is slaved to this tapout
                
                digitalWrite(gate1pin, HIGH);
            }
            if (tapNumber == settingsValues[settingsValueGate2]) { //check if gate out 1 is slaved to this tapout

                digitalWrite(gate2pin, HIGH);
            }
        }
    }
    else {  //if this is gate off
        if (tapNumber == settingsValues[settingsValueGate1]) { //NEED TO CHANGE 0 with variable for what to slave gate out to
            digitalWrite(gate1pin, LOW);
        }
        if (tapNumber == settingsValues[settingsValueGate2]) { //NEED TO CHANGE 0 with variable for what to slave gate out to
            digitalWrite(gate2pin, LOW);
        }
    }
    
    
}

bool basicGateOutBool = false;
bool oldBasicGateOutBool = false;
void handleBasicGateOut() {
    //
    byte myTapTimer = clockIncrement % fullClockLength;
    if (myTapTimer < 12) {
        basicGateOutBool = true;
        
    }
    else {
        basicGateOutBool = false;
    }
    if (basicGateOutBool && !oldBasicGateOutBool) {
        oldBasicGateOutBool = true;
        if (settingsValues[settingsValueGate1] == 5) {
            digitalWrite(gate1pin, HIGH);
        }
        if (settingsValues[settingsValueGate2] == 5) {
            digitalWrite(gate1pin, HIGH);
        }
    }


    else if (!basicGateOutBool && oldBasicGateOutBool) {
        oldBasicGateOutBool = false;
        if (settingsValues[settingsValueGate1] == 5) {
            digitalWrite(gate1pin, LOW);
        }
        if (settingsValues[settingsValueGate2] == 5) {
            digitalWrite(gate1pin, LOW);
        }
    }
}
