int updateNR = 0;
unsigned long endSaveTime = 0;
void handlePage() {

	if (TIMETOSAVE) {
		endSaveTime = 1000+millis();
		displaySavePage();
		TIMETOSAVE = false;
	}


	switch (page) {

	case 0:
		currentSetting = 0;
		displayOverviewPage();
		break;


	case 1:

		currentSetting = 1;
		if (updateDisplay) {
			displayMainMenu();
		}
		break;


	case 2:
		if (updateDisplay) {
			currentSetting = 2;
			displayInputMenu();
		}
		break;


	case 3:
		if (updateDisplay) {
			currentSetting = 3;
			displayOutputMenu();
		}
		break;


	case 4:
		if (updateDisplay) {
			currentSetting = 4;
			displayFootModeMenu();
		}
			break;


	case 5:
		if (updateDisplay) {
			currentSetting = 5;
			displayTapOutMenu();
		}
		break;


	case 6:
		
		if (updateDisplay) {
			currentSetting = 8;
			displayGateMenu();
		}
		break;


	case 7:
		if (updateDisplay) {
			currentSetting = 6;
			displayGate1Menu();
		}
		break;


	case 8:
		if (updateDisplay) {
			currentSetting = 7;
			displayGate2Menu();
		}
		break;


	case 9:
		if (updateDisplay) {
			currentSetting = 9;
			displayGateInMenu();
		}
		break;


	case 10:
		if (updateDisplay) {
			currentSetting = settingsValueSeqStepEdit;
			displaySeqEditor(0);
		}
		
		break;


	case 11:
		if (updateDisplay) {
			currentSetting = settingsValueSeqStepEdit;
			displaySeqEditor(1);
		}
		
		
	default:
		break;


	}
	if (updateDisplay) {
		updateDisplay = false;
		u8g2.sendBuffer();					// transfer internal memory to the display
		//CompositeSerial.println("U");
	}
}


void displayOverviewPage() {
	
	u8g2.clearBuffer();

	//check for changes:  //maybe make it so it can handle clearing and updating TEMPO without updating the rest?

	for (int i = 0; i < 4; i++) {
		if (clockDivisors[i] != displayedClockDivisors[i]) { updateDisplay = true; }
	}

	if (displayedTempo != settingsValues[settingsValueTempo]) {
		updateDisplay = true;
		displayedTempo = settingsValues[settingsValueTempo];
	}

	if (IOSettings != oldIOSettings) {
		updateDisplay = true;
		oldIOSettings = IOSettings;
	}

	if (updateDisplay) {
		displayFootMode();
		displayTempo();
		displayTimeSigs();
		displayIOStatus();
	}
}

void displayMainMenu() {

	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(50, 10, "MENU");
	//MENU ITEMS:  clock in, clock, out, Footmode, tap outputs, gate outputs, save
	
	
	u8g2.setCursor(0, 20);
	u8g2.print("clock in");
	if (settingsValues[1] == 2)u8g2.print(" <=");

	u8g2.setCursor(0, 30);
	u8g2.print("clock out");
	if (settingsValues[1] == 3)u8g2.print(" <=");

	u8g2.setCursor(0, 40);
	u8g2.print("footmode");
	if (settingsValues[1] == 4)u8g2.print(" <=");

	u8g2.setCursor(0, 50);
	u8g2.print("tap outs");
	if (settingsValues[1] == 5)u8g2.print(" <=");

	u8g2.setCursor(0, 60);
	u8g2.print("g1 / g2 / clk");
	if (settingsValues[1] == 6)u8g2.print(" <=");

	u8g2.setCursor(83, 50);
	if (settingsValues[1] == 0)u8g2.print("=> ");
	u8g2.setCursor(100, 50);
	u8g2.print("exit");

	u8g2.setCursor(83, 60);
	if (settingsValues[1] == 1)u8g2.print("=> ");
	u8g2.setCursor(100, 60);
	u8g2.print("save");

	u8g2.setCursor(83, 30);
	if (settingsValues[1] == 7)u8g2.print("=> ");
	u8g2.setCursor(100, 30);
	u8g2.print("g1seq");

	u8g2.setCursor(83, 40);
	if (settingsValues[1] == 8)u8g2.print("=> ");
	u8g2.setCursor(100, 40);
	u8g2.print("g2seq");




	
}



void displayGateMenu() {
	
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(50, 10, "g1/g2/CLK");
	u8g2.setCursor(0, 40);
	u8g2.print("gate 1");
	if (settingsValues[settingsValueGateSelect] == 0)u8g2.print(" <=");
	u8g2.setCursor(0, 50);
	u8g2.print("gate 2");
	if (settingsValues[settingsValueGateSelect] == 1)u8g2.print(" <=");
	u8g2.setCursor(0, 60);
	u8g2.print("CLK in");
	if (settingsValues[settingsValueGateSelect] == 2)u8g2.print(" <=");
 }

void displayGateInMenu() {
	byte tempOffset = 10;
	byte xTempOffset = 25;
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(40, 10, "GATE IN");
	u8g2.setCursor(0 + xTempOffset, 20 + tempOffset);
	u8g2.print("2ppq");
	if (settingsValues[currentSetting] == 0)u8g2.print(" <=");
	u8g2.setCursor(0 + xTempOffset, 35 + tempOffset);
	u8g2.print("4ppq");
	if (settingsValues[currentSetting] == 1)u8g2.print(" <=");
	u8g2.setCursor(0 + xTempOffset, 50 + tempOffset);
	u8g2.print("24ppq");
	if (settingsValues[currentSetting] == 2)u8g2.print(" <=");
	u8g2.setCursor(60 + xTempOffset, 20 + tempOffset);
	u8g2.print("48ppq");
	if (settingsValues[currentSetting] == 3)u8g2.print(" <=");
	
}

void displayGate1Menu() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(50, 10, "GATE 1");


	u8g2.setCursor(0, 30);
	u8g2.print("follow 1");
	if (settingsValues[settingsValueGate1] == 0)u8g2.print(" <=");

	u8g2.setCursor(0, 40);
	u8g2.print("follow 2");
	if (settingsValues[settingsValueGate1] == 1)u8g2.print(" <=");

	u8g2.setCursor(0, 50);
	u8g2.print("follow 3");
	if (settingsValues[settingsValueGate1] == 2)u8g2.print(" <=");

	u8g2.setCursor(0, 60);
	u8g2.print("follow 4");
	if (settingsValues[settingsValueGate1] == 3)u8g2.print(" <=");

	u8g2.setCursor(62, 30);
	if (settingsValues[settingsValueGate1] == 4)u8g2.print("=> ");
	u8g2.setCursor(75, 30);
	u8g2.print("sequencer");

	u8g2.setCursor(60, 40);
	if (settingsValues[settingsValueGate1] == 5)u8g2.print("=> ");
	u8g2.setCursor(75, 40);
	u8g2.print("2PPQ");
	
	u8g2.setCursor(60, 50);
	if (settingsValues[settingsValueGate1] == 6)u8g2.print("=> ");
	u8g2.setCursor(75, 50);
	u8g2.print("4PPQ");
	
	u8g2.setCursor(60, 60);
	if (settingsValues[settingsValueGate1] == 7)u8g2.print("=> ");
	u8g2.setCursor(75, 60);
	u8g2.print("24PPQ");




}

void displayGate2Menu() {
	
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(50, 10, "GATE 2");


	u8g2.setCursor(0, 30);
	u8g2.print("follow 1");
	if (settingsValues[settingsValueGate2] == 0)u8g2.print(" <=");

	u8g2.setCursor(0, 40);
	u8g2.print("follow 2");
	if (settingsValues[settingsValueGate2] == 1)u8g2.print(" <=");

	u8g2.setCursor(0, 50);
	u8g2.print("follow 3");
	if (settingsValues[settingsValueGate2] == 2)u8g2.print(" <=");

	u8g2.setCursor(0, 60);
	u8g2.print("follow 4");
	if (settingsValues[settingsValueGate2] == 3)u8g2.print(" <=");

	u8g2.setCursor(62, 30);
	if (settingsValues[settingsValueGate2] == 4)u8g2.print("=> ");
	u8g2.setCursor(75, 30);
	u8g2.print("sequencer");

	u8g2.setCursor(60, 40);
	if (settingsValues[settingsValueGate2] == 5)u8g2.print("=> ");
	u8g2.setCursor(75, 40);
	u8g2.print("2PPQ");

	u8g2.setCursor(60, 50);
	if (settingsValues[settingsValueGate2] == 6)u8g2.print("=> ");
	u8g2.setCursor(75, 50);
	u8g2.print("4PPQ");

	u8g2.setCursor(60, 60);
	if (settingsValues[settingsValueGate2] == 7)u8g2.print("=> ");
	u8g2.setCursor(75, 60);
	u8g2.print("24PPQ");

}

void displayTapOutMenu() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(40, 10, "tap outputs");

	u8g2.setCursor(0, 40);
	if (settingsValues[5] == 0)u8g2.print("=> ");
	u8g2.setCursor(17, 40);
	u8g2.print("1");
	if (inversion[0]) {
		u8g2.print(" invert");
	}
	else {
		u8g2.print(" normal");
	}

	u8g2.setCursor(0, 50);
	if (settingsValues[5] == 1)u8g2.print("=> ");
	u8g2.setCursor(17, 50);
	u8g2.print("2");
	if (inversion[1]) {
		u8g2.print(" invert");
	}
	else {
		u8g2.print(" normal");
	}
	
	u8g2.setCursor(70, 40);
	if (settingsValues[5] == 2)u8g2.print("=> ");
	u8g2.setCursor(83, 40);
	u8g2.print("3");
	if (inversion[2]) {
		u8g2.print(" invert");
	}
	else {
		u8g2.print(" normal");
	}


	u8g2.setCursor(70, 50);
	if (settingsValues[5] == 3)u8g2.print("=> ");
	u8g2.setCursor(83, 50);
	u8g2.print("4");
	if (inversion[3]) {
		u8g2.print(" invert");
	}
	else {
		u8g2.print(" normal");
	}
	u8g2.setCursor(70, 60);
	if (settingsValues[5] == 4)u8g2.print("=> ");
	u8g2.setCursor(83, 60);
	u8g2.print("back");


	
}

void displayFootModeMenu() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(40, 10, "footmode");
	
	u8g2.setCursor(40, 40);
	u8g2.print("tap tempo");
	if (settingsValues[4] == 0)u8g2.print(" <=");

	u8g2.setCursor(40, 50);
	u8g2.print("resync");
	if (settingsValues[4] == 1)u8g2.print(" <=");
}

void displaySavePage() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_open_iconic_check_6x_t);
	u8g2.drawGlyph(40,50,64);
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(45,63,"SAVED");

	//u8g2.drawStr(25, 50, "SAVING");
	u8g2.sendBuffer();

	save();

	while (millis() < endSaveTime) {
	}
	//page = 1;
}

void displayInputMenu() {
	
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(32, 10, "CLOCK INPUT");
	
	u8g2.setCursor(0, 20);
	u8g2.print("AUTO");
	if(settingsValues[currentSetting] == ClockSettingBoth) u8g2.print(" <=");
	
	u8g2.setCursor(0, 30);
	u8g2.print("USB");
	if (settingsValues[currentSetting] == ClockSettingUSB) u8g2.print(" <=");

	u8g2.setCursor(0, 40);
	u8g2.print("DIN");
	if (settingsValues[currentSetting] == ClockSettingDIN) u8g2.print(" <=");

	u8g2.setCursor(0, 50);
	u8g2.print("OFF");
	if (settingsValues[currentSetting] == ClockSettingOFF) u8g2.print(" <=");

	u8g2.setCursor(0, 60);
	u8g2.print("CLK");
	if (settingsValues[currentSetting] == ClockSettingCLK) u8g2.print(" <=");
}

void displayOutputMenu() {

	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(29, 10, "CLOCK OUTPUT");

	u8g2.setCursor(0, 20);
	u8g2.print("BOTH");
	if (settingsValues[currentSetting] == ClockSettingBoth) u8g2.print(" <=");

	u8g2.setCursor(0, 30);
	u8g2.print("USB");
	if (settingsValues[currentSetting] == ClockSettingUSB) u8g2.print(" <=");

	u8g2.setCursor(0, 40);
	u8g2.print("DIN");
	if (settingsValues[currentSetting] == ClockSettingDIN) u8g2.print(" <=");

	u8g2.setCursor(0, 50);
	u8g2.print("OFF");
	if (settingsValues[currentSetting] == ClockSettingOFF) u8g2.print(" <=");
}

void displayTempo() {

	// TEMPO DISPLAY

	u8g2.setFont(u8g2_font_freedoomr25_tn);	// choose a suitable font
	byte tempoYPos = 50;
	//u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
	//u8g2.drawStr(0, 10, "Horld!");  // write something to the internal memory
	//u8g2.drawStr(0, 20, "YOLO");

	if (settingsValues[settingsValueTempo] > 199) {
		u8g2.setCursor(34, tempoYPos);

	}
	else if (settingsValues[settingsValueTempo] > 99) {
		u8g2.setCursor(32, tempoYPos);
	}
	else {
		u8g2.setCursor(45, tempoYPos);
	}
	u8g2.print(settingsValues[settingsValueTempo]);
	u8g2.setCursor(0, 0);

}



void displaySeqEditor(byte whotSeq) {
	
	
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.setCursor(40, 10);
	u8g2.print("g");
	u8g2.print(whotSeq+1);
	u8g2.print(" seq edit");
	u8g2.setCursor(20, 30);
	byte whotSeqCursor = seqACursor;
	if (whotSeq == 1) whotSeqCursor = seqBCursor;
	for (int i = 0; i < 16; i++) {
		if (whotSeqCursor == i) {
			u8g2.print("+");
		}
		else {
			u8g2.print("-");
		}
	}
	u8g2.setCursor(20, 50);

	u8g2.setCursor(20, 40);
	for (int i = 15; i >=0; i--) {
		if (bitRead(mySequences[whotSeq], i)) {
			u8g2.print("X");
		}
		else {
			u8g2.print("-");
		}
	}
	u8g2.setCursor(20, 50);
	for (int i = 0; i < 16; i++) {
		if (i == settingsValues[settingsValueSeqStepEdit]) {
			u8g2.print("+");
		}
		else {
			u8g2.print("-");
		}
	}
	u8g2.setCursor(65, 60);
	if(oldSettingsValues[settingsValueSeqStepEdit] == 16) u8g2.print("=>"); 
	u8g2.setCursor(80, 60);
	u8g2.print("done");
	
}

void displayIOStatus() {
	u8g2.setFont(u8g2_font_missingplanet_tr);

	//LEFT SIDE, INPUTS
	switch (settingsValues[settingsValueClockIn]) {
	case ClockSettingBoth:
		u8g2.drawStr(0, 10, "AUTO");
		break;

	case ClockSettingUSB:
		u8g2.drawStr(0, 10, "USB");
		break;

	case ClockSettingDIN:
		u8g2.drawStr(0, 10, "DIN");
		break;

	case ClockSettingOFF:
		u8g2.drawStr(0, 10, "OFF");
		break;
	case ClockSettingCLK:
		u8g2.drawStr(0, 10, "CLK");
		break;

	}
	//RIGHT SIDE, OUTPUT
	switch (settingsValues[settingsValueClockOut]) {
	case ClockSettingBoth:
		u8g2.drawStr(85, 10, "USB+DIN");
		break;
	case ClockSettingUSB:
		u8g2.drawStr(110, 10, "USB");
		break;
	case ClockSettingDIN:
		u8g2.drawStr(110, 10, "DIN");
		break;
	case ClockSettingOFF:
		u8g2.drawStr(110, 10, "OFF");
		break;
	default:
		break;

	}

}


void displayTimeSigs() {
	//timesigs display

	for (byte i = 0; i < 4; i++) {

		//updateDisplay = true;
		u8g2.setFont(u8g2_font_missingplanet_tr);	// choose a suitable font

		byte myX = bitRead(i, 1) * 123;
		byte myY = bitRead(i, 0) * 20;
		if (i > 1) { //we are drawing on the right
			if (clockDivisors[i] > 3) {
				myX -= 6;
			}
			if (triplets[i]) {
				myX -= 6;
			}
		}
		byte timeSigsY = 43;
		if (triplets[i]) {
			u8g2.drawStr(myX, timeSigsY + myY, tripSubDivStrings[clockDivisors[i]]);
		}
		else {

			u8g2.drawStr(myX, timeSigsY + myY, subDivStrings[clockDivisors[i]]);
		}
		displayedClockDivisors[i] = clockDivisors[i];
	}
}

void displayFootMode() {
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.setCursor(39, 64);
	u8g2.println("foot: ");
	if (settingsValues[settingsValueFootMode] == FOOTMODETAP) {
		u8g2.print("TAP");
	}
	else {
		u8g2.print("SYNC");
	}


}

