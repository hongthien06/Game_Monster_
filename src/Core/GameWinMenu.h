#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../Config/GameConstants.h"
#include <memory>

enum class GameWinChoice {
    NONE,
    REPLAY,
    QUIT
};

class GameWinMenu {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int selectedOption;
    float inputTimer;
    const float inputCooldown = 0.2f;

    GameWinChoice currentChoice;
    bool isTransitioning = false;
public:
    GameWinMenu(SDL_Renderer* ren, TTF_Font* fnt);
    ~GameWinMenu();


    void HandleInput();
    void Update(float deltaTime);
    void Render();
    GameWinChoice GetChoice() const { return currentChoice; }
    void Reset();
    void SetTransitioning(bool value) { isTransitioning = value; }
    bool IsTransitioning() const { return isTransitioning; }
};