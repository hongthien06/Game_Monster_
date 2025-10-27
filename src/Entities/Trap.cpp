#include <SDL3/SDL.h>
#include "Trap.h"
#include <algorithm>
#include <iostream>

using namespace std;

Trap::Trap(SDL_Renderer* renderer,SDL_Texture* texTure, float x, float y, int dir) 
 : renderer(renderer), trapTex(texTure), x(x), y(y), direction(dir)
{
   state = TRAP_IDLE;
   timer = 0.0f;

   frameWidth = 96;
   frameHeight = 32;

   currentFrame = 0;
   startFrame = 0;
   endFrame = 0;
   frameTime = 0.1f;   // Thoi gian giua moi frame animation
   frameTimer = 0.0f;
  
   chargeTime = 1.0f;  // Thoi gain charge truoc khi ban
   reloadTime = 2.0f;  // Thoi gian hoi chieu
}


// ======== STATE MANAGEMENT ========
void Trap::setState(TrapState newState)
{
    state = newState;
    timer = 0.0f;
    frameTimer = 0.0f;

    switch (state)
    {
        case TRAP_IDLE:
            startFrame = 0;
            endFrame = 3;
            break;

        case TRAP_CHARGE:
            startFrame = 4;
            endFrame = 7;
            timer = chargeTime;
            break;

        case TRAP_FIRE:
            startFrame = 8;
            endFrame = 8;
            timer = 0.1f; // chỉ hiển thị nhanh 1 khung bắn
            break;

        case TRAP_RELOAD:
            startFrame = 9;
            endFrame = 11;
            timer = reloadTime;
            break;

        case TRAP_DISABLED:
            startFrame = 12;
            endFrame = 12;
            break;

        case TRAP_BROKEN:
            startFrame = 13;
            endFrame = 13;
            break;
    }

    currentFrame = startFrame;
}

// ===================== UPDATE =====================
void Trap::update(float deltaTime)
{
    frameTimer += deltaTime;
    timer -= deltaTime;

    // Doi frame animation
    if (frameTimer >= frameTime)
    {
        frameTimer = 0.0f;
        currentFrame++;
        if (currentFrame > endFrame)
            currentFrame = startFrame;
    }

    // Logic chuyen trang thai
    switch (state)
    {
        case TRAP_IDLE:
            // ví dụ: Sau 3 giay ba
            timer += deltaTime;
            if (timer >= 3.0f)
                setState(TRAP_CHARGE);
            break;

        case TRAP_CHARGE:
            if (timer <= 0.0f)
                setState(TRAP_FIRE);
            break;

        case TRAP_FIRE:
            if (timer <= 0.0f)
                setState(TRAP_RELOAD);
            break;

        case TRAP_RELOAD:
            if (timer <= 0.0f)
                setState(TRAP_IDLE);
            break;

        default:
            break;
    }
}

// ===================== RENDER =====================
void Trap::render()
{
    SDL_FRect srcRect = {
        currentFrame * frameWidth,
        0,
        frameWidth,
        frameHeight
    };

    SDL_FRect dstRect = {
        x,
        y,
        (float)frameWidth,
        (float)frameHeight
    };

    SDL_RenderTextureRotated(
        renderer,
        trapTex,
        &srcRect,
        &dstRect,
        0.0,
        nullptr,
        direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE
    );
}