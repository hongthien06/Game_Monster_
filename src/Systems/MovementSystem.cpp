#include "MovementSystem.h"
#include "../Config/GameConstants.h"
#include "../Entities/Character.h"
#include "PhysicsSystem.h"
#include "AfterImageSystem.h"
#include <algorithm>
#include <iostream>

static std::vector<AfterImage> afterImages; // ✅ danh sách dư ảnh

void MovementSystem::HandleMovement(Character& character, float deltaTime, Map& map) {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDir = 0;

    // --- DASH đang hoạt động ---
    if (character.isDashing) {
        character.dashTimer -= deltaTime;
        character.velocity.x = character.dashDirection * GameConstants::DASH_SPEED;

        // === Thêm dư ảnh nhân vật ===
        SDL_Texture* tex = nullptr;
        int frameWidth = 0, frameHeight = 0;

        switch (character.currentState) {
        case CharacterState::STATE_IDLE:
            tex = character.idleTex;
            frameWidth = GameConstants::IDLE_FRAME_WIDTH;
            frameHeight = GameConstants::IDLE_FRAME_HEIGHT;
            break;
        case CharacterState::STATE_WALKING:
            tex = character.walkTex;
            frameWidth = GameConstants::WALK_FRAME_WIDTH;
            frameHeight = GameConstants::WALK_FRAME_HEIGHT;
            break;
        case CharacterState::STATE_RUNNING:
            tex = character.runTex;
            frameWidth = GameConstants::RUN_FRAME_WIDTH;
            frameHeight = GameConstants::RUN_FRAME_HEIGHT;
            break;
        case CharacterState::STATE_JUMPING:
            tex = character.jumpTex;
            frameWidth = GameConstants::JUMP_FRAME_WIDTH;
            frameHeight = GameConstants::JUMP_FRAME_HEIGHT;
            break;
        }

        if (tex) {
            SDL_FRect src = { (float)character.currentFrame * frameWidth, 0, (float)frameWidth, (float)frameHeight };

            SDL_FRect dst = {
                character.position.x,
                character.position.y - (frameHeight - GameConstants::LOGICAL_HEIGHT) / 2.0f,
                (float)frameWidth,
                (float)frameHeight
            };

            SDL_Color color = { 255, 230, 120, 200 }; // màu vàng nhạt mờ
            AfterImageSystem::AddImage(afterImages, tex, src, dst, color, character.flipHorizontal);
        }

        // --- kết thúc dash ---
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
            character.dashCooldownTimer = GameConstants::DASH_COOLDOWN; // 2 giây hồi

            // ✅ thêm dư ảnh ban đầu
            SDL_Texture* tex = character.runTex ? character.runTex : character.walkTex;
            if (tex) {
                SDL_FRect src = { (float)character.currentFrame * 48, 0, 48, 48 };
                SDL_FRect dst = {
                    character.position.x,
                    character.position.y - (48 - GameConstants::LOGICAL_HEIGHT) / 2.0f,
                    48, 48
                };
                SDL_Color color = { 255, 230, 120, 220 };
                AfterImageSystem::AddImage(afterImages, tex, src, dst, color, character.flipHorizontal);
            }

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

    // --- CẬP NHẬT DƯ ẢNH ---
    AfterImageSystem::Update(afterImages, deltaTime);
}

// === Render dư ảnh (có trừ offset camera) ===
void MovementSystem::RenderDashTrails(SDL_Renderer* renderer, const glm::vec2& offset) {
    AfterImageSystem::Render(renderer, afterImages, offset);
}
