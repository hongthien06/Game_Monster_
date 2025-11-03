#include "Character.h"
#include"..//Systems/PhysicsSystem.h"
#include <algorithm>
#include "EntityUtils.h"

// Khoi tao mac dinh
Character::Character()
    : position(0.0f, 0.0f), velocity(0.0f, 0.0f), isOnGround(true), wasOnGround(false),  /// 1 bien moi duoc them la wasOnGround
    flipHorizontal(false), currentState(CharacterState::STATE_IDLE),
    previousState(CharacterState::STATE_IDLE), currentFrame(0),
    animationTimer(0.0f), idleTex(nullptr), walkTex(nullptr),
    runTex(nullptr), jumpTex(nullptr),
    maxHealth(100), health(100)
{
}

// Khoi tao co tham so
Character::Character(SDL_Renderer* renderer, glm::vec2 startPos,
    const char* idlePath,
    const char* walkPath,
    const char* runPath,
    const char* jumpPath)
    : position(startPos), velocity(0.0f, 0.0f), isOnGround(true),
    flipHorizontal(false), currentState(CharacterState::STATE_IDLE),
    previousState(CharacterState::STATE_IDLE), currentFrame(0),
    animationTimer(0.0f)
{
    // Khoi tao cac texture cac trang thai
    idleTex = IMG_LoadTexture(renderer, idlePath);
    walkTex = IMG_LoadTexture(renderer, walkPath);
    runTex = IMG_LoadTexture(renderer, runPath);
    jumpTex = IMG_LoadTexture(renderer, jumpPath);

    if (!idleTex || !walkTex || !runTex || !jumpTex)
        std::cerr << "Error loading textures!\n";


    SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(walkTex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(runTex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(jumpTex, SDL_SCALEMODE_NEAREST);
}

// Giai phong bo nho
Character::~Character() {
    SDL_DestroyTexture(idleTex);
    SDL_DestroyTexture(walkTex);
    SDL_DestroyTexture(runTex);
    SDL_DestroyTexture(jumpTex);
}

void Character::Update(float deltaTime, Map& map) {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDir = 0;

    // Xu li phim
    if (keys[SDL_SCANCODE_A]) moveDir = -1;
    if (keys[SDL_SCANCODE_D]) moveDir = 1;
    bool isRunning = keys[SDL_SCANCODE_LSHIFT];
    if (keys[SDL_SCANCODE_SPACE] && isOnGround) {
        velocity.y = -GameConstants::JUMP_SPEED;
        isOnGround = false;
    }

    // Cap nhat van toc 
    float maxSpeed = isRunning ? GameConstants::RUN_SPEED : GameConstants::WALK_SPEED;

    if (moveDir)
        velocity.x += GameConstants::ACCELERATION * moveDir * deltaTime;
    else {
        if (velocity.x > 0) velocity.x = std::max(0.0f, velocity.x - GameConstants::DECELERATION * deltaTime);
        else if (velocity.x < 0) velocity.x = std::min(0.0f, velocity.x + GameConstants::DECELERATION * deltaTime);
    }

    // Gioi han toc do
    velocity.x = std::max(-maxSpeed, std::min(velocity.x, maxSpeed));

    // Cap nhat huong
    if (moveDir) flipHorizontal = (moveDir == -1);

    wasOnGround = isOnGround;      // Them dong nay de tao logic phat am thanh

    // Ap dung vat ly
    PhysicsSystem::ApplyPhysics(position, velocity, deltaTime, map, isOnGround);

    CharacterState newState;
    if (!isOnGround)
        newState = CharacterState::STATE_JUMPING;
    else if (std::abs(velocity.x) > 1.0f)
        newState = isRunning ? CharacterState::STATE_RUNNING : CharacterState::STATE_WALKING;
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
    case CharacterState::STATE_IDLE: totalFrames = GameConstants::IDLE_FRAMES; frameDuration = GameConstants::IDLE_FRAME_DURATION; break;
    case CharacterState::STATE_WALKING: totalFrames = GameConstants::WALK_FRAMES; frameDuration = GameConstants::WALK_FRAME_DURATION; break;
    case CharacterState::STATE_RUNNING: totalFrames = GameConstants::RUN_FRAMES; frameDuration = GameConstants::RUN_FRAME_DURATION; break;
    case CharacterState::STATE_JUMPING: totalFrames = GameConstants::JUMP_FRAMES; frameDuration = GameConstants::JUMP_FRAME_DURATION; break;
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
    case CharacterState::STATE_IDLE: tex = idleTex; frameWidth = GameConstants::IDLE_FRAME_WIDTH; frameHeight = GameConstants::IDLE_FRAME_HEIGHT; totalFrames = GameConstants::IDLE_FRAMES; break;
    case CharacterState::STATE_WALKING: tex = walkTex; frameWidth = GameConstants::WALK_FRAME_WIDTH; frameHeight = GameConstants::WALK_FRAME_HEIGHT; totalFrames = GameConstants::WALK_FRAMES; break;
    case CharacterState::STATE_RUNNING: tex = runTex; frameWidth = GameConstants::RUN_FRAME_WIDTH; frameHeight = GameConstants::RUN_FRAME_HEIGHT; totalFrames = GameConstants::RUN_FRAMES; break;
    case CharacterState::STATE_JUMPING: tex = jumpTex; frameWidth = GameConstants::JUMP_FRAME_WIDTH; frameHeight = GameConstants::JUMP_FRAME_HEIGHT; totalFrames = GameConstants::JUMP_FRAMES; break;
    }

    if (!tex) return;

    // Xac dinh vung cat trong sprite sheet
    SDL_FRect src = { 0.0, 0.0f, (float)frameWidth/6, (float)frameHeight/6 };
    // Vi tri ve tren man hinh
    SDL_FRect dst = { position.x - cameraOffset.x, position.y - cameraOffset.y, 0, 0};
    // Ve nhan vat
    SDL_RenderTextureRotated(renderer, tex, &src, &dst, 0.0, nullptr, flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

  
}

void Character::SetPosition(const glm::vec2& pos) { position = pos; }
glm::vec2 Character::GetPosition() const { return position; }
