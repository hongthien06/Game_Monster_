#include "MainMenu.h"
#include <iostream>
#include <algorithm>
#include "../Config/GameConstants.h" 

void MainMenu::RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
    glm::vec2 pos, SDL_Color color, bool center)
{
    if (!font) return;
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color background_color = { 0, 0, 0, 0 }; 
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(),0,color);
    if (!surface) {
        std::cerr << "TTF_RenderText_Blended Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dstRect;
    dstRect.w = (float)surface->w;
    dstRect.h = (float)surface->h;

    // Tính toán vị trí
    if (center) {
        dstRect.x = pos.x - dstRect.w / 2.0f;
        dstRect.y = pos.y - dstRect.h / 2.0f;
    }
    else {
        dstRect.x = pos.x;
        dstRect.y = pos.y;
    }

    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}


MainMenu::MainMenu(SDL_Renderer* ren, TTF_Font* fnt)
    : renderer(ren), font(fnt) {
    SetupButtons();
    Reset();
}

MainMenu::~MainMenu() {
}

void MainMenu::Reset() {
    selectedOption = 0;
    inputTimer = inputCooldown;
    currentChoice = MainMenuChoice::NONE;
}

void MainMenu::SetupButtons() {
    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float startY = GameConstants::LOGICAL_HEIGHT / 2.5f;
    float spacing = 55.0f;
    float buttonWidth = 200.0f;
    float buttonHeight = 35.0f; 

    // START GAME
    buttons[0] = {
        "PLAY",
        { centerX, startY + 10.0f },
        buttonWidth, buttonHeight,
        MainMenuChoice::START_GAME
    };

    // OPTIONS
    buttons[1] = {
        "SETTINGS",
        { centerX, startY + 10.0f + spacing },
        buttonWidth, buttonHeight,
        MainMenuChoice::OPTIONS
    };

    // QUIT
    buttons[2] = {
        "EXIT",
        { centerX, startY + 10.0f + spacing * 2 },
        buttonWidth, buttonHeight,
        MainMenuChoice::QUIT
    };
}

void MainMenu::Update(float deltaTime) {
    if (inputTimer > 0) {
        inputTimer -= deltaTime;
    }
}

void MainMenu::HandleKeyboardInput() {
    if (inputTimer > 0) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    int maxOption = 2;

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
        selectedOption = (selectedOption - 1 + (maxOption + 1)) % (maxOption + 1);
        inputTimer = inputCooldown;
    }
    else if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
        selectedOption = (selectedOption + 1) % (maxOption + 1);
        inputTimer = inputCooldown;
    }
    else if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_J]) {
        currentChoice = buttons[selectedOption].choice;
        inputTimer = inputCooldown;
    }
}


void MainMenu::HandleMouseClick(float mouseX, float mouseY) {
    for (int i = 0; i < 3; ++i) {
        const auto& button = buttons[i];

        float buttonX = button.pos.x - button.width / 2.0f;
        float buttonY = button.pos.y - button.height / 2.0f;

        if (mouseX >= buttonX && mouseX <= buttonX + button.width &&
            mouseY >= buttonY && mouseY <= buttonY + button.height) {

            selectedOption = i; 
            currentChoice = button.choice;
            return;
        }
    }
}

void MainMenu::HandleMouseMotion(float mouseX, float mouseY) {
    for (int i = 0; i < 3; ++i) {
        const auto& button = buttons[i];

        float buttonX = button.pos.x - button.width / 2.0f;
        float buttonY = button.pos.y - button.height / 2.0f;

        if (mouseX >= buttonX && mouseX <= buttonX + button.width &&
            mouseY >= buttonY && mouseY <= buttonY + button.height) {

            selectedOption = i; 
            return;
        }
    }
}


void MainMenu::Render() {

    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float centerY = GameConstants::LOGICAL_HEIGHT / 2.0f;

    SDL_Color colorTitle = { 0, 255, 0, 255 }; 
    SDL_Color colorDefault = { 255, 255, 255, 255 }; 
    SDL_Color colorSelected = { 255, 255, 0, 255 };


    const SDL_Color selectionFillColor = { 0, 0, 0, 150 }; 
    const SDL_Color borderColor = { 255, 255, 255, 255 }; 
    const float paddingX = 6.0f; 
    const float paddingY = 6.0f;

  
    RenderText(renderer, font, "GAME MONSTER", { centerX, centerY - 100.0f }, colorTitle, true);

    for (int i = 0; i < 3; ++i) {
        const auto& button = buttons[i];

        SDL_Color textColor = (i == selectedOption) ? colorSelected : colorDefault; 

        SDL_FRect frameRect = {
            button.pos.x - button.width / 2.0f - paddingX,
            button.pos.y - button.height / 2.0f - paddingY,
            button.width + paddingX * 2.0f,
            button.height + paddingY * 2.0f
        };

        if (i == selectedOption) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, selectionFillColor.r, selectionFillColor.g, selectionFillColor.b, selectionFillColor.a);
            SDL_RenderFillRect(renderer, &frameRect);
        }

        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderRect(renderer, &frameRect);

        RenderText(renderer, font, button.text, button.pos, textColor, true);
    }
}