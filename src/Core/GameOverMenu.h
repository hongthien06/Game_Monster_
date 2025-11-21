#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../Config/GameConstants.h"
#include <memory>

enum class GameOverChoice {
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

    GameOverChoice currentChoice;
    bool isTransitioning = false;
public:
    GameOverMenu(SDL_Renderer* ren, TTF_Font* fnt);
    ~GameOverMenu();
    void HandleInput();
    void Update(float deltaTime);
    void Render();
    GameOverChoice GetChoice() const { return currentChoice; }
    void Reset();
    void SetTransitioning(bool value) { isTransitioning = value; }
    bool IsTransitioning() const { return isTransitioning; }
};