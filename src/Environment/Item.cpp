#include "Item.h"
#include "../Config/GameConstants.h"

Item::Item(glm::vec2 pos, SDL_Texture* tex, ItemType itemType)
    : position(pos),
    startPosition(pos),
    texture(tex),
    type(itemType),
    isCollected(false),
    currentFrame(0),
    animationTimer(0.0f),
    floatSpeed(3.0f),       
    floatAmplitude(5.0f)
{
    if (type == ItemType::COIN) {

        totalFrames = GameConstants::COIN_FRAMES;
        frameDuration = GameConstants::COIN_FRAME_DURATION;
        collider = { pos.x, pos.y, GameConstants::COIN_FRAME_WIDTH, GameConstants::COIN_FRAME_HEIGHT };
    }
    else if (type == ItemType::HEALTH_POTION) {
        totalFrames = GameConstants::HEALTH_POTION_FRAMES;
        frameDuration = GameConstants::HEALTH_POTION_FRAME_DURATION;
        collider = { pos.x, pos.y, GameConstants::HEALTH_POTION_FRAME_WIDTH, GameConstants::HEALTH_POTION_FRAME_HEIGHT };
    }
}

Item::~Item() {

}

void Item::Update(float deltaTime) {
    if (isCollected) return;

    animationTimer += deltaTime;
    if (animationTimer >= frameDuration && totalFrames > 0) {
        animationTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    // Float effect: position.y dao động theo sin
    floatOffset += floatSpeed * deltaTime;
    position.y = startPosition.y + sin(floatOffset) * floatAmplitude;

    // Cập nhật collider
    collider.x = position.x;
    collider.y = position.y;
}

void Item::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    if (isCollected || !texture) return;

    float srcFrameSize;   // Kích thước trong texture gốc
    float displaySize;    // Kích thước hiển thị trên màn hình

    if (type == ItemType::COIN) {
        srcFrameSize = 20.0f;
        displaySize = 20.0f;   // Coin giữ nguyên size
    }
    else if (type == ItemType::HEALTH_POTION) {
        srcFrameSize = 64.0f;  // Texture gốc 64x64
        displaySize = 24.0f;   // Hiển thị nhỏ lại (thử 24, 28, 32...)
    }

    // Vùng cắt từ texture gốc
    SDL_FRect srcRect = {
        (float)currentFrame * srcFrameSize,
        0.0f,
        srcFrameSize,
        srcFrameSize
    };

    // Vị trí vẽ trên màn hình với kích thước đã scale
    SDL_FRect dstRect = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        displaySize,   // Dùng displaySize thay vì srcFrameSize
        displaySize
    };

    SDL_RenderTexture(renderer, texture, &srcRect, &dstRect);

    // DEBUG: Vẽ Collider
    /*
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // Màu xanh Cyan
    SDL_FRect cRect{
        collider.x - cameraOffset.x,
        collider.y - cameraOffset.y,
        collider.w,
        collider.h
    };
    SDL_RenderRect(renderer, &cRect);
    */
}