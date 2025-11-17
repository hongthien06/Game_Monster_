#include "MainMenu.h"
#include <iostream>
#include <algorithm>
#include "../Config/GameConstants.h" // Giả sử tồn tại file này

// Hàm RenderText (lấy từ GameOverMenu)
void MainMenu::RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
    glm::vec2 pos, SDL_Color color, bool center)
{
    if (!font) return;

    // Chuyển đổi màu từ SDL_Color sang Uint8 cho hàm TTF_RenderText_Blended
    SDL_Color black = { 0, 0, 0, 255 }; // Màu nền text
    // SỬA LỖI: Dùng 4 tham số (font, text, foreground, background)
    SDL_Color background_color = { 0, 0, 0, 0 }; // Background trong suốt (alpha = 0)
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
    float startY = GameConstants::LOGICAL_HEIGHT / 2.0f;
    float spacing = 50.0f;
    float buttonWidth = 250.0f;
    float buttonHeight = 35.0f; // Ước tính chiều cao text

    // START GAME
    buttons[0] = {
        "BAT DAU",
        { centerX, startY + 10.0f },
        buttonWidth, buttonHeight,
        MainMenuChoice::START_GAME
    };

    // OPTIONS
    buttons[1] = {
        "THIET LAP",
        { centerX, startY + 10.0f + spacing },
        buttonWidth, buttonHeight,
        MainMenuChoice::OPTIONS
    };

    // QUIT
    buttons[2] = {
        "THOAT GAME",
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

// Xử lý Input bằng bàn phím
void MainMenu::HandleKeyboardInput() {
    if (inputTimer > 0) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    int maxOption = 2; // 0, 1, 2

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

// Xử lý BẤM CHUỘT (Click)
void MainMenu::HandleMouseClick(float mouseX, float mouseY) {
    for (int i = 0; i < 3; ++i) {
        const auto& button = buttons[i];

        // Tính toán hộp va chạm (Hitbox) của nút
        float buttonX = button.pos.x - button.width / 2.0f;
        float buttonY = button.pos.y - button.height / 2.0f;

        // Kiểm tra xem chuột có nằm trong hitbox không
        if (mouseX >= buttonX && mouseX <= buttonX + button.width &&
            mouseY >= buttonY && mouseY <= buttonY + button.height) {

            selectedOption = i; // Cập nhật tùy chọn được chọn
            currentChoice = button.choice;
            return;
        }
    }
}

// Xử lý DI CHUỘT (Motion/Hover)
void MainMenu::HandleMouseMotion(float mouseX, float mouseY) {
    for (int i = 0; i < 3; ++i) {
        const auto& button = buttons[i];

        float buttonX = button.pos.x - button.width / 2.0f;
        float buttonY = button.pos.y - button.height / 2.0f;

        if (mouseX >= buttonX && mouseX <= buttonX + button.width &&
            mouseY >= buttonY && mouseY <= buttonY + button.height) {

            selectedOption = i; // Tô sáng nút khi di chuột qua
            return;
        }
    }
}


void MainMenu::Render() {
    // Không cần lớp phủ đen nếu đây là Main Menu (thường vẽ background riêng)
    // Nếu muốn giống Game Over Menu:
    /*
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect overlayRect = { 0.0f, 0.0f, (float)GameConstants::LOGICAL_WIDTH, (float)GameConstants::LOGICAL_HEIGHT };
    SDL_RenderFillRect(renderer, &overlayRect);
    */

    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float centerY = GameConstants::LOGICAL_HEIGHT / 2.0f;

    SDL_Color colorTitle = { 0, 255, 0, 255 }; // Xanh lá cây
    SDL_Color colorDefault = { 255, 255, 255, 255 }; // Trắng
    SDL_Color colorSelected = { 255, 255, 0, 255 }; // Vàng

    // Tiêu đề
    RenderText(renderer, font, "GAME TITLE", { centerX, centerY - 100.0f }, colorTitle, true);

    // Render các nút
    for (int i = 0; i < 3; ++i) {
        const auto& button = buttons[i];

        SDL_Color color = (i == selectedOption) ? colorSelected : colorDefault;

        // Vẽ Hitbox (chỉ để Debug)
        /*
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
        SDL_FRect debugRect = { button.pos.x - button.width / 2.0f, button.pos.y - button.height / 2.0f, button.width, button.height };
        SDL_RenderFillRect(renderer, &debugRect);
        */

        // Vẽ Text
        RenderText(renderer, font, button.text, button.pos, color, true);
    }
}