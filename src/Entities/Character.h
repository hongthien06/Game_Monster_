#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Audio.h"
#include "../Config/GameConstants.h"

enum class CharacterState {
    IDLE,
    WALKING,
    RUNNING,
    JUMPING
};

class Character {
protected:
    SDL_Texture* currentTexture;
    glm::vec2 position;
    glm::vec2 velocity;
    bool isOnGround;
    bool flipHorizontal;
    CharacterState state;
    int currentFrame;
    float animationTimer;

public:
    Character();
    Character(glm::vec2 startPos);
    virtual ~Character();

    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) = 0;

    void SetPosition(const glm::vec2& pos);
    glm::vec2 GetPosition() const;

    void SetVelocity(const glm::vec2& vel);
    glm::vec2 GetVelocity() const;

    void SetState(CharacterState newState);
    CharacterState GetState() const;
};
