#include <SDL3/SDL.h>
#include "SDL_mixer.h"
#include "Audio.h" 

using namespace std;

bool Audio::initialize() {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        SDL_Log("Failed to initialize SDL audio subsystem: %s", SDL_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, SDL_AUDIO_S16, 2, 2048) < 0) {
        SDL_Log("Failed to initialize SDL_mixer: %s", Mix_GetError());
        return false;
    }

    return true;
}

// Load am thanh tu file
bool Audio::loadSound(const string& landingPath) {
    landingSound = Mix_LoadWAV(landingPath.c_str());
    if (!landingSound) {
        SDL_Log("Failed to load landing sound: %s", Mix_GetError());
        return false;
    }
    return true;
}

// Chay am thanh
void Audio::playLandingSound() {
    if (landingSound) {
        Mix_PlayChannel(-1, landingSound, 0);
    } else {
        SDL_Log("Landing sound not loaded!");
    }
}

// Giai phong tai nguyen va tat SDL_mixer
void Audio::shutdown() {
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

