#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <string>

enum class TutorialChoice {
    NONE = 0,
    BACK
};

class TutorialMenu {
public:
    TutorialMenu(SDL_Renderer* ren, TTF_Font* fnt);
    ~TutorialMenu();

    void Reset();
    void Update(float deltaTime);
    void HandleInput();
    void Render();

    TutorialChoice GetChoice() const { return currentChoice; }
    void ResetChoice() { currentChoice = TutorialChoice::NONE; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;

    int selectedOption;
    float inputTimer;
    const float inputCooldown = 0.2f;

    TutorialChoice currentChoice;

    void RenderText(const std::string& text, glm::vec2 pos, SDL_Color color, bool center = false);
};
