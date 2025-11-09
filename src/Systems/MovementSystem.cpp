#include "MovementSystem.h"
#include "../Config/GameConstants.h"
#include "../Entities/Character.h"
#include "PhysicsSystem.h"
#include "TrailSystem.h"
#include <algorithm>
#include <iostream>

static std::vector<Trail> dashTrails; // danh sách vệt sáng toàn cục

void MovementSystem::HandleMovement(Character& character, float deltaTime, Map& map) {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDir = 0;

    // --- DASH đang hoạt động ---
    if (character.isDashing) {
        character.dashTimer -= deltaTime;
        character.velocity.x = character.dashDirection * GameConstants::DASH_SPEED;

        // Tạo vệt sáng vàng lướt theo nhân vật
        SDL_FRect rect = {
            character.position.x + (character.dashDirection == -1 ? 10 : 6), // lệch nhẹ theo hướng
            character.position.y + GameConstants::PLAYER_HEIGHT + 10,        // ngay dưới chân
        28, 8                                                            // mảnh, dài ngang chân
        };
        SDL_Color color = { 255, 240, 150, 200 }; // vàng sáng ấm, hơi trong suốt
        TrailSystem::AddTrail(dashTrails, rect, color);


        // Hết thời gian dash → ngừng
        if (character.dashTimer <= 0.0f) {
            character.isDashing = false;
            character.dashCooldownTimer = GameConstants::DASH_COOLDOWN;
        }

        PhysicsSystem::ApplyPhysics(character.position, character.velocity, deltaTime, map, character.isOnGround);
        return;
    }

    // --- GIẢM THỜI GIAN HỒI DASH ---
    if (character.dashCooldownTimer > 0.0f)
        character.dashCooldownTimer -= deltaTime;

    // --- NHẬN INPUT ---
    if (keys[SDL_SCANCODE_A]) moveDir = -1;
    if (keys[SDL_SCANCODE_D]) moveDir = 1;
    bool isRunning = keys[SDL_SCANCODE_LSHIFT];

    // --- NHẢY ---
    if (keys[SDL_SCANCODE_SPACE] && character.isOnGround) {
        character.velocity.y = -GameConstants::JUMP_SPEED;
        character.isOnGround = false;
    }

    // --- DASH ---
    if (keys[SDL_SCANCODE_LCTRL] && character.dashCooldownTimer <= 0.0f) {
        if (moveDir != 0) {
            character.isDashing = true;
            character.dashDirection = moveDir;
            character.dashTimer = GameConstants::DASH_DURATION;
            character.dashCooldownTimer = GameConstants::DASH_COOLDOWN;

            // Vệt sáng ban đầu khi vừa nhấn dash
            SDL_FRect rect = {
                character.position.x + 8,
                character.position.y + 12,
                14, 18
            };
            SDL_Color color = { 255, 220, 60, 220 }; // vàng tươi, alpha cao hơn 1 chút
            TrailSystem::AddTrail(dashTrails, rect, color);

            return;
        }
    }

    // --- VẬN TỐC TỐI ĐA ---
    float maxSpeed = isRunning ? GameConstants::RUN_SPEED : GameConstants::WALK_SPEED;

    // --- CẬP NHẬT VẬN TỐC ---
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

    // --- CẬP NHẬT VỆT SÁNG ---
    TrailSystem::Update(dashTrails, deltaTime);
}

void MovementSystem::RenderDashTrails(SDL_Renderer* renderer) {
    TrailSystem::Render(renderer, dashTrails);
}

