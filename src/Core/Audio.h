#pragma once
#include "miniaudio.h"
#include <vector>
#include <string>

class Audio {
private:
    ma_engine engine;
    
    ma_sound bgm;
    bool bgmLoaded = false;
    float bgmCurrentVolume = 1.0f;
    float bgmInitialVolume = 1.0f;

    std::vector<ma_sound*> activeSounds;
    void cleanupFinishedSounds();  

     // Cac bien can thiet de tao hiue ung fade cho am thanh
    float bgmFadeTimer = 0.0f;
    float bgmFadeDuration = 0.0f;
    bool bgmFading = false;
public:
    Audio();
    ~Audio();
    void update(float deltaTime); 
    void playSound(const std::string& filePath, bool loop = false);
    void playSound(const std::string& filePath, bool loop, float volume);
    void stopAll();
    void playBGM(const std::string& filePath, bool loop, float volume);
    void stopBGM();
    void fadeOutBGM(float duration); // Them ham tao Fade de chuyen nhac
    void setBGMVolume(float volume); // Chinh am luong
    void muteBGM();
    void unmuteBGM();
};
