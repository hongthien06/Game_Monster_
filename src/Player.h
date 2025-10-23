#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include "GameConstants.h"

enum class PlayerState {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING,
    STATE_JUMPING
};

class Player {
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

    int currentFrame;
    float animationTimer;

public:
    Player(SDL_Renderer* renderer);
    ~Player();

    void SetPosition(glm::vec2 pos);
    glm::vec2 GetPosition() const;

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);
};
