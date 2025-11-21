#include "MovementSystem.h"
#include "../Config/GameConstants.h"
#include "../Entities/Character.h"
#include "PhysicsSystem.h"
#include <algorithm>
#include <iostream>

// XỬ LÝ LOGIC DI CHUYỂN NHÂN VẬT
void MovementSystem::HandleMovement(Character& character, float deltaTime, Map& map) {

    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDir = 0;

    // XỬ LÝ KĨ NĂNG DASH
    if (character.isDashing) {
        character.dashTimer -= deltaTime;
        
        // CỐ ĐỊNH VẬN TỐC KHI DASH
        character.velocity.x = character.dashDirection * GameConstants::DASH_SPEED;

        if (character.dashTimer <= 0.0f) {
            character.isDashing = false;
            character.dashCooldownTimer = GameConstants::DASH_COOLDOWN;
            character.currentState = CharacterState::STATE_IDLE;
        }

        // ÁP DỤNG VẬT LÝ VÀ KHÔNG XỬ LÝ ĐẦU VÀO KHÁC
        PhysicsSystem::ApplyPhysics(character.position, character.velocity, deltaTime, map, character.isOnGround);
        return;
    }

    // GIẢM THỜI GIAN HỒI CHIÊU DASH
    if (character.dashCooldownTimer > 0.0f)
        character.dashCooldownTimer -= deltaTime;

    // XỬ LÝ DI CHUYỂN
    if (keys[SDL_SCANCODE_A]) moveDir = -1;
    if (keys[SDL_SCANCODE_D]) moveDir = 1;
    character.isRunning = keys[SDL_SCANCODE_LSHIFT];

    // XỬ LÝ NHẢY
    if (keys[SDL_SCANCODE_SPACE] && character.isOnGround) {
        character.velocity.y = -GameConstants::JUMP_SPEED;
        character.isOnGround = false;
    }

    // XỬ LÝ DASH
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

    // XÁC ĐỊNH TỐC ĐỘ DỰA THEO TRẠNG THÁI
    float maxSpeed = character.isRunning ? GameConstants::RUN_SPEED : GameConstants::WALK_SPEED;

    if (moveDir)
        character.velocity.x += GameConstants::ACCELERATION * moveDir * deltaTime;
    else {
        if (character.velocity.x > 0)
            character.velocity.x = std::max(0.0f, character.velocity.x - GameConstants::DECELERATION * deltaTime);
        else if (character.velocity.x < 0)
            character.velocity.x = std::min(0.0f, character.velocity.x + GameConstants::DECELERATION * deltaTime);
    }

    // GIỚI HẠN VẬN TỐC
    character.velocity.x = std::max(-maxSpeed, std::min(character.velocity.x, maxSpeed));

    // LẬT HÌNH
    if (moveDir)
        character.flipHorizontal = (moveDir == -1);

    // LƯU TRẠNG THÁI TRƯỚC
    character.wasOnGround = character.isOnGround;

    // ÁP DỤNG VẬT LÝ TRỌNG LỰC, VA CHẠM VỚI BẢN ĐỒ
    PhysicsSystem::ApplyPhysics(character.position, character.velocity, deltaTime, map, character.isOnGround);
}
