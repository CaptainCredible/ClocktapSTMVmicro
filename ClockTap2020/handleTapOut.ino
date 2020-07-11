


//  16 steps
//  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #   
//  ################################################
//  48 clock increments

byte seqTimeSig = 6; //3 is 4/4, 2 is trip make this part of settingsvalues!
byte seqATimeSig = 6; //3 is 4/4, 2 is trip make this part of settingsvalues!
byte seqBTimeSig = 6; //3 is 4/4, 2 is trip make this part of settingsvalues!
bool seqAGateState = false;
bool oldseqAGateState = false;
bool seqBGateState = false;
bool oldseqBGateState = false;
bool aSeqIsRunning = false;
bool bSeqIsRunning = false;

void handleSeqGate() {
    if (settingsValues[settingsValueGate1] == 4) {  //SKIP THIS IF NO SEQ IS RUNNING
        aSeqIsRunning = true;
    }
    else {
        aSeqIsRunning = false;
    }

    if (settingsValues[settingsValueGate2] == 4) {  //SKIP THIS IF NO SEQ IS RUNNING
        bSeqIsRunning = true;
    }
    else {
        bSeqIsRunning = false;
    }

    if (aSeqIsRunning){
        uint16_t currentSeq = mySequences[0];  //make SEQ selectable
        //byte myASeqClock = clockIncrement % seqATimeSig;
        byte myASeqClock = clockIncrement % 6;
        if (myASeqClock == 0) {
            if (bitRead(currentSeq, 15 - seqACursor)) {
                seqAGateState = true;
            }
            seqACursor++;
            seqACursor = seqACursor % 16;
            if (page == 10) { //seq edit1 page
                updateDisplay = true;
            }

        }
        else {
            seqAGateState = false;
        }

        if (seqAGateState && !oldseqAGateState) {
            oldseqAGateState = true;
                digitalWrite(gate1pin, HIGH);
        }
        if (oldseqAGateState && !seqAGateState){
            oldseqAGateState = false;
                digitalWrite(gate1pin, LOW);
        }

    }
    if (bSeqIsRunning) {
        uint16_t currentSeq = mySequences[1];  //make SEQ selectable
        //byte myBSeqClock = clockIncrement % seqBTimeSig;
        byte myBSeqClock = clockIncrement % 6;
        if (myBSeqClock == 0) {
            if (bitRead(currentSeq, 15 - seqBCursor)) {
                seqBGateState = true;
            }
            seqBCursor++;
            seqBCursor = seqBCursor % 16;
            if (page == 11) { //seq edit1 page
                updateDisplay = true;
            }
        }
        else {
            seqBGateState = false;
        }

        if (seqBGateState && !oldseqBGateState) {
            oldseqBGateState = true;
            digitalWrite(gate2pin, HIGH);
        }
        if (oldseqBGateState && !seqBGateState) {
            oldseqBGateState = false;
            digitalWrite(gate2pin, LOW);
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

bool AbasicGateOutBool = false;
bool BbasicGateOutBool = false;
bool oldABasicGateOutBool = false;
bool oldBBasicGateOutBool = false;
void handleBasicGateOut() {
    byte myTapTimer = clockIncrement % fullClockLength;
    switch (settingsValues[settingsValueGate1]) {
    case 5: //basic 1/1 2ppq
        if (myTapTimer < 12) {
            AbasicGateOutBool = true;
        }
        else {
            AbasicGateOutBool = false;
        }
        break;
    case 6: //4PPQ
        if (myTapTimer%24 < 6) {
            AbasicGateOutBool = true;
            //CompositeSerial.print("X");
        } else {
            AbasicGateOutBool = false;
        }
        break;
    case 7:  //24PPQ
        if (myTapTimer % 2 == 0) {
            AbasicGateOutBool = true;
        }
        else {
            AbasicGateOutBool = false;
        }
    case 8:  //48PPQ
        //hmm
        break;
    }

    if (AbasicGateOutBool && !oldABasicGateOutBool) {
        oldABasicGateOutBool = true;
        if (settingsValues[settingsValueGate1] >= 5) {
            digitalWrite(gate1pin, HIGH);
        }
    }
    else if (!AbasicGateOutBool && oldABasicGateOutBool) {
        oldABasicGateOutBool = false;
        if (settingsValues[settingsValueGate1] >= 5) {
            digitalWrite(gate1pin, LOW);
        }
    }
    
    switch (settingsValues[settingsValueGate2]) {
    case 5: //basic 1/1 2ppq
        if (myTapTimer < 12) {
            BbasicGateOutBool = true;
        }
        else {
            BbasicGateOutBool = false;
        }
        break;
    case 6: //4PPQ
        if (myTapTimer & 24 < 6) {
            BbasicGateOutBool = true;
        }
        else {
            BbasicGateOutBool = false;
        }
        break;
    case 7:  //24PPQ
        if (myTapTimer % 2 == 0) {
            BbasicGateOutBool = true;
        }
        else {
            BbasicGateOutBool = false;
        }
    case 8:  //48PPQ
        //hmm
        break;
    }
    
    if (BbasicGateOutBool && !oldBBasicGateOutBool) {
        oldBBasicGateOutBool = true;
        if (settingsValues[settingsValueGate2] >= 5) {
            digitalWrite(gate2pin, HIGH);
        }
    }
    else if (!BbasicGateOutBool && oldBBasicGateOutBool) {
        oldBBasicGateOutBool = false;
        if (settingsValues[settingsValueGate2] >= 5) {
            digitalWrite(gate2pin, LOW);
        }
    }
}
