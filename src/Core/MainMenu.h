#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

enum class MainMenuChoice {
    NONE,
    START_GAME,
    QUIT
};

class MainMenu {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int selectedOption;
    float inputTimer;
    const float inputCooldown = 0.2f;
    MainMenuChoice choice;

public:
    MainMenu(SDL_Renderer* ren, TTF_Font* fnt);
    ~MainMenu();

    void HandleInput();
    void Update(float deltaTime);
    void Render();
    MainMenuChoice GetChoice() const { return choice; }
    void Reset();
};
