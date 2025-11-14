#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../Config/GameConstants.h"
#include <memory>

enum class MenuChoice {
    NONE,       
    REPLAY,
    QUIT
};

class GameOverMenu {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int selectedOption; 
    float inputTimer;
    const float inputCooldown = 0.2f;

    MenuChoice currentChoice;

public:
    GameOverMenu(SDL_Renderer* ren, TTF_Font* fnt);
    ~GameOverMenu();

 
    void HandleInput();
    void Update(float deltaTime);
    void Render();
    MenuChoice GetChoice() const { return currentChoice; }
    void Reset();
};