#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <string>

// Các lựa chọn trong menu chính
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

    void HandleMouseClick(float mouseX, float mouseY);

    void HandleMouseMotion(float mouseX, float mouseY);
    void Render();

    MainMenuChoice GetChoice() const { return currentChoice; }
    void ResetChoice() { currentChoice = MainMenuChoice::NONE; } 

    void SetTransitioning(bool value) { isTransitioning = value; }
    bool IsTransitioning() const { return isTransitioning; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    float inputTimer;
    const float inputCooldown = 0.2f;
    int selectedOption; 
    MainMenuChoice currentChoice;
    struct ButtonArea {
        std::string text;
        glm::vec2 pos;
        float width;
        float height;
        MainMenuChoice choice;
    };

    ButtonArea buttons[3];
    void SetupButtons();

    void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
        glm::vec2 pos, SDL_Color color, bool center = false);

    bool isTransitioning = false;
};