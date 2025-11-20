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
    glm::vec2 startPosition;

    SDL_Texture* texture;
    SDL_FRect collider;

    ItemType type;

    //check nhặt item 
    bool isCollected;

    // Thông số Animation 
    int totalFrames;
    int currentFrame;
    float frameDuration;
    float animationTimer;

    float floatOffset;
    


public:
    Item(glm::vec2 pos, SDL_Texture* tex, ItemType itemType);
    ~Item();

    float floatSpeed;
    float floatAmplitude;

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

   
    const SDL_FRect& GetCollider() const { return collider; }
    ItemType GetType() const { return type; }
    bool IsCollected() const { return isCollected; }
    void Collect() { isCollected = true; } 
    float getFloatSpeed() {
        return this->floatSpeed;
    }
    float getFloatAmplitude() {
        return this->floatAmplitude;
    }
};