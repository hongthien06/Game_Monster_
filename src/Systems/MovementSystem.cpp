#include "MovementSystem.h"
#include "../Config/GameConstants.h"
#include "../Entities/Character.h"
#include "PhysicsSystem.h"
#include <algorithm>
#include <iostream>

void MovementSystem::HandleMovement(Character& character, float deltaTime, Map& map) {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDir = 0;

    if (character.isDashing) {
        character.dashTimer -= deltaTime;
        character.velocity.x = character.dashDirection * GameConstants::DASH_SPEED;

        if (character.dashTimer <= 0.0f) {
            character.isDashing = false;
            character.dashCooldownTimer = GameConstants::DASH_COOLDOWN;
            character.currentState = CharacterState::STATE_IDLE;
        }

        PhysicsSystem::ApplyPhysics(character.position, character.velocity, deltaTime, map, character.isOnGround);
        return;
    }

    if (character.dashCooldownTimer > 0.0f)
        character.dashCooldownTimer -= deltaTime;

    if (keys[SDL_SCANCODE_A]) moveDir = -1;
    if (keys[SDL_SCANCODE_D]) moveDir = 1;
    bool isRunning = keys[SDL_SCANCODE_LSHIFT];

    if (keys[SDL_SCANCODE_SPACE] && character.isOnGround) {
        character.velocity.y = -GameConstants::JUMP_SPEED;
        character.isOnGround = false;
    }

    if (keys[SDL_SCANCODE_LCTRL] && character.dashCooldownTimer <= 0.0f) {
        if (moveDir != 0) {
            character.isDashing = true;
            character.dashDirection = moveDir;
            character.dashTimer = GameConstants::DASH_DURATION;
            character.dashCooldownTimer = GameConstants::DASH_COOLDOWN;
            character.currentState = CharacterState::STATE_DASHING;
            character.currentFrame = 0;
            character.animationTimer = 0.0f;
            return;
        }
    }

    float maxSpeed = isRunning ? GameConstants::RUN_SPEED : GameConstants::WALK_SPEED;

    if (moveDir)
        character.velocity.x += GameConstants::ACCELERATION * moveDir * deltaTime;
    else {
        if (character.velocity.x > 0)
            character.velocity.x = std::max(0.0f, character.velocity.x - GameConstants::DECELERATION * deltaTime);
        else if (character.velocity.x < 0)
            character.velocity.x = std::min(0.0f, character.velocity.x + GameConstants::DECELERATION * deltaTime);
    }

    character.velocity.x = std::max(-maxSpeed, std::min(character.velocity.x, maxSpeed));

    if (moveDir)
        character.flipHorizontal = (moveDir == -1);

    character.wasOnGround = character.isOnGround;

    PhysicsSystem::ApplyPhysics(character.position, character.velocity, deltaTime, map, character.isOnGround);
}
