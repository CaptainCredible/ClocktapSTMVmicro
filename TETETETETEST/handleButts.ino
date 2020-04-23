
bool oldtapInState = false;
bool tapInState = false;
bool pedalHold = false;

void handleButts() {
    handleTapInput();
    /*
       // HANDLE LITTLE BUTTONS
       for (int i = 0; i < 4; i++) {
       oldLittleButtStates[i] = littleButtStates[i];

       if (smallDebounceReady[i]) {
         littleButtStates[i] = !digitalRead(littleButtPins[i]);
       }
       else {
         debounce(0, i);
       }

       if (littleButtStates[i] && !oldLittleButtStates[i]) {
         //    Serial.print("small button ");
         //    Serial.println(i);
         inversion[i] = !inversion[i];
         smallDebounceTimers[i] = millis();
         smallDebounceReady[i] = false;


         if (localTapTimer[i] > tapBlinkLength) { //set LED imediately to show respect

           digitalWrite(LEDs[i], inversion[i]);

         }
         else {
           digitalWrite(LEDs[i], !inversion[i]);
         }
         //Serial.print("inversion ");
         //Serial.print(i);
         //Serial.print(" = ");
         //Serial.println(inversion[i]);
       }
       else if (!littleButtStates[i] && oldLittleButtStates[i]) {
         smallDebounceTimers[i] = millis();
         smallDebounceReady[i] = false;
       }
     }
     */

    for (int i = 0; i < 4; i++) {
        oldBigButtStates[i] = bigButtStates[i];
        if (bigDebounceReady[i]) {
            bigButtStates[i] = !digitalRead(bigButtPins[i]);
        }
        else {
            debounce(1, i);
        }





        if (bigButtStates[i] && !oldBigButtStates[i]) {
            CompositeSerial.print("BUTT ");
            CompositeSerial.println(i);
            tripTimer[i] = millis();
            flippedTrips[i] = false;
            bigDebounceTimers[i] = millis();
            bigDebounceReady[i] = false;
            //HWMIDI.sendNoteOn(40+i, 127, 1);
            //HWMIDI.sendNoteOff(40+i, 127, 1);
            if ((i == 0) && intClock) {  //do this if we are handling last butt and we are in intClock
              //CompositeSerial.println("TAP IN");
                lastTimeOfTap = timeOfTap;
                timeOfTap = millis();
                if (timeOfTap - lastTimeOfTap < 3000) { //if less than 3 sec since last tap
                    tapTimer = timeOfTap - lastTimeOfTap;
                    clockStepTimer = tapTimer / 24;
                    handleStart();
                }
            }
            else {
                clockDivisors[i]++;
                clockDivisors[i] = clockDivisors[i] % 4;
                setClockLengths(i);
            }

        }
        else if (!bigButtStates[i] && oldBigButtStates[i]) {
            bigDebounceTimers[i] = millis();
            bigDebounceReady[i] = false;
            digitalWrite(LEDs[i], LOW);
        }

        if (bigButtStates[i]) {
            if (millis() - tripTimer[i] > 300 && !flippedTrips[i]) {
                //Serial.println(millis() - tripTimer[i]);
                triplets[i] = !triplets[i];
                flippedTrips[i] = true;
                clockDivisors[i]--;
                if (clockDivisors[i] < 0) {
                    clockDivisors[i] = 3;
                }
                setClockLengths(i);
                if (triplets[i]) {
                    blink(i, 3);
                }
                else {
                    blink(i, 1);
                }


                for (int pop = 0; pop < 4; pop++) {
                    //Serial.print("triplets ");
                    //Serial.print(pop);
                    //Serial.print(" = ");
                    //Serial.println(triplets[pop]);
                }
            }
        }
    }
}

void handleTapInput() {
    int pedIn = 4;

    oldBigButtStates[pedIn] = bigButtStates[pedIn];
    if (bigDebounceReady[pedIn]) {
        bigButtStates[pedIn] = !digitalRead(tapIn);
    }
    else {
        debounce(1, pedIn);
    }

    if (bigButtStates[pedIn] && !oldBigButtStates[pedIn]) {
        //digitalWrite(LEDs[pedIn], HIGH);
        CompositeSerial.print("PEDAL   ");
        tripTimer[pedIn] = millis();
        //flippedTrips[pedIn] = false;
        bigDebounceTimers[pedIn] = millis();
        bigDebounceReady[pedIn] = false;

        if ((pedIn == 4) && intClock) {  //don't do this if we are handling last butt and we are in intClock
            lastTimeOfTap = timeOfTap;
            timeOfTap = millis();
            if (timeOfTap - lastTimeOfTap < 3000) { //if less than 3 sec since last tap
                tapTimer = timeOfTap - lastTimeOfTap;
                clockStepTimer = tapTimer / 24;
                handleStart();
            }
        }
    }
    if (bigButtStates[pedIn]) {

        if (millis() - tripTimer[pedIn] > 300 && !pedalHold) {
            pedalHold = true;
            //Serial.println(millis() - tripTimer[pedIn]);
            //flippedTrips[pedIn] = !flippedTrips[pedIn];
            overrideMidiClock = !overrideMidiClock;
            if (overrideMidiClock) {
                blink(0, 6);
                blink(1, 6);
                intClock = true;

            }
            else {
                if (!notReceivedClockSinceBoot) { //dont turn intclock of if there is no other clock
                    intClock = false;
                }
                blink(2, 6);
                blink(3, 6);
            }
        }
    }
    else {
        pedalHold = false;
    }
}