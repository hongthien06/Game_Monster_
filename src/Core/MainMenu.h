#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <string>

// Các lựa chọn trong menu chính
enum class MainMenuChoice {
    NONE = 0,
    START_GAME,
    OPTIONS,
    QUIT
};

class MainMenu {
public:
    MainMenu(SDL_Renderer* ren, TTF_Font* fnt);
    ~MainMenu();

    void Reset();
    void Update(float deltaTime);
    void HandleKeyboardInput();

    // Hàm xử lý sự kiện bấm chuột
    void HandleMouseClick(float mouseX, float mouseY);

    // Hàm xử lý di chuột để tô sáng (Hover)
    void HandleMouseMotion(float mouseX, float mouseY);

    void Render();

    MainMenuChoice GetChoice() const { return currentChoice; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    float inputTimer;
    const float inputCooldown = 0.2f;
    int selectedOption; // 0: Start, 1: Options, 2: Quit
    MainMenuChoice currentChoice;

    // Định nghĩa vị trí và kích thước của các nút (Hitbox)
    struct ButtonArea {
        std::string text;
        glm::vec2 pos;
        float width;
        float height;
        MainMenuChoice choice;
    };

    ButtonArea buttons[3];
    void SetupButtons();

    // Helper function (sẽ được định nghĩa trong .cpp)
    void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
        glm::vec2 pos, SDL_Color color, bool center = false);
};