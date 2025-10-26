#pragma once
#include "Character.h"
#include <SDL3_image/SDL_image.h>

enum class PlayerState {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING,
    STATE_JUMPING
};

class Player:public Character {
private:
    SDL_Texture* idleTex;
    SDL_Texture* walkTex;
    SDL_Texture* runTex;
    SDL_Texture* jumpTex;

    glm::vec2 playerPos;
    float playerVelocityX;
    float playerVelocityY;
    bool isOnGround;
    bool flipHorizontal;

    PlayerState currentState;
    PlayerState previousState;

public:
    Player(SDL_Renderer* renderer);
    ~Player();

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
};
