char subDivStrings[6][6] = {
						 "1/1",
						 "1/2",
						 "1/4",
						 "1/8",
						 "1/16",
						 "1/32",
};

void printToTopMiddle(int) {
	u8x8.clearLine(0);
	u8x8.setCursor(8, 0);
	u8x8.println(tempo);
}

void displayOverviewPage() {
	if (clockDivisors[0] != oldClockDivisors[0]) {
		u8x8.setCursor(0, 0);
		u8x8.println(subDivStrings[clockDivisors[0]] ) ;
		u8x8.print(clockDivisors[0]);
	}
}
