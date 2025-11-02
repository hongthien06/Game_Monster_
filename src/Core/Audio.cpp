#define MINIAUDIO_IMPLEMENTATION
#include "Audio.h"
#include <iostream>

using  namespace std;

Audio::Audio() {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        cerr << "Khong the khoi tao engine am thanh!.\n";
    }
}

Audio::~Audio() {
    // Cleanup all active sounds
    for (auto sound : activeSounds) {
        ma_sound_uninit(sound);
        delete sound;
    }
    activeSounds.clear();
    ma_engine_uninit(&engine);
}

void Audio::playSound(const std::string& filePath, bool loop) {
    // Clean up finished sounds first
    cleanupFinishedSounds();

    ma_sound* sound = new ma_sound;
    if (ma_sound_init_from_file(&engine, filePath.c_str(), 0, NULL, NULL, sound) != MA_SUCCESS) {
        cerr << "Khong the tai am thanh: " << filePath << "\n";
        delete sound;
        return;
    }
    ma_sound_set_looping(sound, loop);
    ma_sound_set_volume(sound, 0.7f); // 0.0f = im lặng, 1.0f = âm lượng gốc
    ma_sound_start(sound);
    activeSounds.push_back(sound);
}

void Audio::cleanupFinishedSounds() {
    auto it = activeSounds.begin();
    while (it != activeSounds.end()) {
        if (!ma_sound_is_playing(*it)) {
            ma_sound_uninit(*it);
            delete *it;
            it = activeSounds.erase(it);
        } else {
            ++it;
        }
    }
}

void Audio::stopAll() {
    // Stop all sounds
    for (auto sound : activeSounds) {
        ma_sound_stop(sound);
    }
    ma_engine_stop(&engine);
}
