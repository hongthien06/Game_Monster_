#define MINIAUDIO_IMPLEMENTATION
#include <iostream>
#include "Audio.h"

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
    // Giải phóng hiệu ứng
    for (auto sound : activeSounds) {
        ma_sound_uninit(sound);
        delete sound;
    }
    activeSounds.clear();

    // Giải phóng BGM nếu có
    if (bgmLoaded) {
        ma_sound_uninit(&bgm);
    }

    ma_engine_uninit(&engine);
}

void Audio::playSound(const std::string& filePath, bool loop) {
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

    if (bgmLoaded) {
        ma_sound_stop(&bgm); // chỉ stop, không uninit ngay
        ma_sound_uninit(&bgm); // uninit 1 lần duy nhất, an toàn
    }

    ma_result result = ma_sound_init_from_file(&engine, filePath.c_str(), 0, NULL, NULL, &bgm);
    if (result == MA_SUCCESS) {
        bgmLoaded = true;
        bgmVolume = volume;
        ma_sound_set_volume(&bgm, volume);
        ma_sound_set_looping(&bgm, loop);
        ma_sound_start(&bgm);
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
    if (!bgmLoaded || !bgmFading) return;

    bgmFadeTimer -= deltaTime;
    float t = bgmFadeTimer / bgmFadeDuration;
    if (t < 0.0f) t = 0.0f;

    bgmVolume = t;
    ma_sound_set_volume(&bgm, bgmVolume);

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

// void Audio::setBGMVolume(float volume) {
//     if (!bgmLoaded) return;

//     // Giu volume trong khoang an toan
//     if (volume < 0.0f) volume = 0.0f;
//     if (volume > 1.0f) volume = 1.0f;

//     bgmVolume = volume;
//     ma_sound_set_volume(&bgm, bgmVolume);
// }