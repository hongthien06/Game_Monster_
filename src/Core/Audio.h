#pragma once
#include "../miniaudio.h"
#include <vector>
#include <string>

class Audio {
private:
    ma_engine engine;
    std::vector<ma_sound*> activeSounds;
    void cleanupFinishedSounds();
public:
    Audio();
    ~Audio();
    void playSound(const std::string& filePath, bool loop = false);
    void stopAll();
};
