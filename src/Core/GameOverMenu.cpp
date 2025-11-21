#include "GameOverMenu.h"
#include <iostream>
#include <glm/glm.hpp> 
#include "../Config/GameConstants.h"


// TẠO TEXTURE TỪ FONT VÀ TEXT VÀ VẼ LÊN RENDERER
static void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
    glm::vec2 pos, SDL_Color color, bool center = false)
{
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_DestroySurface(surface);
        return;
    }

    // THIẾT LẬP KHUNG HÌNH CHỮ NHẬT
    SDL_FRect dstRect;
    dstRect.x = pos.x;
    dstRect.y = pos.y;
    dstRect.w = (float)surface->w;
    dstRect.h = (float)surface->h;

    // XỬ LÝ CĂN GIỮA
    if (center) {
        dstRect.x = pos.x - dstRect.w / 2.0f;
        dstRect.y = pos.y - dstRect.h / 2.0f;
    }

    // VẼ TEXTURE LÊN MÀN HÌNH
    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}


GameOverMenu::GameOverMenu(SDL_Renderer* ren, TTF_Font* fnt)
    : renderer(ren), font(fnt) {

    Reset();
}

GameOverMenu::~GameOverMenu() {
}

// RESET MENU VỀ MẶC ĐỊNH
void GameOverMenu::Reset() {
    selectedOption = 0;
    inputTimer = inputCooldown;
    currentChoice = GameOverChoice::NONE;
    isTransitioning = false;
}


void GameOverMenu::Update(float deltaTime) {
    if (inputTimer > 0) {
        inputTimer -= deltaTime;
    }
}

// XỬ LÝ PHÍM BẤM
void GameOverMenu::HandleInput() {
    if (isTransitioning) return;
    if (inputTimer > 0) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    // ĐIỀU HƯỚNG LÊN
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
        if (selectedOption > 0) {
            selectedOption = 0;
            inputTimer = inputCooldown;
        }
    }
    // ĐIỀU HƯỚNG XUỐNG
    else if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
        if (selectedOption < 1) {
            selectedOption = 1;
            inputTimer = inputCooldown;
        }
    }
    // PHÍM CHỌN 
    else if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_J]) {
        if (selectedOption == 0) {
            currentChoice = GameOverChoice::REPLAY;
        }
        else if (selectedOption == 1) {
            currentChoice = GameOverChoice::QUIT;
        }
        inputTimer = inputCooldown;
    }
}

void GameOverMenu::Render() {
    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float startY = GameConstants::LOGICAL_HEIGHT / 2.0f;

    // THÔNG SỐ NÚT BẤM
    float buttonWidth = 200.0f;
    float buttonHeight = 35.0f;
    float spacing = 55.0f;
    
    // ĐỊNH NGHĨA MÀU SẮC
    const SDL_Color borderColor = { 255, 255, 255, 255 };
    const SDL_Color selectionFillColor = { 0, 0, 0, 150 }; 
    const float paddingX = 6.0f;
    const float paddingY = 6.0f;

    SDL_Color colorTitle = { 255, 0, 0, 255 };
    SDL_Color colorDefault = { 255, 255, 255, 255 };
    SDL_Color colorSelected = { 255, 255, 0, 255 };

    // VẼ TIÊU ĐỀ
    RenderText(renderer, font, "GAME OVER", { centerX, startY - 75.0f }, colorTitle, true);

    // VẼ NÚT REPLAY
    glm::vec2 posReplay = { centerX, startY };
    SDL_Color colorReplayText = (selectedOption == 0) ? colorSelected : colorDefault;

    SDL_FRect frameReplay = {
        posReplay.x - buttonWidth / 2.0f - paddingX,
        posReplay.y - buttonHeight / 2.0f - paddingY,
        buttonWidth + paddingX * 2.0f,
        buttonHeight + paddingY * 2.0f
    };

    // KIỂM TRA ĐANG CHỌN NÚT
    if (selectedOption == 0) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, selectionFillColor.r, selectionFillColor.g, selectionFillColor.b, selectionFillColor.a);
        SDL_RenderFillRect(renderer, &frameReplay);
    }

    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderRect(renderer, &frameReplay);

    // VẼ CHỮ REPLAY
    RenderText(renderer, font, "REPLAY", posReplay, colorReplayText, true);

    // VẼ NÚT QUIT
    glm::vec2 posQuit = { centerX, startY + spacing };
    SDL_Color colorQuitText = (selectedOption == 1) ? colorSelected : colorDefault;

    SDL_FRect frameQuit = {
        posQuit.x - buttonWidth / 2.0f - paddingX,
        posQuit.y - buttonHeight / 2.0f - paddingY,
        buttonWidth + paddingX * 2.0f,
        buttonHeight + paddingY * 2.0f
    };

    // KIỂM TRA ĐANG CHỌN NÚT
    if (selectedOption == 1) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, selectionFillColor.r, selectionFillColor.g, selectionFillColor.b, selectionFillColor.a);
        SDL_RenderFillRect(renderer, &frameQuit);
    }
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderRect(renderer, &frameQuit);
    // VẼ CHỮ QUIT
    RenderText(renderer, font, "QUIT", posQuit, colorQuitText, true);
}