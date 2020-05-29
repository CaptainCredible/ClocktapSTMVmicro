
bool oldtapInState = false;
bool tapInState = false;
bool pedalHold = false;

int currentRotaryValue = 0;
int oldRotaryValue = 0;
int currenRotarytRange = 800;

void handleRotaryEncoder() {
    {
        static uint32_t count;
        static uint32_t prevCount;
        count = currentRotaryValue;
        prevCount = oldRotaryValue;
        if (count != prevCount)
        {
            oldRotaryValue = count;
            CompositeSerial.print(F("Count: "));
            CompositeSerial.println(count);


        }
    }
}


void handleButts() {
    handleTapInput();
    //CompositeSerial.println(digitalRead(PB4));

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

            
            
                clockDivisors[i]++;
                clockDivisors[i] = clockDivisors[i] % 4;
                setClockLengths(i);
            

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

    /*
                  //CompositeSerial.println("TAP IN");
                lastTimeOfTap = timeOfTap;
                timeOfTap = millis();
                if (timeOfTap - lastTimeOfTap < 1500) { //if less than 1.5 sec since last tap
                    tapTimer = timeOfTap - lastTimeOfTap;
                    clockStepTimer = tapTimer / 24;
                    tempo = 60000 / tapTimer;
                    printToTopMiddle(tempo);
                    handleStart();
                }
    */

    //CompositeSerial.println("handld");
    oldBigButtStates[pedIn] = bigButtStates[pedIn];
    if (bigDebounceReady[pedIn]) {
        bigButtStates[pedIn] = !digitalRead(tapIn);
    }
    else {
        debounce(1, pedIn);
    }

    //STILL SOME DEBOUNCE ISSUE HERE

    //                 ON                          OFF
    //                 _____________________________   _
    //                 |                            | | |
    //                 |                            | | |
    //_________________|                            |_| |________________________________

    if (bigButtStates[pedIn] && !oldBigButtStates[pedIn]) {
        //digitalWrite(LEDs[pedIn], HIGH);
        CompositeSerial.print("PEDAL   ");
        CompositeSerial.print(bigButtStates[pedIn]);
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