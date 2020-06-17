
bool oldtapInState = false;
bool tapInState = false;
bool pedalHold = false;
unsigned long clickTimer = 0;
int currenRotarytRange = 800;
bool oldRotaryPush = false;
byte clickCount = 0;
bool TIMETOSAVE = false;
void handleRotaryPush() {
	bool nowRotaryPush = !digitalRead(rotaryClick);
	if (nowRotaryPush && !oldRotaryPush) {
		oldRotaryPush = nowRotaryPush;
		click = true;
		clickTimer = millis() + 100;
	}
	if (!nowRotaryPush && millis() > clickTimer) { oldRotaryPush = false; }

	if (click) {
		CompositeSerial.println("click");
		switch (page) {
		case 0:
			page = 1;
			settingsValues[1] = 2;
			break;
		case 1: // we are selecting things in the menu
			if (settingsValues[1] == 1) { //SAVE BuTTON
				TIMETOSAVE = true;
				updateDisplay = true;
			}
			else {
				page = settingsValues[1]; //select what menu page to open
				updateDisplay = true;
			}
			break;

		case 2: //we are selecting clock input
			applyIOsettings();
			break;
		default:
			break;
		}
		click = false;
	}

}

void applyIOsettings() {
	byte INSETTING = settingsValues[settingsValueClockIn];
	switch (INSETTING) {
	case ClockSettingBoth:

		break;
	}
}

void handleButts() {
	handleRotaryPush();
	handleTapInput();
	for (int i = 0; i < 4; i++) {
		oldBigButtStates[i] = bigButtStates[i];
		if (bigDebounceReady[i]) {
			bigButtStates[i] = !digitalRead(bigButtPins[i]);
		}
		else {
			debounce(1, i);
		}


		if (bigButtStates[i] && !oldBigButtStates[i]) {

			tripTimer[i] = millis();
			flippedTrips[i] = false;
			bigDebounceTimers[i] = millis();
			bigDebounceReady[i] = false;

			clockDivisors[i]++;
			clockDivisors[i] = clockDivisors[i] % 5;
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
	int pedIn = 4; //pedal pins position in debounce array




	oldBigButtStates[pedIn] = bigButtStates[pedIn];
	if (bigDebounceReady[pedIn]) {
		bigButtStates[pedIn] = !digitalRead(tapIn);
	}
	else {
		debounce(1, pedIn);
	}

	//STILL SOME DEBOUNCE ISSUE HERE  [FIXED]

	//                 ON                          OFF
	//                 _____________________________   _
	//                 |                            | | |
	//                 |                            | | |
	//_________________|                            |_| |________________________________

	if (bigButtStates[pedIn] != oldBigButtStates[pedIn]) { //if there is a change, activate debounce
		bigDebounceTimers[pedIn] = millis();
		bigDebounceReady[pedIn] = false;
	}


	if (bigButtStates[pedIn] && !oldBigButtStates[pedIn]) {
		//digitalWrite(LEDs[pedIn], HIGH);

		if (timeSinceLastMidiMessage > 1000) {
			intClock = true;
		}

		tripTimer[pedIn] = millis();
		//flippedTrips[pedIn] = false;
		if (intClock) {
			lastTimeOfTap = timeOfTap;
			timeOfTap = millis();
			if (timeOfTap - lastTimeOfTap < 3000) { //if less than 3 sec since last tap
				tapTimer = timeOfTap - lastTimeOfTap;
				clockStepTimer = tapTimer / 24;
				settingsValues[settingsValueTempo] = 60000 / tapTimer;
				//tempo = 60000 / tapTimer;
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