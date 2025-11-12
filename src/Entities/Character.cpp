#include "Character.h"
#include "..//Systems/MovementSystem.h"
#include "..//Systems/PhysicsSystem.h"
#include <algorithm>
#include "EntityUtils.h"
#include <iostream>

// ===== KHỞI TẠO MẶC ĐỊNH =====
Character::Character()
    : position(0.0f, 0.0f), velocity(0.0f, 0.0f), isOnGround(true), wasOnGround(false),
    flipHorizontal(false), currentState(CharacterState::STATE_IDLE),
    previousState(CharacterState::STATE_IDLE), currentFrame(0),
    animationTimer(0.0f),
    idleTex(nullptr), walkTex(nullptr), runTex(nullptr), jumpTex(nullptr),
    shotTex(nullptr), attackTex(nullptr), hurtTex(nullptr), deadTex(nullptr),
    heartTexture(nullptr),dashTex(nullptr),
    renderer(nullptr),
    maxHealth(100), health(100),
    isDashing(false),isRunning(false), dashTimer(0.0f), dashCooldownTimer(0.0f), dashDirection(0),
    landedSoundPlayed(false), landedSoundCooldown(0.0f)
{
}

// ===== KHỞI TẠO CÓ THAM SỐ - HỖ TRỢ 8 TEXTURES =====
Character::Character(SDL_Renderer* renderer, glm::vec2 startPos,
    const char* idlePath,
    const char* walkPath,
    const char* runPath,
    const char* jumpPath,
    const char* dashPath,
    const char* shotPath,
    const char* attackPath,
    const char* hurtPath,
    const char* deadPath,
    const char* heartPath)
    : position(startPos), velocity(0.0f, 0.0f), isOnGround(true), wasOnGround(false),
    flipHorizontal(false), currentState(CharacterState::STATE_IDLE),
    previousState(CharacterState::STATE_IDLE), currentFrame(0),
    animationTimer(0.0f),
    idleTex(nullptr), walkTex(nullptr), runTex(nullptr), jumpTex(nullptr), dashTex(nullptr),
    shotTex(nullptr), attackTex(nullptr), hurtTex(nullptr), deadTex(nullptr),
    heartTexture(nullptr),
    renderer(renderer),
    maxHealth(100), health(100),
    isDashing(false), isRunning(false), dashTimer(0.0f), dashCooldownTimer(0.0f), dashDirection(0),
    landedSoundPlayed(false), landedSoundCooldown(0.0f)
{
    // Load 4 texture bắt buộc
    LoadTexture(renderer, &idleTex, idlePath);
    LoadTexture(renderer, &walkTex, walkPath);
    LoadTexture(renderer, &runTex, runPath);
    LoadTexture(renderer, &jumpTex, jumpPath);
    LoadTexture(renderer, &dashTex, dashPath);

    // Load 4 texture tùy chọn (cho Player/Enemy)
    if (shotPath) LoadTexture(renderer, &shotTex, shotPath);
    if (attackPath) LoadTexture(renderer, &attackTex, attackPath);
    if (hurtPath) LoadTexture(renderer, &hurtTex, hurtPath);
    if (deadPath) LoadTexture(renderer, &deadTex, deadPath);

    LoadTexture(renderer, &heartTexture, heartPath);
}

// ===== PHƯƠNG THỨC HELPER LOAD TEXTURE =====
void Character::LoadTexture(SDL_Renderer* renderer, SDL_Texture** texture, const char* path) {
    if (!path) return;

    *texture = IMG_LoadTexture(renderer, path);

    if (!*texture) {
        std::cerr << "Loi load texture: " << path << "\n";
        return;
    }

    SDL_SetTextureScaleMode(*texture, SDL_SCALEMODE_NEAREST);
}

// ===== GIẢI PHÓNG BỘ NHỚ =====
Character::~Character() {
    SDL_DestroyTexture(idleTex);
    SDL_DestroyTexture(walkTex);
    SDL_DestroyTexture(runTex);
    SDL_DestroyTexture(jumpTex);
    SDL_DestroyTexture(dashTex);
    SDL_DestroyTexture(shotTex);
    SDL_DestroyTexture(attackTex);
    SDL_DestroyTexture(hurtTex);
    SDL_DestroyTexture(deadTex);
    SDL_DestroyTexture(heartTexture);

}

// ===== UPDATE =====
void Character::Update(float deltaTime, Map& map) {
    MovementSystem::HandleMovement(*this, deltaTime, map);

    CharacterState newState = currentState;

    if (isDashing) {
        newState = CharacterState::STATE_DASHING;
    }
    else if (!isOnGround) {
        newState = CharacterState::STATE_JUMPING;
    }
    else if (std::abs(velocity.x) > 1.0f) {
        newState = isRunning ? CharacterState::STATE_RUNNING
                             : CharacterState::STATE_WALKING;
    }
    else {
        newState = CharacterState::STATE_IDLE;
    }

   
    if (newState != currentState) {
        currentState = newState;
        currentFrame = 0;
        animationTimer = 0.0f;
    }


    // Cập nhật animation
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
    case CharacterState::STATE_DASHING:
        totalFrames = GameConstants::DASH_FRAMES;
        frameDuration = GameConstants::DASH_FRAME_DURATION;
        break;
    }

    animationTimer += deltaTime;
    if (animationTimer >= frameDuration && totalFrames > 0) {
        animationTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    // Kiểm tra rơi ra ngoài bản đồ
    if (position.y > GameConstants::WORLD_HEIGHT) {
        position = { 100.0f, 100.0f };
        velocity = { 0.0f, 0.0f };
        isOnGround = false;
    }
}

// ===== RENDER =====
void Character::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_Texture* tex = nullptr;
    int frameWidth = 0, frameHeight = 0, totalFrames = 0;

    // Chọn texture và thông tin frame dựa vào trạng thái
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
    case CharacterState::STATE_DASHING:
        tex = dashTex;
        frameWidth = GameConstants::DASH_FRAME_WIDTH;
        frameHeight = GameConstants::DASH_FRAME_HEIGHT;
        totalFrames = GameConstants::DASH_FRAMES;
        break;

    }

    if (!tex) return;

    // Xac dinh vung cat trong sprite sheet
    SDL_FRect src = { (float)currentFrame * frameWidth, 0.0f, (float)frameWidth, (float)frameHeight };
    // Vi tri ve tren man hinh
    SDL_FRect dst = { position.x - cameraOffset.x, position.y - cameraOffset.y, 48.0f, 48.0f };
    // Ve nhan vat
    SDL_RenderTextureRotated(renderer, tex, &src, &dst, 0.0, nullptr, flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void Character::SetPosition(const glm::vec2& pos) { position = pos; }
glm::vec2 Character::GetPosition() const { return position; }