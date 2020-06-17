int updateNR = 0;

void handleDisplay() {
	switch (page) {
	case 0:
		displayOverviewPage();
		currentSetting = 0;
		break;
	case 1:
		displayMainMenu();
		currentSetting = 1;
		break;
	case 2:
		displayInputMenu();
		break;
	case 3:
		displayOutputMenu();
		break;
	default:
		break;
	}
	if (updateDisplay) {
		updateDisplay = false;
		u8g2.sendBuffer();					// transfer internal memory to the display
	}
}

void displayMainMenu() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(50, 10, "MENU");
	//MENU ITEMS:  clock in, clock, out, Footmode, tap outputs, gate outputs, save
	u8g2.setCursor(0, 20);
	u8g2.print("clk in");
	if (settingsValues[1] == 0)u8g2.print(" <=");

	u8g2.setCursor(0, 30);
	u8g2.print("clk out");
	if (settingsValues[1] == 1)u8g2.print(" <=");

	u8g2.setCursor(0, 40);
	u8g2.print("footmode");
	if (settingsValues[1] == 2)u8g2.print(" <=");

	u8g2.setCursor(0, 50);
	u8g2.print("tap outs");
	if (settingsValues[1] == 3)u8g2.print(" <=");
	
	u8g2.setCursor(0, 60);
	u8g2.print("gate outs");
	if (settingsValues[1] == 4)u8g2.print(" <=");

	u8g2.setCursor(83, 60);
	if (settingsValues[1] == 5)u8g2.print("=> ");
	u8g2.setCursor(100, 60);
	u8g2.print("save");



	updateDisplay = true;
	//set rotary value to choose pageSelect
	//display list of pages
	//display cursor next to selection
}

void displayInputMenu() {
}

void displayOutputMenu() {
}

void displayTempo() {

	// TEMPO DISPLAY

	u8g2.setFont(u8g2_font_freedoomr25_tn);	// choose a suitable font
	byte tempoYPos = 65;
	//u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
	//u8g2.drawStr(0, 10, "Horld!");  // write something to the internal memory
	//u8g2.drawStr(0, 20, "YOLO");

	if (settingsValues[tempo] > 199) {
		u8g2.setCursor(34, tempoYPos);

	}
	else if (settingsValues[tempo] > 99) {
		u8g2.setCursor(32, tempoYPos);
	}
	else {
		u8g2.setCursor(45, tempoYPos);
	}
	u8g2.print(settingsValues[tempo]);
	u8g2.setCursor(0, 0);

}

void displayIOStatus() {

	//LEFT SIDE, INPUTS
	if (bitRead(IOSettings, EXTCLOCKAUTOBIT)) {
		u8g2.drawStr(0, 10, "AUTO");
	}
	else {
		byte DINYOFFSET = 0;
		if (bitRead(IOSettings, EXTCLOCKUSBBIT)) {
			u8g2.drawStr(0, 10, "USB");
			DINYOFFSET = 10;
		}
		if (bitRead(IOSettings, EXTCLOCKDINBIT)) {
			u8g2.drawStr(0, 10 + DINYOFFSET, "DIN");
		}
	}
	//RIGHT SIDE, OUTPUT

	byte DINYOFFSET = 0;
	if (bitRead(IOSettings, SENDUSBBIT)) {
		u8g2.drawStr(100, 10, "USB");
		DINYOFFSET = 10;
	}
	if (bitRead(IOSettings, SENDDINBIT)) {
		u8g2.drawStr(100, 10 + DINYOFFSET, "DIN");
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

void displayOverviewPage() {

	u8g2.clearBuffer();

	//check for changes:

	for (int i = 0; i < 4; i++) {
		if (clockDivisors[i] != displayedClockDivisors[i]) { updateDisplay = true; }
	}

	if (displayedTempo != settingsValues[tempo]) {
		updateDisplay = true;
		displayedTempo = settingsValues[tempo];
	}

	if (IOSettings != oldIOSettings) {
		updateDisplay = true;
		oldIOSettings = IOSettings;
	}

	if (updateDisplay) {
		displayTempo();
		displayTimeSigs();
		displayIOStatus();
		//		updateNR++;
		//		CompositeSerial.println(updateNR);
	}
}
