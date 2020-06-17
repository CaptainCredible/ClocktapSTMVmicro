void handleRotaryEncoder() {   //checks if it needs to recalculate tempo
    {
        static uint32_t count;
        static uint32_t prevCount;
        count = settingsValues[currentSetting];
        prevCount = oldSettingsValues[currentSetting];
        if (count != prevCount)
        {
            oldSettingsValues[currentSetting] = count;
            if (currentSetting == tempo) {
                tapTimer =  60000 / settingsValues[tempo];
                clockStepTimer = tapTimer / 24;
            }
        }
    }
}
