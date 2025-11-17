#include "MainMenu.h"
#include <glm/glm.hpp>
#include "../Config/GameConstants.h"
#include <string>

void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
    glm::vec2 pos, SDL_Color color, bool center = false)
{
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FRect rect{ pos.x, pos.y, (float)surface->w, (float)surface->h };

    if (center) {
        rect.x -= rect.w / 2;
        rect.y -= rect.h / 2;
    }

    SDL_RenderTexture(renderer, texture, nullptr, &rect);
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

MainMenu::MainMenu(SDL_Renderer* ren, TTF_Font* fnt)
    : renderer(ren), font(fnt)
{
    Reset();
}

MainMenu::~MainMenu() {}

void MainMenu::Reset() {
    selectedOption = 0;
    inputTimer = inputCooldown;
    choice = MainMenuChoice::NONE;
}

void MainMenu::Update(float deltaTime) {
    inputTimer -= deltaTime;
}

void MainMenu::HandleInput() {
    if (inputTimer > 0) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    if ((keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) && selectedOption > 0) {
        selectedOption--;
        inputTimer = inputCooldown;
    }
    else if ((keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) && selectedOption < 1) {
        selectedOption++;
        inputTimer = inputCooldown;
    }
    else if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_J]) {
        if (selectedOption == 0)
            choice = MainMenuChoice::START_GAME;
        else
            choice = MainMenuChoice::QUIT;

        inputTimer = inputCooldown;
    }
}

void MainMenu::Render() {

    // nền mờ
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_FRect bg{ 0, 0, (float)GameConstants::SCREEN_WIDTH, (float)GameConstants::SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &bg);

    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float centerY = GameConstants::LOGICAL_HEIGHT / 2.0f;

    SDL_Color titleColor = { 255, 255, 0, 255 };
    SDL_Color playColor = (selectedOption == 0) ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 };
    SDL_Color quitColor = (selectedOption == 1) ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 };

    RenderText(renderer, font, "MY SDL GAME", { centerX, centerY - 70 }, titleColor, true);
    RenderText(renderer, font, "Start Game", { centerX, centerY }, playColor, true);
    RenderText(renderer, font, "Quit", { centerX, centerY + 40 }, quitColor, true);
}
