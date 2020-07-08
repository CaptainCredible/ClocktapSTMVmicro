void handleRotaryEncoder() {   
    {
        
        static uint32_t count;
        static uint32_t prevCount;
        count = settingsValues[currentSetting];
        prevCount = oldSettingsValues[currentSetting];
        if (count != prevCount)
        {
            /*
            CompositeSerial.print(currentSetting);
            CompositeSerial.print(" = ");
            CompositeSerial.println(settingsValues[currentSetting]);
            */
            oldSettingsValues[currentSetting] = count;
            updateDisplay = true;
            if (currentSetting == settingsValueTempo) {    //checks if it needs to recalculate tempo
                tapTimer =  60000000 / settingsValues[settingsValueTempo];
                clockStepTimer = tapTimer / 24;
            }
        }
    }
}