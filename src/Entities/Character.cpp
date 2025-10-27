#include "Character.h"
#include <algorithm>


// Khoi tao mac dinh
Character::Character()
    : position(0.0f, 0.0f), velocity(0.0f, 0.0f), isOnGround(true),
    flipHorizontal(false), currentState(CharacterState::STATE_IDLE),
    previousState(CharacterState::STATE_IDLE), currentFrame(0),
    animationTimer(0.0f), idleTex(nullptr), walkTex(nullptr),
    runTex(nullptr), jumpTex(nullptr)
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

void Character::Update(float deltaTime) {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDir = 0;
    // Di chuyen trai phai
    if (keys[SDL_SCANCODE_A]) moveDir = -1;
    if (keys[SDL_SCANCODE_D]) moveDir = 1;
    // Chay nhanh
    bool isRunning = keys[SDL_SCANCODE_LSHIFT];

    // Nhay cao
    if (keys[SDL_SCANCODE_SPACE] && isOnGround) {
        velocity.y = -GameConstants::JUMP_SPEED;
        isOnGround = false;
    }

    // Trong luc
    velocity.y += GameConstants::GRAVITY * deltaTime;
    position.y += velocity.y * deltaTime;

    // Cham mat dat
    if (position.y + GameConstants::PLAYER_HEIGHT >= GameConstants::FLOOR_Y) {
        position.y = GameConstants::FLOOR_Y - GameConstants::PLAYER_HEIGHT;
        velocity.y = 0.0f;
        isOnGround = true;
    }

    //Cap nhat trang thai nhan vat
    if (!isOnGround)
        currentState = CharacterState::STATE_JUMPING;
    else if (moveDir)
        currentState = isRunning ? CharacterState::STATE_RUNNING : CharacterState::STATE_WALKING;
    else
        currentState = CharacterState::STATE_IDLE;

    float maxSpeed = isRunning ? GameConstants::RUN_SPEED : GameConstants::WALK_SPEED;
    if (moveDir)
        velocity.x += GameConstants::ACCELERATION * moveDir * deltaTime;
    else {
        if (velocity.x > 0) velocity.x = std::max(0.0f, velocity.x - GameConstants::DECELERATION * deltaTime);
        else if (velocity.x < 0) velocity.x = std::min(0.0f, velocity.x + GameConstants::DECELERATION * deltaTime);
    }

    // Gioi han toc do toi da
    velocity.x = std::max(-maxSpeed, std::min(velocity.x, maxSpeed));

    // Cap nhat vi tri
    position.x += velocity.x * deltaTime;

    // Gioi han trong bien ban do
    position.x = std::max(0.0f, std::min(position.x, GameConstants::WORLD_WIDTH - 48.0f));

    if (moveDir) flipHorizontal = (moveDir == -1);

    // Reset frame animation
    if (currentState != previousState) {
        currentFrame = 0;
        animationTimer = 0.0f;
    }
    previousState = currentState;

    int totalFrames = 0;
    float frameDuration = 0.0f;


    switch (currentState) {
    case CharacterState::STATE_IDLE: totalFrames = GameConstants::IDLE_FRAMES; frameDuration = GameConstants::IDLE_FRAME_DURATION; break;
    case CharacterState::STATE_WALKING: totalFrames = GameConstants::WALK_FRAMES; frameDuration = GameConstants::WALK_FRAME_DURATION; break;
    case CharacterState::STATE_RUNNING: totalFrames = GameConstants::RUN_FRAMES; frameDuration = GameConstants::RUN_FRAME_DURATION; break;
    case CharacterState::STATE_JUMPING: totalFrames = GameConstants::JUMP_FRAMES; frameDuration = GameConstants::JUMP_FRAME_DURATION; break;
    }

    // Cap nhat frame animation
    animationTimer += deltaTime;
    if (animationTimer >= frameDuration && totalFrames > 0) {
        animationTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % totalFrames;
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
    SDL_FRect src = { (float)currentFrame * frameWidth, 0.0f, (float)frameWidth, (float)frameHeight };
    // Vi tri ve tren man hinh
    SDL_FRect dst = { position.x - cameraOffset.x, position.y - cameraOffset.y, 48.0f, 48.0f };
    // Ve nhan vat
    SDL_RenderTextureRotated(renderer, tex, &src, &dst, 0.0, nullptr, flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void Character::SetPosition(const glm::vec2& pos) { position = pos; }
glm::vec2 Character::GetPosition() const { return position; }
