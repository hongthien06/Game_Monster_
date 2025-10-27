#include "Player.h"
#include <algorithm>
#include <iostream>

//CONSTRUCTOR
Player::Player(SDL_Renderer* renderer, glm::vec2 startPos)
    : Character(renderer, startPos,
        "assets/images/Player/Idle.png",   
        "assets/images/Player/Walk.png",
        "assets/images/Player/Run.png",
        "assets/images/Player/Jump.png"),
    shotTex(nullptr), attackTex(nullptr), hurtTex(nullptr), deadTex(nullptr),
    playerState(PlayerState::STATE_IDLE),
    previousPlayerState(PlayerState::STATE_IDLE),
    playerCurrentFrame(0),
    playerAnimationTimer(0.0f),
    health(100),
    maxHealth(100),
    isAlive(true),
    isAttacking(false),
    canMove(true),
    attackCooldown(0.5f),
    attackTimer(0.0f),
    hurtDuration(0.3f),
    hurtTimer(0.0f)
{
    LoadAllTextures(renderer);
}

//  DESTRUCTOR 
Player::~Player() {
    // Giải phóng các texture bổ sung 
    if (shotTex) SDL_DestroyTexture(shotTex);
    if (attackTex) SDL_DestroyTexture(attackTex);
    if (hurtTex) SDL_DestroyTexture(hurtTex);
    if (deadTex) SDL_DestroyTexture(deadTex);
}

//  LOAD TEXTURES 
void Player::LoadAllTextures(SDL_Renderer* renderer) {
    shotTex = IMG_LoadTexture(renderer, "assets/images/Player/Shot.png");
    attackTex = IMG_LoadTexture(renderer, "assets/images/Player/Attack.png");
    hurtTex = IMG_LoadTexture(renderer, "assets/images/Player/Hurt.png");
    deadTex = IMG_LoadTexture(renderer, "assets/images/Player/Dead.png");

    // Kiểm tra lỗi load texture
    if (!shotTex || !attackTex || !hurtTex || !deadTex) {
        std::cerr << "ERROR: Khong the load mot hoac nhieu texture bo sung cua Player!\n";
        std::cerr << "Kiem tra duong dan: assets/images/Player/\n";
    }

    // Thiết lập scale mode
    if (shotTex) SDL_SetTextureScaleMode(shotTex, SDL_SCALEMODE_NEAREST);
    if (attackTex) SDL_SetTextureScaleMode(attackTex, SDL_SCALEMODE_NEAREST);
    if (hurtTex) SDL_SetTextureScaleMode(hurtTex, SDL_SCALEMODE_NEAREST);
    if (deadTex) SDL_SetTextureScaleMode(deadTex, SDL_SCALEMODE_NEAREST);
}

//  XỬ LÝ INPUT 
void Player::HandleInput() {
    // Không xử lý input nếu không thể di chuyển 
    if (!canMove) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    // Phím J: Tấn công cận chiến
    if (keys[SDL_SCANCODE_J]) {
        Attack();
    }

    // Phím K: Bắn tên
    if (keys[SDL_SCANCODE_K]) {
        Shot();
    }
}

//  CẬP NHẬT TRẠNG THÁI 
void Player::UpdatePlayerState(float deltaTime) {
    // Nếu đã chết, trạng thái cố định là DEAD
    if (!isAlive) {
        playerState = PlayerState::STATE_DEAD;
        canMove = false;
        return;
    }

    if (attackTimer > 0) {
        attackTimer -= deltaTime;
    }

    // Xử lý trạng thái HURT
    if (playerState == PlayerState::STATE_HURT) {
        hurtTimer -= deltaTime;
        if (hurtTimer <= 0) {
            canMove = true;
            playerState = PlayerState::STATE_IDLE;
        }
        return;
    }

    // Xử lý trạng thái ATTACK hoặc SHOT
    if (isAttacking) {
        int totalFrames = 0;
        if (playerState == PlayerState::STATE_ATTACK) {
            totalFrames = GameConstants::ATTACK_FRAMES;
        }
        else if (playerState == PlayerState::STATE_SHOT) {
            totalFrames = GameConstants::SHOT_FRAMES;
        }

        // Nếu đã chạy hết animation
        if (playerCurrentFrame >= totalFrames - 1) {
            isAttacking = false;
            canMove = true;
            playerState = PlayerState::STATE_IDLE;
        }
        return;
    }
    if (!isOnGround) {
        playerState = PlayerState::STATE_JUMP;
    }
    else {
        switch (currentState) {
        case CharacterState::STATE_IDLE:
            playerState = PlayerState::STATE_IDLE;
            break;
        case CharacterState::STATE_WALKING:
            playerState = PlayerState::STATE_WALK;
            break;
        case CharacterState::STATE_RUNNING:
            playerState = PlayerState::STATE_RUN;
            break;
        case CharacterState::STATE_JUMPING:
            playerState = PlayerState::STATE_JUMP;
            break;
        }
    }
}

void Player::UpdatePlayerAnimation(float deltaTime) {
    // Reset animation nếu trạng thái thay đổi
    if (playerState != previousPlayerState) {
        playerCurrentFrame = 0;
        playerAnimationTimer = 0.0f;
        previousPlayerState = playerState;
    }

    // Xác định số frame và tốc độ animation dựa trên trạng thái
    int totalFrames = 0;
    float frameDuration = 0.0f;

    switch (playerState) {
    case PlayerState::STATE_IDLE:
        totalFrames = GameConstants::IDLE_FRAMES;
        frameDuration = GameConstants::IDLE_FRAME_DURATION;
        break;
    case PlayerState::STATE_WALK:
        totalFrames = GameConstants::WALK_FRAMES;
        frameDuration = GameConstants::WALK_FRAME_DURATION;
        break;
    case PlayerState::STATE_RUN:
        totalFrames = GameConstants::RUN_FRAMES;
        frameDuration = GameConstants::RUN_FRAME_DURATION;
        break;
    case PlayerState::STATE_JUMP:
        totalFrames = GameConstants::JUMP_FRAMES;
        frameDuration = GameConstants::JUMP_FRAME_DURATION;
        break;
    case PlayerState::STATE_ATTACK:
        totalFrames = GameConstants::ATTACK_FRAMES;
        frameDuration = GameConstants::ATTACK_FRAME_DURATION;
        break;
    case PlayerState::STATE_SHOT:
        totalFrames = GameConstants::SHOT_FRAMES;
        frameDuration = GameConstants::SHOT_FRAME_DURATION;
        break;
    case PlayerState::STATE_HURT:
        totalFrames = GameConstants::HURT_FRAMES;
        frameDuration = hurtDuration / 4.0f;
        break;
    case PlayerState::STATE_DEAD:
        totalFrames = GameConstants::DEAD_FRAMES;
        frameDuration = GameConstants::DEAD_FRAME_DURATION;
        break;
    }

    // Cập nhật animation timer
    playerAnimationTimer += deltaTime;

    // Chuyển frame khi đủ thời gian
    if (playerAnimationTimer >= frameDuration && totalFrames > 0) {
        playerAnimationTimer -= frameDuration;

        // Animation chết không lặp lại (dừng ở frame cuối)
        if (playerState == PlayerState::STATE_DEAD) {
            playerCurrentFrame = std::min(playerCurrentFrame + 1, totalFrames - 1);
        }
        else {
            playerCurrentFrame = (playerCurrentFrame + 1) % totalFrames;
        }
    }
}

// UPDATE TỔNG THỂ
void Player::Update(float deltaTime, Map &map) {
    // ✅ GỌI Character::Update() TRƯỚC để xử lý di chuyển
    if (canMove) {
        Character::Update(deltaTime, map);
    }
        

    // Xử lý input (attack, shot)
    HandleInput();

    // Cập nhật trạng thái Player (đồng bộ với Character)
    UpdatePlayerState(deltaTime);

    // Cập nhật animation
    UpdatePlayerAnimation(deltaTime);
}

//RENDER
void Player::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_Texture* currentTexture = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
    int totalFrames = 0;

    switch (playerState) {
    case PlayerState::STATE_IDLE:
        currentTexture = idleTex;
        frameWidth = GameConstants::IDLE_FRAME_WIDTH;
        frameHeight = GameConstants::IDLE_FRAME_HEIGHT;
        totalFrames = GameConstants::IDLE_FRAMES;
        break;
    case PlayerState::STATE_WALK:
        currentTexture = walkTex;
        frameWidth = GameConstants::WALK_FRAME_WIDTH;
        frameHeight = GameConstants::WALK_FRAME_HEIGHT;
        totalFrames = GameConstants::WALK_FRAMES;
        break;
    case PlayerState::STATE_RUN:
        currentTexture = runTex;
        frameWidth = GameConstants::RUN_FRAME_WIDTH;
        frameHeight = GameConstants::RUN_FRAME_HEIGHT;
        totalFrames = GameConstants::RUN_FRAMES;
        break;
    case PlayerState::STATE_JUMP:
        currentTexture = jumpTex;
        frameWidth = GameConstants::JUMP_FRAME_WIDTH;
        frameHeight = GameConstants::JUMP_FRAME_HEIGHT;
        totalFrames = GameConstants::JUMP_FRAMES;
        break;
    case PlayerState::STATE_SHOT:
        currentTexture = shotTex;
        frameWidth = GameConstants::SHOT_FRAME_WIDTH;
        frameHeight = GameConstants::SHOT_FRAME_HEIGHT;
        totalFrames = GameConstants::SHOT_FRAMES;
        break;
    case PlayerState::STATE_ATTACK:
        currentTexture = attackTex;
        frameWidth = GameConstants::ATTACK_FRAME_WIDTH;
        frameHeight = GameConstants::ATTACK_FRAME_HEIGHT;
        totalFrames = GameConstants::ATTACK_FRAMES;
        break;
    case PlayerState::STATE_HURT:
        currentTexture = hurtTex;
        frameWidth = GameConstants::HURT_FRAME_WIDTH;
        frameHeight = GameConstants::HURT_FRAME_HEIGHT;
        totalFrames = GameConstants::HURT_FRAMES;
        break;
    case PlayerState::STATE_DEAD:
        currentTexture = deadTex;
        frameWidth = GameConstants::DEAD_FRAME_WIDTH;
        frameHeight = GameConstants::DEAD_FRAME_HEIGHT;
        totalFrames = GameConstants::DEAD_FRAMES;
        break;
    }

    if (!currentTexture) {
        std::cerr << "ERROR: currentTexture is NULL for state " << (int)playerState << "\n";
        return;
    }

    int safeFrame = std::min(playerCurrentFrame, totalFrames - 1);

    // Tính toán vùng source (frame hiện tại trong sprite sheet)
    SDL_FRect srcRect = {
        (float)(safeFrame * frameWidth),
        0.0f,
        (float)frameWidth,
        (float)frameHeight
    };

    // Tính toán vùng destination (vị trí vẽ trên màn hình)
    SDL_FRect dstRect = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        48.0f,
        48.0f
    };

    // Vẽ sprite với flip nếu cần
    SDL_RenderTextureRotated(
        renderer,
        currentTexture,
        &srcRect,
        &dstRect,
        0.0,
        nullptr,
        flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE
    );
}

//  COMBAT METHODS 

void Player::TakeDamage(int damage) {
    if (!isAlive || playerState == PlayerState::STATE_HURT) return;

    health -= damage;

    if (health <= 0) {
        health = 0;
        isAlive = false;
        playerState = PlayerState::STATE_DEAD;
        canMove = false;
        std::cout << "Player da chet!\n";
    }
    else {
        // Chuyển sang trạng thái hurt
        playerState = PlayerState::STATE_HURT;
        hurtTimer = hurtDuration;
        canMove = false;
        std::cout << "Player nhan " << damage << " sat thuong! HP con lai:" << health << "\n";
    }
}

void Player::Attack() {
    if (attackTimer > 0 || !canMove || !isAlive) return;

    playerState = PlayerState::STATE_ATTACK;
    isAttacking = true;
    canMove = false;
    attackTimer = attackCooldown;

    std::cout << "Player tan cong can chien!\n";
}

void Player::Shot() {
    if (attackTimer > 0 || !canMove || !isAlive) return;

    playerState = PlayerState::STATE_SHOT;
    isAttacking = true;
    canMove = false;
    attackTimer = attackCooldown;

    std::cout << "Player ban ten!\n";
}

void Player::Heal(int amount) {
    if (!isAlive) return;

    health = std::min(health + amount, maxHealth);
    std::cout << "Player hoi " << amount << " HP! HP hien tai: " << health << "\n";
}