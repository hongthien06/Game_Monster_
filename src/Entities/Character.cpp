#include "Character.h"
#include "..//Systems/MovementSystem.h"
#include "..//Systems/PhysicsSystem.h"
#include <algorithm>
#include "EntityUtils.h"
#include <iostream>

// Khoi tao mac dinh
Character::Character()
    : position(0.0f, 0.0f), velocity(0.0f, 0.0f), isOnGround(true), 
     wasOnGround(false), landedSoundPlayed(false), landedSoundCooldown(0.0f),  /// 3 bien moi duoc them la wasOnGround

    flipHorizontal(false), currentState(CharacterState::STATE_IDLE),
    previousState(CharacterState::STATE_IDLE), currentFrame(0),
    animationTimer(0.0f),
    idleTex(nullptr), walkTex(nullptr), runTex(nullptr), jumpTex(nullptr),
    shotTex(nullptr), attackTex(nullptr), hurtTex(nullptr), deadTex(nullptr),
    renderer(nullptr),
    maxHealth(100), health(100),
    isDashing(false), dashTimer(0.0f), dashCooldownTimer(0.0f), dashDirection(0)
{
}

// Khoi tao co tham so - HỖ TRỢ 8 TEXTURES
Character::Character(SDL_Renderer* renderer, glm::vec2 startPos,
    const char* idlePath,
    const char* walkPath,
    const char* runPath,
    const char* jumpPath,
    const char* shotPath,
    const char* attackPath,
    const char* hurtPath,
    const char* deadPath)
    : position(startPos), velocity(0.0f, 0.0f), isOnGround(true), wasOnGround(false),
    flipHorizontal(false), currentState(CharacterState::STATE_IDLE),
    previousState(CharacterState::STATE_IDLE), currentFrame(0),
    animationTimer(0.0f),
    idleTex(nullptr), walkTex(nullptr), runTex(nullptr), jumpTex(nullptr),
    shotTex(nullptr), attackTex(nullptr), hurtTex(nullptr), deadTex(nullptr),
    renderer(renderer),
    maxHealth(100), health(100),
    isDashing(false), dashTimer(0.0f), dashCooldownTimer(0.0f), dashDirection(0)
{
    // Load 4 texture bắt buộc
    LoadTexture(renderer, &idleTex, idlePath);
    LoadTexture(renderer, &walkTex, walkPath);
    LoadTexture(renderer, &runTex, runPath);
    LoadTexture(renderer, &jumpTex, jumpPath);

    // Load 4 texture tùy chọn (cho Player)
    if (shotPath) LoadTexture(renderer, &shotTex, shotPath);
    if (attackPath) LoadTexture(renderer, &attackTex, attackPath);
    if (hurtPath) LoadTexture(renderer, &hurtTex, hurtPath);
    if (deadPath) LoadTexture(renderer, &deadTex, deadPath);
}

// Phương thức helper load texture
void Character::LoadTexture(SDL_Renderer* renderer, SDL_Texture** texture, const char* path) {
    if (!path) return;

    *texture = IMG_LoadTexture(renderer, path);

    if (!*texture) {
        std::cerr << "Error loading texture: " << path << "\n";
        return;
    }

    SDL_SetTextureScaleMode(*texture, SDL_SCALEMODE_NEAREST);
}

// Giai phong bo nho
Character::~Character() {
    SDL_DestroyTexture(idleTex);
    SDL_DestroyTexture(walkTex);
    SDL_DestroyTexture(runTex);
    SDL_DestroyTexture(jumpTex);
    SDL_DestroyTexture(shotTex);
    SDL_DestroyTexture(attackTex);
    SDL_DestroyTexture(hurtTex);
    SDL_DestroyTexture(deadTex);
}

void Character::Update(float deltaTime, Map& map) {
    MovementSystem::HandleMovement(*this, deltaTime, map);

    CharacterState newState;
    if (!isOnGround)
        newState = CharacterState::STATE_JUMPING;
    else if (std::abs(velocity.x) > 1.0f)
        newState = CharacterState::STATE_WALKING;
    else
        newState = CharacterState::STATE_IDLE;

    if (newState != currentState) {
        currentState = newState;
        currentFrame = 0;
        animationTimer = 0.0f;
    }

    // Animation
    int totalFrames = 0;
    float frameDuration = 0.0f;

    switch (currentState) {
    case CharacterState::STATE_IDLE:
        totalFrames = GameConstants::IDLE_FRAMES;
        frameDuration = GameConstants::IDLE_FRAME_DURATION;
        break;
    case CharacterState::STATE_WALKING:
        totalFrames = GameConstants::WALK_FRAMES;
        frameDuration = GameConstants::WALK_FRAME_DURATION;
        break;
    case CharacterState::STATE_RUNNING:
        totalFrames = GameConstants::RUN_FRAMES;
        frameDuration = GameConstants::RUN_FRAME_DURATION;
        break;
    case CharacterState::STATE_JUMPING:
        totalFrames = GameConstants::JUMP_FRAMES;
        frameDuration = GameConstants::JUMP_FRAME_DURATION;
        break;
    }

    animationTimer += deltaTime;
    if (animationTimer >= frameDuration && totalFrames > 0) {
        animationTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    if (position.y > GameConstants::WORLD_HEIGHT) {
        position = { 100.0f, 100.0f };
        velocity = { 0.0f, 0.0f };
        isOnGround = false;
    }
}

void Character::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_Texture* tex = nullptr;
    int frameWidth = 0, frameHeight = 0, totalFrames = 0;

    // Chon texture va thong tin frame dua vao trang thai
    switch (currentState) {
    case CharacterState::STATE_IDLE:
        tex = idleTex;
        frameWidth = GameConstants::IDLE_FRAME_WIDTH;
        frameHeight = GameConstants::IDLE_FRAME_HEIGHT;
        totalFrames = GameConstants::IDLE_FRAMES;
        break;
    case CharacterState::STATE_WALKING:
        tex = walkTex;
        frameWidth = GameConstants::WALK_FRAME_WIDTH;
        frameHeight = GameConstants::WALK_FRAME_HEIGHT;
        totalFrames = GameConstants::WALK_FRAMES;
        break;
    case CharacterState::STATE_RUNNING:
        tex = runTex;
        frameWidth = GameConstants::RUN_FRAME_WIDTH;
        frameHeight = GameConstants::RUN_FRAME_HEIGHT;
        totalFrames = GameConstants::RUN_FRAMES;
        break;
    case CharacterState::STATE_JUMPING:
        tex = jumpTex;
        frameWidth = GameConstants::JUMP_FRAME_WIDTH;
        frameHeight = GameConstants::JUMP_FRAME_HEIGHT;
        totalFrames = GameConstants::JUMP_FRAMES;
        break;
    }

    if (!tex) return;

    // Xac dinh vung cat trong sprite sheet
    SDL_FRect src = {
     currentFrame * frameWidth,
     0.0f,
     (float)frameWidth,
     (float)frameHeight
    };
    // Vi tri ve tren man hinh
    SDL_FRect dst = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        48.0f,
        48.0f
    };
    // Ve nhan vat
    SDL_RenderTextureRotated(renderer, tex, &src, &dst, 0.0, nullptr,
        flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void Character::SetPosition(const glm::vec2& pos) { position = pos; }
glm::vec2 Character::GetPosition() const { return position; }