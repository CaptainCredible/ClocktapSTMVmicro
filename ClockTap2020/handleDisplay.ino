
void handleDisplay() {
	displayOverviewPage();
}




bool updateDisplay = false;
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



// TEMPO DISPLAY
	if (updateDisplay) {
		
		u8g2.setFont(u8g2_font_freedoomr25_tn);	// choose a suitable font

		//u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
		//u8g2.drawStr(0, 10, "Horld!");  // write something to the internal memory
		//u8g2.drawStr(0, 20, "YOLO");

		if (settingsValues[tempo] > 199) {
			u8g2.setCursor(34, 45);
			
		}
		else if(settingsValues[tempo]>99){
			u8g2.setCursor(32, 45);
		}
		else {
			u8g2.setCursor(45, 45);
		}
		u8g2.print(settingsValues[tempo]);
		u8g2.setCursor(0, 0);
	}
	
//timesigs display
	
		for (byte i = 0; i < 4; i++) {
			if (updateDisplay) {
				updateDisplay = true;
				u8g2.setFont(u8g2_font_missingplanet_tr);	// choose a suitable font
				//u8g2.setCursor(0, 0);
				//u8g2.setCursor((bitRead(i,0))*58,(bitRead(i,1))*120);
				//u8g2.println(subDivStrings[clockDivisors[i]]);
				//if()
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

				if (triplets[i]) {
					u8g2.drawStr(myX, 40 + myY, tripSubDivStrings[clockDivisors[i]]);
				}
				else {

					u8g2.drawStr(myX, 40 + myY, subDivStrings[clockDivisors[i]]);
				}
				displayedClockDivisors[i] = clockDivisors[i];
			}
}

		
	




	if (updateDisplay) {
		updateDisplay = false;
		u8g2.sendBuffer();					// transfer internal memory to the display
		
	}
}
