#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <string>

enum class MainMenuChoice {
    NONE = 0,
    START_GAME,
    TUTORIAL,
    QUIT
};

class MainMenu {
public:
    MainMenu(SDL_Renderer* ren, TTF_Font* fnt);
    ~MainMenu();

    void Reset();
    void Update(float deltaTime);
    void HandleKeyboardInput();
    void Render();

    MainMenuChoice GetChoice() const { return currentChoice; }
    void ResetChoice() { currentChoice = MainMenuChoice::NONE; }

    void SetTransitioning(bool value) { isTransitioning = value; }
    bool IsTransitioning() const { return isTransitioning; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int selectedOption;
    float inputTimer;
    const float inputCooldown = 0.2f;
    MainMenuChoice currentChoice;
    bool isTransitioning = false;
};