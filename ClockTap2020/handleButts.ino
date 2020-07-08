
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
		clickTimer = millis() + 500;
	}
	if (!nowRotaryPush && millis() > clickTimer) { oldRotaryPush = false; }

	if (click) {
		switch (page) {
		case 0:
			page = 1;
			//settingsValues[1] = 0;
			break;

		case 1: // we are selecting things in the main menu
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
		case 6:  // we are on gate select page

			page = settingsValues[currentSetting] + 7; //make 0,1,2 select pages 7,8,9
			updateDisplay = true;
			break;
		case 7:  // we are on gate1 page

			page = 0;
			updateDisplay = true;
			break;
		case 8:  // we are on gate2 page
			page = 0;
			updateDisplay = true;
			break;
		case 9:  // we are on gateIn page
			page = 0;
			updateDisplay = true;
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
	//handleGateInput();
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

			}
		}
	}
}

bool oldGateInState = false;
bool gateInState = false;

/*
void handleGateInput() {
	gateInState = digitalRead(gateInPin);
	if (gateInState && !oldGateInState) { // if we jut went high
		gateInTrig();
		oldGateInState = gateInState;
	}
	else if (!gateInState && oldGateInState) { // if we went low
		oldGateInState = gateInState;
	}
}
*/


unsigned long oldGateMicros = 0;

void gateInTrig() {
	if (settingsValues[settingsValueGateInMode] < 3) { // if the gate in mode requires us to calculate tempo 
		//systick_disable();
		
		//umidi.sendStop();
		//systick_enable();
		
        //CompositeSerial.print("I can send a lot of data as long as it is all sent as one like this.");
		//CompositeSerial.print("5678");
		unsigned long NOW = micros();
		delta = NOW - oldGateMicros;
		if (NOW - oldGateMicros < 1000000) { //if less than 3 sec since last tap
			switch (settingsValues[settingsValueGateInMode]) {
			case 0: //1p
				clockStepTimer = delta / 6;
				break;
			case 1: //1/2p
				clockStepTimer = delta / 3;
				break;
			case 2: //1/4p
				clockStepTimer = delta / 1, 5;
				break;
			default:
				break;
			}

			//if(settingsValues[settingsValueGateInMode] == 0) clockStepTimer = delta / 6;
			//if (settingsValues[settingsValueGateInMode] == 1) clockStepTimer = delta / 6;


			settingsValues[settingsValueTempo] = 15000000 / delta;
		}
		else { //try to do handlestart() manually here
			//umidi.sendStart();
			   //clockIncrement = -1;
		//umidi.sendStop();
		//umidi.sendStart();
		//umidi.poll();
		//HWMIDI.sendRealTime(midi::Stop);
		//HWMIDI.sendRealTime(midi::Start);
			//umidi.sendReset();
		//handleStart();
		asyncHandleStart = true;
		}
		oldGateMicros = NOW;
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
			if (intClock && settingsValues[settingsValueFootMode] == FOOTMODETAP) {

				lastTimeOfTap = timeOfTap;
				timeOfTap = micros();
				if (timeOfTap - lastTimeOfTap < 2000000) { //if less than 3 sec since last tap
					tapTimer = timeOfTap - lastTimeOfTap;
					clockStepTimer = tapTimer / 24;
					settingsValues[settingsValueTempo] = 60000000 / tapTimer;
					handleStart();
				}
			} if (settingsValues[settingsValueFootMode] == FOOTMODESYNC) {
				handleStart();

			}
		}
		if (millis() - tripTimer[pedIn] > 1000 && !pedalHold) {
			pedalHold = true;
			settingsValues[settingsValueFootMode]++;
			settingsValues[settingsValueFootMode] = settingsValues[settingsValueFootMode] % 2;
			updateDisplay = true;

		}
	}
	else {
		pedalHold = false;
	}
}
