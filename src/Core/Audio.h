#pragma once
#include <SDL3/SDL.h>
#include "SDL_mixer.h"
#include <string>

class Audio {
public:
    Audio();
    ~Audio();

    // Khoi tao SDL_mixer
    bool initialize();

    // Load am thanh tu file
    bool loadSound(const std::string& landingPath);

    // CChay am thanh
    void playLandingSound();

    // Giai phong tai nguyen va tat SDL_mixer
    void shutdown();

private:
    Mix_Chunk* landingSound; // Dung luu am thanh khi dap dat
};
