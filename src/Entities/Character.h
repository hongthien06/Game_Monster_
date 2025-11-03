#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include"..//Environment/Map.h"
#include "../Config/GameConstants.h"
#include <iostream>

enum class CharacterState {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING,
    STATE_JUMPING
};

class Character {
protected:
    SDL_Texture* idleTex;
    SDL_Texture* walkTex;
    SDL_Texture* runTex;
    SDL_Texture* jumpTex;

    glm::vec2 position;
    glm::vec2 velocity;

    bool isOnGround;
    bool wasOnGround;                     // Bien duoc them de xu li logic am thanh
    bool landedSoundPlayed = false;       // Bien duoc them de xu li logic am thanh
    float landedSoundCooldown = 0.0f;     // Bien duoc them de xu li logic am thanh
    bool flipHorizontal;

    CharacterState currentState;
    CharacterState previousState;

    int currentFrame;
    float animationTimer;

    int health;
    int maxHealth;

public:
    Character();
    Character(SDL_Renderer* renderer, glm::vec2 startPos,
        const char* idlePath,
        const char* walkPath,
        const char* runPath,
        const char* jumpPath);
    virtual ~Character();

    virtual void Update(float deltaTime, Map& map);
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    void SetPosition(const glm::vec2& pos);
    glm::vec2 GetPosition() const;

};
