#include "Player.h"
#include <algorithm>
#include <iostream>


// Khoi tao nhan vat
Player::Player(SDL_Renderer* renderer)
    : Character(glm::vec2(50.0f, GameConstants::FLOOR_Y)),
    flipHorizontal(false),
    currentState(PlayerState::STATE_IDLE),
    previousState(PlayerState::STATE_IDLE)
{
    // Tai cac texture nhan vat
    idleTex = IMG_LoadTexture(renderer, "assets/images/Player/Archer/Idle.png");
    walkTex = IMG_LoadTexture(renderer, "assets/images/Player/Archer/Walk.png");
    runTex = IMG_LoadTexture(renderer, "assets/images/Player/Archer/Run.png");
    jumpTex = IMG_LoadTexture(renderer, "assets/images/Player/Archer/Jump.png");

    if (!idleTex || !walkTex || !runTex || !jumpTex)
        std::cerr << "Error loading player textures!\n";

    // Hien thi anh ro net
    SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(walkTex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(runTex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(jumpTex, SDL_SCALEMODE_NEAREST);
}


// Giai phong tai nguyen nhan vat 
Player::~Player() {
    SDL_DestroyTexture(idleTex);
    SDL_DestroyTexture(walkTex);
    SDL_DestroyTexture(runTex);
    SDL_DestroyTexture(jumpTex);
}

// Cap nhat gameplay
void Player::Update(float deltaTime) {
    SDL_PumpEvents();
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDir = 0;

    // Phim dau vao 
    if (keys[SDL_SCANCODE_A]) moveDir = -1;
    if (keys[SDL_SCANCODE_D]) moveDir = 1;
    bool isRunning = keys[SDL_SCANCODE_LSHIFT];

    if (keys[SDL_SCANCODE_SPACE] && isOnGround) {
        velocity.y = -GameConstants::JUMP_SPEED;
        isOnGround = false;
    }

    // Trong luc
    velocity.y += GameConstants::GRAVITY * deltaTime;
    position.y += velocity.y * deltaTime;

    // Gioi han nhan vat ko roi xuyen dat
    if (position.y + GameConstants::PLAYER_HEIGHT >= GameConstants::FLOOR_Y) {
        position.y = GameConstants::FLOOR_Y - GameConstants::PLAYER_HEIGHT;
        velocity.y = 0.0f;
        isOnGround = true;
    }

    // Xac dinh trang thai nhan vat
    if (!isOnGround)
        currentState = PlayerState::STATE_JUMPING;
    else if (moveDir != 0)
        currentState = isRunning ? PlayerState::STATE_RUNNING : PlayerState::STATE_WALKING;
    else
        currentState = PlayerState::STATE_IDLE;

    // Tang giam toc do
    float maxSpeed = isRunning ? GameConstants::RUN_SPEED : GameConstants::WALK_SPEED;
    if (moveDir)
        velocity.x += GameConstants::ACCELERATION * moveDir * deltaTime;
    else {
        if (velocity.x > 0)
            velocity.x = std::max(0.0f, velocity.x - GameConstants::DECELERATION * deltaTime);
        else if (velocity.x < 0)
            velocity.x = std::min(0.0f, velocity.x + GameConstants::DECELERATION * deltaTime);
    }

    // Gioi han toc do
    velocity.x = std::max(-maxSpeed, std::min(velocity.x, maxSpeed));
    position.x += velocity.x * deltaTime;

    // Gioi han man hinh
    position.x = std::max(0.0f, std::min(position.x, GameConstants::WORLD_WIDTH - 32.0f));

    // Huong nhin cua nhan vat
    if (moveDir != 0) flipHorizontal = (moveDir == -1);

    // Reset hoat anh khi doi trang thai
    if (currentState != previousState) {
        currentFrame = 0;
        animationTimer = 0.0f;
    }
    previousState = currentState;

    // Cap nhat hoat anh
    int totalFrames = 0;
    float frameDuration = 0.0f;
    switch (currentState) {
    case PlayerState::STATE_IDLE:
        totalFrames = GameConstants::IDLE_FRAMES;
        frameDuration = GameConstants::IDLE_FRAME_DURATION;
        break;
    case PlayerState::STATE_WALKING:
        totalFrames = GameConstants::WALK_FRAMES;
        frameDuration = GameConstants::WALK_FRAME_DURATION;
        break;
    case PlayerState::STATE_RUNNING:
        totalFrames = GameConstants::RUN_FRAMES;
        frameDuration = GameConstants::RUN_FRAME_DURATION;
        break;
    case PlayerState::STATE_JUMPING:
        totalFrames = GameConstants::JUMP_FRAMES;
        frameDuration = 0.12f;
        break;
    }

    animationTimer += deltaTime;
    if (animationTimer >= frameDuration && totalFrames > 0) {
        animationTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % totalFrames;
    }
}


// Ve nhan vat  
void Player::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_Texture* tex = nullptr;
    int frameWidth = 0, frameHeight = 0;
    int totalFrames = 0;

    switch (currentState) {
    case PlayerState::STATE_IDLE:
        tex = idleTex;
        frameWidth = GameConstants::IDLE_FRAME_WIDTH;
        frameHeight = GameConstants::IDLE_FRAME_HEIGHT;
        totalFrames = GameConstants::IDLE_FRAMES;
        break;
    case PlayerState::STATE_WALKING:
        tex = walkTex;
        frameWidth = GameConstants::WALK_FRAME_WIDTH;
        frameHeight = GameConstants::WALK_FRAME_HEIGHT;
        totalFrames = GameConstants::WALK_FRAMES;
        break;
    case PlayerState::STATE_RUNNING:
        tex = runTex;
        frameWidth = GameConstants::RUN_FRAME_WIDTH;
        frameHeight = GameConstants::RUN_FRAME_HEIGHT;
        totalFrames = GameConstants::RUN_FRAMES;
        break;
    case PlayerState::STATE_JUMPING:
        tex = jumpTex;
        frameWidth = GameConstants::JUMP_FRAME_WIDTH;
        frameHeight = GameConstants::JUMP_FRAME_HEIGHT;
        totalFrames = GameConstants::JUMP_FRAMES;
        break;
    }

    if (!tex) return;

    SDL_FRect src = {
        (float)currentFrame * frameWidth,
        0.0f,
        (float)frameWidth,
        (float)frameHeight
    };

    SDL_FRect dst = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        32.0f, 32.0f
    };

    SDL_RenderTextureRotated(
        renderer,
        tex,
        &src,
        &dst,
        0.0,
        nullptr,
        flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE
    );
}
