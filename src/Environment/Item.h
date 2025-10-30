#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>


enum class ItemType {
    COIN,
    HEALTH_POTION,
    MANA_POTION
 
};

class Item {
private:

    glm::vec2 position;
    SDL_Texture* texture;
    SDL_FRect collider;

    ItemType type;
    bool isCollected;

    // Thông số Animation 
    int totalFrames;
    int currentFrame;
    float frameDuration;
    float animationTimer;

public:

    Item(glm::vec2 pos, SDL_Texture* tex, ItemType itemType);
    ~Item();

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    // Getters cho logic va chạm trong ItemManager
    const SDL_FRect& GetCollider() const { return collider; }
    ItemType GetType() const { return type; }
    bool IsCollected() const { return isCollected; }
    void Collect() { isCollected = true; } // Đánh dấu đã nhặt
};