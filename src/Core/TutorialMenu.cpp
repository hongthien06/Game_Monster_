#include "TutorialMenu.h"
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

TutorialMenu::TutorialMenu(SDL_Renderer* ren, TTF_Font* fnt)
    : renderer(ren), font(fnt) {
    Reset();
}

TutorialMenu::~TutorialMenu() {
}

// RESET MENU VỀ MẶC ĐỊNH
void TutorialMenu::Reset() {
    selectedOption = 0;
    inputTimer = inputCooldown;
    currentChoice = TutorialChoice::NONE;
    isTransitioning = false;
}

void TutorialMenu::Update(float deltaTime) {
    if (inputTimer > 0) {
        inputTimer -= deltaTime;
    }
}

// XỬ LÝ PHÍM BẤM
void TutorialMenu::HandleInput() {
    if (isTransitioning) return;
    if (inputTimer > 0) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    // PHÍM CHỌN HOẶC THOÁT
    if (keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_J]) {
        currentChoice = TutorialChoice::BACK;
        inputTimer = inputCooldown;
    }
}

void TutorialMenu::Render() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect overlay = { 0.0f, 0.0f, (float)GameConstants::LOGICAL_WIDTH, (float)GameConstants::LOGICAL_HEIGHT };
    SDL_RenderFillRect(renderer, &overlay);

    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float startY = 90.0f;
    float lineHeight = 50.0f;
    float columnCenterOffset = 90.0f;

    float leftColumnX = centerX - columnCenterOffset;
    float rightColumnX = centerX + columnCenterOffset;

    // THÔNG SỐ NÚT BẤM
    float buttonWidth = 200.0f;
    float buttonHeight = 35.0f;

    // ĐỊNH NGHĨA MÀU SẮC
    SDL_Color colorTitle = { 255, 0, 0, 255 };
    SDL_Color colorLabel = { 255, 255, 255, 255 };
    SDL_Color colorSelected = { 255, 255, 0, 255 };

    const SDL_Color borderColor = { 255, 255, 255, 255 };
    const SDL_Color selectionFillColor = { 0, 0, 0, 150 };
    const float paddingX = 6.0f;
    const float paddingY = 6.0f;

    // VẼ TIÊU ĐỀ
    RenderText(renderer, font, "TUTORIAL", { centerX, 50.0f }, colorTitle, true);

    // VẼ CỘT TRÁI
    float currentY = startY;
    RenderText(renderer, font, "Move: A D", { leftColumnX, currentY }, colorLabel, true); currentY += lineHeight;
    RenderText(renderer, font, "Run: Shift", { leftColumnX, currentY }, colorLabel, true); currentY += lineHeight;
    RenderText(renderer, font, "Jump: Space", { leftColumnX, currentY }, colorLabel, true); currentY += lineHeight;

    // VẼ CỘT PHẢI
    currentY = startY;
    RenderText(renderer, font, "Healing: H", { rightColumnX, currentY }, colorLabel, true); currentY += lineHeight;
    RenderText(renderer, font, "Shot: K", { rightColumnX, currentY }, colorLabel, true); currentY += lineHeight;
    RenderText(renderer, font, "Dash: Ctrl", { rightColumnX, currentY }, colorLabel, true); currentY += lineHeight;

    // VẼ NÚT BACK
    glm::vec2 posBack = { centerX, GameConstants::LOGICAL_HEIGHT - 55.0f };

    SDL_FRect frameBack = {
        posBack.x - buttonWidth / 2.0f - paddingX,
        posBack.y - buttonHeight / 2.0f - paddingY,
        buttonWidth + paddingX * 2.0f,
        buttonHeight + paddingY * 2.0f
    };

    // KIỂM TRA ĐANG CHỌN NÚT 
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, selectionFillColor.r, selectionFillColor.g, selectionFillColor.b, selectionFillColor.a);
    SDL_RenderFillRect(renderer, &frameBack);

    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderRect(renderer, &frameBack);

    RenderText(renderer, font, "BACK", posBack, colorSelected, true);
}