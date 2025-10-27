#pragma once
#include <SDL3/SDL.h>

//Enum dinh nghia trang thai cua Trap:
typedef enum {
    TRAP_IDLE,       // im lìm / chờ
    TRAP_CHARGE,     // chuẩn bị bắn (wind-up) -> animation báo hiệu
    TRAP_FIRE,       // frame bắn (vào thời điểm spawn arrow)
    TRAP_RELOAD,     // hồi chiêu / cooldown sau khi bắn
    TRAP_DISABLED,   // bị vô hiệu (ví dụ người chơi phá hỏng)
    TRAP_BROKEN      // phá hủy hoàn toàn (nếu cần)
} TrapState;

class Trap {
protected:
    SDL_Renderer* renderer;
    SDL_Texture* trapTex;   // chỉ 1 tấm sprite sheet
    TrapState state;
    float timer;

    // Animation
    int frameWidth; 
    int frameHeight;
    int currentFrame;
    int startFrame, endFrame; // vùng frame cho state hiện tại
    float frameTime, frameTimer;

    // Vị trí và hướng
    float x, y;
    int direction;

    // Logic
    float chargeTime, reloadTime;

public:
    Trap(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, int dir);
    void update(float deltaTime);
    void render();
    void setState(TrapState newState);
};
