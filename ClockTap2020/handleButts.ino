
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
		switch (page) {
		case 0:
			page = 1;
			settingsValues[1] = 0;
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
			page = 0;
			updateDisplay = true;
			break;

		case 3: //we are selecting clock output
			applyIOsettings();
			page = 0;
			updateDisplay = true;
			break;
		case 4: //we are setting foot mode
			page = 0;
			updateDisplay = true;
			break;
		case 5: //we are setting tapout inversion
			//page = 0;
			if (settingsValues[5] == 4) {
				page = 0;
				updateDisplay = true;
			}
			else {
				inversion[settingsValues[5]] = !inversion[settingsValues[5]];
				updateDisplay = true;
			}
			break;
		case 6:
			page = 0;
			updateDisplay = true;

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
		enableUSBclockIN = true;
		enableDINclockIN = true;
		break;
	case ClockSettingDIN:
		enableUSBclockIN = false;
		enableDINclockIN = true;
		break;
	case ClockSettingUSB:
		enableUSBclockIN = true;
		enableDINclockIN = false;
		break;
	case ClockSettingOFF:
		enableUSBclockIN = false;
		enableDINclockIN = false;
		break;
	}

	byte OUTSETTING = settingsValues[settingsValueClockOut];
	switch (OUTSETTING) {
	case ClockSettingBoth:
		enableUSBclockOUT = true;
		enableDINclockOUT = true;
		break;
	case ClockSettingDIN:
		enableUSBclockOUT = false;
		enableDINclockOUT = true;
		break;
	case ClockSettingUSB:
		enableUSBclockOUT = true;
		enableDINclockOUT = false;
		break;
	case ClockSettingOFF:
		enableUSBclockOUT = false;
		enableDINclockOUT = false;
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


	if (bigButtStates[pedIn]) {

		if (!oldBigButtStates[pedIn]) {
			if (timeSinceLastMidiMessage > 1000) {
				intClock = true;
			}
			tripTimer[pedIn] = millis();
			if (intClock) {
				lastTimeOfTap = timeOfTap;
				timeOfTap = micros();
				if (timeOfTap - lastTimeOfTap < 3000000) { //if less than 3 sec since last tap
					tapTimer = timeOfTap - lastTimeOfTap;
					clockStepTimer = tapTimer / 24;
					settingsValues[settingsValueTempo] = 60000000 / tapTimer;
					//tempo = 60000 / tapTimer;
					handleStart();
				}
			}
		}
		if (millis() - tripTimer[pedIn] > 1000 && !pedalHold) {
			pedalHold = true;
			settingsValues[settingsValueFootMode]++;
			settingsValues[settingsValueFootMode] % 2;
		}

	} else {
		pedalHold = false;
	}
}