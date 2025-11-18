#pragma once
#include "miniaudio.h"
#include <string>
#include <vector>

class Audio {
public:
    Audio();
    ~Audio();

    // Play sound effect
    void playSound(const std::string& filePath, bool loop = false);
    void playSound(const std::string& filePath, bool loop, float volume);

    // BGM
    void playBGM(const std::string& filePath, bool loop = true, float volume = 1.0f);
    void stopBGM();
    void fadeOutBGM(float duration);
    void update(float deltaTime);

    // Mute/Unmute
    void muteBGM();
    void unmuteBGM();
    bool isMusicOn() const;

    // Stop all sounds
    void stopAll();

private:
    ma_engine engine;
    ma_sound bgm;
    bool bgmLoaded = false;

    std::vector<ma_sound*> activeSounds;

    // BGM volume control
    float bgmInitialVolume = 1.0f;
    float bgmCurrentVolume = 1.0f;

    // Fade out
    bool bgmFading = false;
    float bgmFadeDuration = 0.0f;
    float bgmFadeTimer = 0.0f;

    // Music state
    bool musicOn = true;

    void cleanupFinishedSounds();
};
