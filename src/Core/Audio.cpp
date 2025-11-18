#define MINIAUDIO_IMPLEMENTATION
#include "Audio.h"
#include <iostream>

using namespace std;

Audio::Audio() {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        cerr << "Khong the khoi tao engine am thanh! Error code: " << result << "\n";
    }
    else {
        cout << "Audio engine initialized successfully\n";
    }
}

Audio::~Audio() {
    for (auto sound : activeSounds) {
        ma_sound_uninit(sound);
        delete sound;
    }
    activeSounds.clear();

    if (bgmLoaded) {
        ma_sound_uninit(&bgm);
    }

    ma_engine_uninit(&engine);
}

void Audio::playSound(const std::string& filePath, bool loop) {
    if (!musicOn) return; // tôn trọng trạng thái musicOn

    cleanupFinishedSounds();

    ma_sound* sound = new ma_sound;
    if (ma_sound_init_from_file(&engine, filePath.c_str(), 0, NULL, NULL, sound) != MA_SUCCESS) {
        cerr << "Khong the tai am thanh: " << filePath << "\n";
        delete sound;
        return;
    }

    ma_sound_set_looping(sound, loop);
    ma_sound_set_volume(sound, 1.0f);
    ma_sound_start(sound);
    activeSounds.push_back(sound);
}

void Audio::playSound(const std::string& filePath, bool loop, float volume) {
    if (!musicOn) return; // tôn trọng trạng thái musicOn

    cleanupFinishedSounds();

    ma_sound* sound = new ma_sound;
    if (ma_sound_init_from_file(&engine, filePath.c_str(), 0, NULL, NULL, sound) != MA_SUCCESS) {
        cerr << "Khong the tai am thanh: " << filePath << "\n";
        delete sound;
        return;
    }

    ma_sound_set_looping(sound, loop);
    ma_sound_set_volume(sound, volume);
    ma_sound_start(sound);
    activeSounds.push_back(sound);
}

void Audio::cleanupFinishedSounds() {
    auto it = activeSounds.begin();
    while (it != activeSounds.end()) {
        if (!ma_sound_is_playing(*it)) {
            ma_sound_uninit(*it);
            delete* it;
            it = activeSounds.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Audio::playBGM(const std::string& filePath, bool loop, float volume) {
    cout << "Trying to play BGM: " << filePath << endl;

    if (bgmLoaded) {
        ma_sound_stop(&bgm);
        ma_sound_uninit(&bgm);
    }

    ma_result result = ma_sound_init_from_file(&engine, filePath.c_str(), 0, NULL, NULL, &bgm);
    if (result == MA_SUCCESS) {
        bgmLoaded = true;
        bgmInitialVolume = volume;
        bgmCurrentVolume = volume;
        ma_sound_set_looping(&bgm, loop);

        if (musicOn)
            ma_sound_set_volume(&bgm, volume);
        else
            ma_sound_set_volume(&bgm, 0.0f);

        ma_sound_start(&bgm);
        cout << "BGM started. Volume: " << bgmCurrentVolume << endl;
    }
    else {
        cout << "Failed to load BGM file. Error code: " << result << endl;
    }
}

void Audio::stopBGM() {
    if (bgmLoaded) {
        ma_sound_stop(&bgm);
    }
}

void Audio::fadeOutBGM(float duration) {
    if (!bgmLoaded) return;
    bgmFadeDuration = duration;
    bgmFadeTimer = duration;
    bgmFading = true;
}

void Audio::update(float deltaTime) {
    cleanupFinishedSounds();

    if (!bgmLoaded || !bgmFading) return;

    bgmFadeTimer -= deltaTime;
    float t = bgmFadeTimer / bgmFadeDuration;
    if (t < 0.0f) t = 0.0f;

    bgmCurrentVolume = t * bgmInitialVolume;
    ma_sound_set_volume(&bgm, bgmCurrentVolume);

    if (t <= 0.0f) {
        ma_sound_stop(&bgm);
        ma_sound_uninit(&bgm);
        bgmLoaded = false;
        bgmFading = false;
    }
}

void Audio::stopAll() {
    for (auto sound : activeSounds) {
        ma_sound_stop(sound);
    }
}

void Audio::muteBGM() {
    if (bgmLoaded) {
        ma_sound_stop(&bgm);       // Dừng BGM hoàn toàn
        bgmFading = false;
        bgmCurrentVolume = 0.0f;
        std::cout << "BGM muted (stopped).\n";
    }
}

void Audio::unmuteBGM() {
    if (bgmLoaded) {
        ma_sound_start(&bgm);      // Bắt đầu lại
        ma_sound_set_volume(&bgm, bgmInitialVolume);
        bgmCurrentVolume = bgmInitialVolume;
        std::cout << "BGM unmuted (Volume restored: " << bgmInitialVolume << ").\n";
    }
}


bool Audio::isMusicOn() const {
    return musicOn;
}
