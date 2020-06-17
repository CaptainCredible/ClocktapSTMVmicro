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

void displaySavePage() {
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_DigitalDisco_tf);
	u8g2.drawStr(0, 50, "SAVING");
	CompositeSerial.println("SAV");
	u8g2.sendBuffer();
	while (millis() < endSaveTime) {
		//CompositeSerial.println("wait");
	}
	//page = 1;
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
	u8g2.print("gate outs");
	if (settingsValues[1] == 6)u8g2.print(" <=");

	u8g2.setCursor(83, 50);
	if (settingsValues[1] == 1)u8g2.print("=> ");
	u8g2.setCursor(100, 50);
	u8g2.print("save");

	u8g2.setCursor(83, 60);
	if (settingsValues[1] == 0)u8g2.print("=> ");
	u8g2.setCursor(100, 60);
	u8g2.print("back");


	updateDisplay = true;
	//set rotary value to choose pageSelect
	//display list of pages
	//display cursor next to selection
}

void displayInputMenu() {
	currentSetting = 2;
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_missingplanet_tr);
	u8g2.drawStr(32, 10, "CLOCK INPUT");
	
	u8g2.setCursor(0, 20);
	u8g2.print("BOTH");
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
}

void displayOutputMenu() {
	currentSetting = 3;
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
	byte tempoYPos = 65;
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
	}
	//RIGHT SIDE, OUTPUT
	switch (settingsValues[settingsValueClockOut]) {
	case ClockSettingBoth:
		u8g2.drawStr(85, 10, "USB+DIN");
		break;
	case ClockSettingUSB:
		u8g2.drawStr(100, 10, "USB");
		break;
	case ClockSettingDIN:
		u8g2.drawStr(100, 10, "DIN");
		break;
	case ClockSettingOFF:
		u8g2.drawStr(100, 10, "OFF");
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

void displayOverviewPage() {

	u8g2.clearBuffer();

	//check for changes:

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
		displayTempo();
		displayTimeSigs();
		displayIOStatus();
		//		updateNR++;
		//		CompositeSerial.println(updateNR);
	}
}