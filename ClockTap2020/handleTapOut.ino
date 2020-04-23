
void handleTapOut() {
    //CompositeSerial.println("TAP! ");
    for (int i = 0; i < 4; i++) {

        localTapTimer[i] = clockIncrement % clockLengths[i];
        //  //Serial.println(taps[i]);

        //CompositeSerial.println(clockIncrement);

        if (localTapTimer[i] == 0) {

            taps[i] = true;
            if (i == 0) {

                //    HWMIDI.sendNoteOn(42,128,1);
            }

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
}