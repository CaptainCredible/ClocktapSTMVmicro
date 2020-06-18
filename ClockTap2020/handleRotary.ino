void handleRotaryEncoder() {   //checks if it needs to recalculate tempo
    {
        static uint32_t count;
        static uint32_t prevCount;
        count = settingsValues[currentSetting];
        prevCount = oldSettingsValues[currentSetting];
        if (count != prevCount)
        {
            oldSettingsValues[currentSetting] = count;
            updateDisplay = true;
            if (currentSetting == settingsValueTempo) {
                tapTimer =  60000000 / settingsValues[settingsValueTempo];
                clockStepTimer = tapTimer / 24;
            }
        }
    }
}
