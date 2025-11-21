#include "MainMenu.h"
#include <iostream>
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

MainMenu::MainMenu(SDL_Renderer* ren, TTF_Font* fnt)
    : renderer(ren), font(fnt) {
    Reset();
}

MainMenu::~MainMenu() {
}

// RESET MENU VỀ MẶC ĐỊNH
void MainMenu::Reset() {
    selectedOption = 0;
    inputTimer = inputCooldown;
    currentChoice = MainMenuChoice::NONE;
    isTransitioning = false;
}

void MainMenu::Update(float deltaTime) {
    if (inputTimer > 0) {
        inputTimer -= deltaTime;
    }
}

// XỬ LÝ PHÍM BẤM
void MainMenu::HandleKeyboardInput() {
    if (isTransitioning) return;
    if (inputTimer > 0) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    // ĐIỀU HƯỚNG LÊN
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
        if (selectedOption > 0) {
            selectedOption--;
            inputTimer = inputCooldown;
        }
    }
    // ĐIỀU HƯỚNG XUỐNG
    else if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
        if (selectedOption < 2) {
            selectedOption++;
            inputTimer = inputCooldown;
        }
    }
    // PHÍM CHỌN 
    else if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_J]) {
        if (selectedOption == 0) {
            currentChoice = MainMenuChoice::START_GAME;
        }
        else if (selectedOption == 1) {
            currentChoice = MainMenuChoice::TUTORIAL;
        }
        else if (selectedOption == 2) {
            currentChoice = MainMenuChoice::QUIT;
        }
        inputTimer = inputCooldown;
    }
}

void MainMenu::Render() {
    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float centerY = GameConstants::LOGICAL_HEIGHT / 2.0f;
    float startY = (GameConstants::LOGICAL_HEIGHT / 2.5f) + 10.0f;

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
    RenderText(renderer, font, "MONSTER SIGIE", { centerX, centerY - 90.0f }, colorTitle, true);

    // VẼ NÚT PLAY
    glm::vec2 posPlay = { centerX, startY };
    SDL_Color colorPlayText = (selectedOption == 0) ? colorSelected : colorDefault;

    SDL_FRect framePlay = {
        posPlay.x - buttonWidth / 2.0f - paddingX,
        posPlay.y - buttonHeight / 2.0f - paddingY,
        buttonWidth + paddingX * 2.0f,
        buttonHeight + paddingY * 2.0f
    };

    // KIỂM TRA ĐANG CHỌN NÚT
    if (selectedOption == 0) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, selectionFillColor.r, selectionFillColor.g, selectionFillColor.b, selectionFillColor.a);
        SDL_RenderFillRect(renderer, &framePlay);
    }
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderRect(renderer, &framePlay);

    // VẼ CHỮ PLAY
    RenderText(renderer, font, "PLAY", posPlay, colorPlayText, true);

    // VẼ NÚT TUTORIAL
    glm::vec2 posTutorial = { centerX, startY + spacing };
    SDL_Color colorTutorialText = (selectedOption == 1) ? colorSelected : colorDefault;

    SDL_FRect frameTutorial = {
        posTutorial.x - buttonWidth / 2.0f - paddingX,
        posTutorial.y - buttonHeight / 2.0f - paddingY,
        buttonWidth + paddingX * 2.0f,
        buttonHeight + paddingY * 2.0f
    };

    // KIỂM TRA ĐANG CHỌN NÚT
    if (selectedOption == 1) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, selectionFillColor.r, selectionFillColor.g, selectionFillColor.b, selectionFillColor.a);
        SDL_RenderFillRect(renderer, &frameTutorial);
    }
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderRect(renderer, &frameTutorial);

    // VẼ CHỮ TUTORIAL
    RenderText(renderer, font, "TUTORIAL", posTutorial, colorTutorialText, true);

    // VẼ NÚT EXIT
    glm::vec2 posExit = { centerX, startY + spacing * 2 };
    SDL_Color colorExitText = (selectedOption == 2) ? colorSelected : colorDefault;

    SDL_FRect frameExit = {
        posExit.x - buttonWidth / 2.0f - paddingX,
        posExit.y - buttonHeight / 2.0f - paddingY,
        buttonWidth + paddingX * 2.0f,
        buttonHeight + paddingY * 2.0f
    };

    // KIỂM TRA ĐANG CHỌN NÚT
    if (selectedOption == 2) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, selectionFillColor.r, selectionFillColor.g, selectionFillColor.b, selectionFillColor.a);
        SDL_RenderFillRect(renderer, &frameExit);
    }
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderRect(renderer, &frameExit);

    // VẼ CHỮ EXIT
    RenderText(renderer, font, "EXIT", posExit, colorExitText, true);
}