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
    // else if (type == ItemType::HEALTH_POTION) { ... }
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

    float frameSize = 20.0f;

    // Vùng cắt
    SDL_FRect srcRect = { (float)currentFrame * frameSize, 0.0f, frameSize, frameSize };

    // Vị trí vẽ trên màn hình 
    SDL_FRect dstRect = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        frameSize,
        frameSize
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