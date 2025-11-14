#include "Player.h"
#include <algorithm>
#include "../Core/Audio.h"
#include <iostream>
#include <cmath>
#include "EntityUtils.h"

// ===== CONSTRUCTOR - THÊM KHỞI TẠO I-FRAMES =====
Player::Player(SDL_Renderer* renderer, glm::vec2 startPos)
    : Character(renderer, startPos,
        "assets/images/Player/Idle.png",
        "assets/images/Player/Walk.png",
        "assets/images/Player/Run.png",
        "assets/images/Player/Jump.png",
        "assets/images/Player/Dash.png",
        "assets/images/Player/Shot.png",
        "assets/images/Player/Attack.png",
        "assets/images/Player/Hurt.png",
        "assets/images/Player/Dead.png",
        "assets/hearts/hearts.png"),
    playerState(PlayerState::STATE_IDLE),
    previousPlayerState(PlayerState::STATE_IDLE),
    playerCurrentFrame(0),
    playerAnimationTimer(0.0f),
    isAlive(true),
    isAttacking(false),
    canMove(true),
    attackCooldown(0.5f),
    attackTimer(0.0f),
    hurtDuration(0.3f),
    hurtTimer(0.0f),
    // FIX: KHỞI TẠO I-FRAMES
    iFramesDuration(1.2f),      // 1.2 giây bất tử
    iFramesTimer(0.0f),
    isFlashing(false),
    flashTimer(0.0f),
    shootCooldown(0.3f),
    shootTimer(0.0f),
    projectileDamage(15),
    projectileSpeed(600.0f),
    mouseWorldPos(0.0f, 0.0f),
    shouldSpawnArrow(false),
    arrowSpawnFrame(13)
{
    // Không cần LoadAllTextures() nữa - đã load trong Character constructor
}

Player::~Player() {
    projectiles.clear();
}

// ===== HANDLE INPUT =====
void Player::HandleInput() {
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

// ===== UPDATE PLAYER STATE =====
void Player::UpdatePlayerState(float deltaTime) {
    /*if (!isAlive) {
        playerState = PlayerState::STATE_DEAD;
        canMove = false;
        return;
    }*/

    if (attackTimer > 0) {
        attackTimer -= deltaTime;
    }

    if (shootTimer > 0) {
        shootTimer -= deltaTime;
    }

    // FIX: Xử lý trạng thái HURT - CHỈ LOCK DI CHUYỂN, KHÔNG LOCK I-FRAMES
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

    // ===== HIỆU ỨNG CHẠM ĐẤT (CÓ COOL-DOWN) =====
    landedSoundCooldown -= deltaTime;

    if (!wasOnGround && isOnGround && landedSoundCooldown <= 0.0f) {
        audio.playSound("assets/audio/jump_landed.wav");
        landedSoundCooldown = 0.10f;
    }

    // Đồng bộ với Character state
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
        case CharacterState::STATE_DASHING:
            playerState = PlayerState::STATE_DASH;
            break;
        }
    }
}

// ===== UPDATE ANIMATION =====
void Player::UpdatePlayerAnimation(float deltaTime) {
    if (playerState != previousPlayerState) {
        playerCurrentFrame = 0;
        playerAnimationTimer = 0.0f;
        previousPlayerState = playerState;
    }

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
    case PlayerState::STATE_DASH:
        totalFrames = GameConstants::DASH_FRAMES;
        frameDuration = GameConstants::DASH_FRAME_DURATION;
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

    playerAnimationTimer += deltaTime;

    if (playerAnimationTimer >= frameDuration && totalFrames > 0) {
        playerAnimationTimer -= frameDuration;

        if (playerState == PlayerState::STATE_DEAD) {
            playerCurrentFrame = std::min(playerCurrentFrame + 1, totalFrames - 1);
        }
        else {
            playerCurrentFrame = (playerCurrentFrame + 1) % totalFrames;
        }

        if (playerState == PlayerState::STATE_SHOT &&
            playerCurrentFrame == arrowSpawnFrame) {
            SpawnArrow();
        }
    }
}

// ===== UPDATE PROJECTILES =====
void Player::UpdateProjectiles(float deltaTime) {
    for (auto& proj : projectiles) {
        proj->Update(deltaTime);
    }

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const std::unique_ptr<Projectile>& p) {
                return !p->IsActive();
            }),
        projectiles.end()
    );
}

// ===== MAIN UPDATE =====
void Player::Update(float deltaTime, Map& map) {
    // FIX: XỬ LÝ I-FRAMES - GIẢM DẦN TIMER
    if (iFramesTimer > 0) {
        iFramesTimer -= deltaTime;

        // Tạo hiệu ứng nhấp nháy
        flashTimer += deltaTime;
        if (flashTimer >= 0.1f) {
            isFlashing = !isFlashing;
            flashTimer = 0.0f;
        }
    }
    else {
        isFlashing = false;
    }

    // Gọi Character::Update() để xử lý di chuyển
    if (canMove) {
        Character::Update(deltaTime, map);
    }

    // Roi ra khoi ban do
    if (isAlive && position.y > GameConstants::WORLD_HEIGHT + 100.0f) {
        std::cout << "[Player] Fell out of the world! DIED!\n";
        TakeDamage(999);
    }

    // Xử lý input
    HandleInput();

    // Cập nhật trạng thái
    UpdatePlayerState(deltaTime);

    // Cập nhật animation
    UpdatePlayerAnimation(deltaTime);

    // Cập nhật projectiles
    UpdateProjectiles(deltaTime);
}

// ===== RENDER =====
void Player::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // FIX: NHẤP NHÁY KHI INVULNERABLE
    if (isFlashing && (int)(iFramesTimer * 10) % 2 == 0) {
        return;  // KHÔNG VẼ FRAME NÀY
    }

    // Render projectiles trước
    for (auto& proj : projectiles) {
        proj->Render(renderer, cameraOffset);
    }

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
    case PlayerState::STATE_DASH:
        currentTexture = dashTex;
        frameWidth = GameConstants::DASH_FRAME_WIDTH;
        frameHeight = GameConstants::DASH_FRAME_HEIGHT;
        totalFrames = GameConstants::DASH_FRAMES;
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

    float screenHeight = (float)GameConstants::LOGICAL_HEIGHT;

    DrawCornerHealthBar(
        renderer,
        health,
        maxHealth,
        8.0f,
        screenHeight - 25.0f,
        150.0f,
        20.0f,
        true,
        heartTexture
    );

    if (!currentTexture) return;

    int safeFrame = std::min(playerCurrentFrame, totalFrames - 1);

    SDL_FRect srcRect = {
        (float)(safeFrame * frameWidth),
        0.0f,
        (float)frameWidth,
        (float)frameHeight
    };

    SDL_FRect dstRect = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        48.0f,
        48.0f
    };

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

// ===== COMBAT: TAKE DAMAGE - FIX CHÍNH Ở ĐÂY =====
void Player::TakeDamage(int damage) {
    // FIX 1: CHECK I-FRAMES TRƯỚC - KHÔNG NHẬN SÁT THƯƠNG KHI BẤT TỬ
    if (!isAlive || iFramesTimer > 0) {
        std::cout << "[Player] Invulnerable! Remaining: " << iFramesTimer << "s\n";
        return;
    }

    health -= damage;
    std::cout << "[Player] Took " << damage << " damage! HP: " << health << "/" << maxHealth << "\n";

    if (health <= 0) {
        health = 0;
        isAlive = false;
        /*playerState = PlayerState::STATE_DEAD;*/
        canMove = false;
        velocity = glm::vec2(0.0f);
        std::cout << "[Player] DIED!\n";
        return;
    }

    // FIX 2: KÍCH HOẠT I-FRAMES SAU KHI BỊ ĐÁNH
    iFramesTimer = iFramesDuration;
    isFlashing = true;
    flashTimer = 0.0f;

    // FIX 3: CHUYỂN SANG STATE HURT (CHỈ ẢNH HƯỞNG DI CHUYỂN)
    playerState = PlayerState::STATE_HURT;
    hurtTimer = hurtDuration;
    canMove = false;

    // Optional: Phát âm thanh bị đánh
    audio.playSound("assets/audio/player_hurt.wav");
}

// ===== COMBAT: ATTACK =====
void Player::Attack() {
    if (attackTimer > 0 || !canMove || !isAlive) return;

    playerState = PlayerState::STATE_ATTACK;
    isAttacking = true;
    canMove = false;
    attackTimer = attackCooldown;

    std::cout << "Player tan cong can chien!\n";
}

// ===== COMBAT: SHOT =====
void Player::Shot() {
    if (shootTimer > 0 || !canMove || !isAlive) return;

    playerState = PlayerState::STATE_SHOT;
    isAttacking = true;
    canMove = false;
    shootTimer = shootCooldown;

    shouldSpawnArrow = true;

    std::cout << "Player bat dau len cung!\n";
}

// ===== SPAWN ARROW =====
void Player::SpawnArrow() {
    if (!shouldSpawnArrow) return;

    shouldSpawnArrow = false;

    std::cout << "Tao mui ten tai frame " << playerCurrentFrame << "!\n";

    float offsetX = flipHorizontal ? -15.0f : 15.0f;
    glm::vec2 spawnPos = position + glm::vec2(24.0f + offsetX, 36.0f);

    glm::vec2 direction = glm::vec2(flipHorizontal ? -1.0f : 1.0f, 0.0f);

    auto projectile = std::make_unique<Projectile>(
        renderer,
        spawnPos,
        direction,
        projectileSpeed,
        projectileDamage
    );

    projectile->SetTrailColor({ 100, 200, 255, 255 });
    projectiles.push_back(std::move(projectile));
}

// ===== HEAL =====
void Player::Heal(int amount) {
    if (!isAlive) return;

    health = std::min(health + amount, maxHealth);
    std::cout << "Player hoi " << amount << " HP! HP: " << health << "\n";
}

// ===== MOUSE HANDLING =====
void Player::UpdateMousePosition(float mouseScreenX, float mouseScreenY, glm::vec2 cameraOffset) {
    mouseWorldPos.x = mouseScreenX + cameraOffset.x;
    mouseWorldPos.y = mouseScreenY + cameraOffset.y;
}

void Player::HandleMouseClick(float mouseScreenX, float mouseScreenY, glm::vec2 cameraOffset) {
    UpdateMousePosition(mouseScreenX, mouseScreenY, cameraOffset);
    Shot();
}

// ===== GET BOUNDING BOX =====
SDL_FRect Player::GetBoundingBox() const {
    return SDL_FRect{
        position.x + 8.0f,
        position.y + 8.0f,
        32.0f,
        40.0f
    };
}

// ===== GET ARROW SPAWN POSITION =====
glm::vec2 Player::GetArrowSpawnPosition() const {
    return position + glm::vec2(24.0f, 24.0f);
}


void Player::Reset(glm::vec2 startPos) {
    position = startPos;
    isAlive = true;
    canMove = true;
    playerState = PlayerState::STATE_IDLE;
    previousPlayerState = PlayerState::STATE_IDLE;
    playerCurrentFrame = 0;

    health = maxHealth;

    velocity = glm::vec2(0.0f, 0.0f);

    attackTimer = 0.0f;
    shootTimer = 0.0f;
    hurtTimer = 0.0f;
    iFramesTimer = 0.0f;
    isFlashing = false;

    projectiles.clear();
}
void Player::SnapToGround(Map& map) {
    SDL_FRect bbox = GetBoundingBox();
    float originalY = position.y;
    position.y += 1.0f;

    SDL_FRect snapBox = GetBoundingBox();
    for (const auto& tile : map.GetCollisionTiles()) {
        if (SDL_HasRectIntersectionFloat(&snapBox, &tile)) {
            position.y = tile.y - snapBox.h - 0.01f; 
            velocity.y = 0.0f;
            isOnGround = true;
            return;
        }
    }

    position.y = originalY;
}