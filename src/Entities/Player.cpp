#include "Player.h"
#include <algorithm>
#include "../Core/Audio.h"
#include "../Components/ScreenTransition.h"  // ===== THÊM MỚI =====
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
    // ===== LIVES SYSTEM =====
    lives(3),               // Bắt đầu với 3 mạng
    maxLives(3),            // Tối đa 3 mạng
    respawnPoint(startPos), // Điểm hồi sinh = điểm bắt đầu
    isDying(false),
    deathTimer(0.0f),
    deathDelay(2.0f),
    isGameOver(false),

    // FIX: KHỞI TẠO I-FRAMES
    iFramesDuration(1.2f),      // 1.2 giây bất tử
    iFramesTimer(0.0f),
    isFlashing(false),
    flashTimer(0.0f),
    shootCooldown(0.3f),
    shootTimer(0.0f),
    projectileDamage(30),
    projectileSpeed(600.0f),
    mouseWorldPos(0.0f, 0.0f),
    shouldSpawnArrow(false),
    arrowSpawnFrame(13),
    // ===== INVENTORY - KHỞI TẠO TÚI ĐỒ =====
    healthPotionCount(0),      // Bắt đầu không có potion
    maxHealthPotions(5)      // Tối đa 5 potion
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

    static bool hKeyWasPressed = false;
    bool hKeyPressed = keys[SDL_SCANCODE_H];

    if (hKeyPressed && !hKeyWasPressed) {
        UseHealthPotion();
    }

    hKeyWasPressed = hKeyPressed;
}

// ===== THÊM MỚI: SỬ DỤNG HEALTH POTION =====
void Player::UseHealthPotion() {
    // Kiểm tra điều kiện sử dụng
    if (!isAlive) {
        std::cout << "[Player] Khong the dung potion khi da chet!\n";
        return;
    }

    if (healthPotionCount <= 0) {
        std::cout << "[Player] Khong co Health Potion de dung!\n";
        //audio.playSound("assets/audio/error.wav");  // Optional: âm thanh lỗi
        return;
    }

    if (health >= maxHealth) {
        std::cout << "[Player] Mau da day, khong can dung potion!\n";
        return;
    }

    // Sử dụng potion
    healthPotionCount--;
    int healAmount = GameConstants::HEALTH_POTION_HEAL_AMOUNT;
    health = std::min(health + healAmount, maxHealth);

    std::cout << "[Player] Da su dung Health Potion! (" << healthPotionCount << " con lai) HP: "
        << health << "/" << maxHealth << "\n";

    // Phát âm thanh (nếu có)
    //audio.playSound("assets/audio/heal");
}

// ===== THÊM MỚI: THÊM POTION VÀO TÚI =====
void Player::AddHealthPotion() {
    if (healthPotionCount >= maxHealthPotions) {
        std::cout << "[Player] Tui da day! Khong the mang them potion.\n";
        return;
    }

    healthPotionCount++;
    std::cout << "[Player] Nhat duoc Health Potion! Hien co: "
        << healthPotionCount << "/" << maxHealthPotions << "\n";

    // Phát âm thanh nhặt đồ
    //audio.playSound("assets/audio/pickup.wav");
}

// ===== UPDATE PLAYER STATE =====
void Player::UpdatePlayerState(float deltaTime, bool wasOnGroundOld) {
    if (isDying) {
        deathTimer -= deltaTime;
        if (deathTimer <= 0) {
            Respawn();
        }
        return;
    }

    if (!isAlive) {
        playerState = PlayerState::STATE_DEAD;
        canMove = false;
        return;
    }

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

    // Chỉ phát âm thanh khi vừa chạm đất và cooldown hết, và player đang có vận tốc Y cao (bất kỳ)
    if (!wasOnGroundOld && isOnGround && landedSoundCooldown <= 0.0f) {
        /*std::cout << "Playing jump_landed sound! wasOnGroundOld=" << wasOnGroundOld << " isOnGround=" << isOnGround << "\n";*/
        audio.playSound("assets/audio/jump_landed.wav");
        landedSoundCooldown = 0.4f;  // Tăng cooldown thành 0.3s để tránh lặp
    }
    
    // DEBUG
    static float debugTimer = 0.0f;
    debugTimer += deltaTime;
    if (debugTimer > 0.5f) {
        /*std::cout << "wasOnGroundOld=" << wasOnGroundOld << " isOnGround=" << isOnGround << "\n";*/
        debugTimer = 0.0f;
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
// ===== CHECK FALL DEATH =====
void Player::CheckFallDeath() {
    // Kiểm tra nếu rơi xuống dưới WORLD_HEIGHT + 100
    if (position.y > GameConstants::WORLD_HEIGHT + 100.0f && !isDying && isAlive) {
        std::cout << "[Player] Roi khoi map! Mat 1 mang!\n";
        health = 0;  // Set máu = 0 trước
        LoseLife();  // Sau đó gọi LoseLife
    }
}

// ===== LOSE LIFE =====
void Player::LoseLife() {
    if (isDying) return;  // Đang chết rồi thì không xử lý nữa

    lives--;
    std::cout << "[Player] MAT 1 MANG! Con lai: " << lives << "/" << maxLives << "\n";

    if (lives <= 0) {
        // HẾT MẠNG - GAME OVER
        isAlive = false;
        playerState = PlayerState::STATE_DEAD;
        canMove = false;
        isGameOver = true;
        std::cout << "[Player] GAME OVER! Het mang!\n";
        // TODO: Có thể thêm màn hình Game Over ở đây
    }
    else {
        // CÒN MẠNG - CHUẨN BỊ RESPAWN
        isDying = true;
        deathTimer = deathDelay;
        playerState = PlayerState::STATE_DEAD;
        canMove = false;
        //audio.playSound("assets/audio/player_death.wav");
    }
}

void Player::Respawn() {
    std::cout << "[Player] HOI SINH tai (" << respawnPoint.x << ", " << respawnPoint.y << ")\n";

    // Reset vị trí
    position = respawnPoint;
    velocity = glm::vec2(0.0f, 0.0f);

    // Reset máu đầy
    health = maxHealth;  // Reset HP về max
    

    // Reset trạng thái
    isDying = false;
    isAlive = true;
    canMove = true;
    playerState = PlayerState::STATE_IDLE;

    // Kích hoạt I-frames sau khi hồi sinh
    iFramesTimer = iFramesDuration * 2.0f;  // Bất tử lâu hơn sau respawn
    //isFlashing = true;

    //audio.playSound("assets/audio/respawn.wav");
}

// ===== ADD LIFE =====
void Player::AddLife() {
    if (lives < maxLives) {
        lives++;
        std::cout << "[Player] NHAN THEM 1 MANG! Hien tai: " << lives << "/" << maxLives << "\n";
    }
}


// ===== MAIN UPDATE =====
void Player::Update(float deltaTime, Map& map) {
    // Lưu trạng thái cũ TRƯỚC KHI cập nhật
    bool wasOnGroundOld = isOnGround;
    
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

    CheckFallDeath();

    // Gọi Character::Update() để xử lý di chuyển
    if (canMove) {
        Character::Update(deltaTime, map);
    }

    //// Roi ra khoi ban do
    //if (isAlive && position.y > GameConstants::WORLD_HEIGHT + 100.0f) {
    //    std::cout << "[Player] Fell out of the world! DIED!\n";
    //    TakeDamage(999);
    //}

    // Xử lý input
    HandleInput();

    // Cập nhật trạng thái
    UpdatePlayerState(deltaTime, wasOnGroundOld);

    // Cập nhật animation
    UpdatePlayerAnimation(deltaTime);

    // Cập nhật projectiles
    UpdateProjectiles(deltaTime);
}

// ===== RENDER =====
void Player::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    if (isFlashing && (int)(SDL_GetTicks() / 100) % 2 == 0) {
        return;
    }


    // Render projectiles
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
    float screenWidth = (float)GameConstants::LOGICAL_WIDTH;

    if (isAlive) {
        DrawHealthBar(
            renderer,
            health,
            maxHealth,
            position,
            cameraOffset,
            45.0f,
            true
        );
    }

    // ===== VẼ SỐ MẠNG (LIVES) =====
    float liveHeartSize = 20.0f;
    float liveHeartSpacing = 24.0f;
    float liveHeartX = screenWidth - 80.0f;
    float liveHeartY = screenHeight - 285.0f;

    for (int i = 0; i < maxLives; i++) {
        float heartX = liveHeartX + i * liveHeartSpacing;
        int frameIndex = (i < lives) ? 0 : 1;

        SDL_FRect srcRect = {
            frameIndex * 16.0f,
            0.0f,
            16.0f,
            16.0f
        };

        SDL_FRect dstRect = {
            heartX,
            liveHeartY,
            liveHeartSize,
            liveHeartSize
        };

        SDL_RenderTexture(renderer, heartTexture, &srcRect, &dstRect);
    }

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
        LoseLife();
        //isAlive = false;
        /*playerState = PlayerState::STATE_DEAD;*/
        /*canMove = false;
        velocity = glm::vec2(0.0f);
        std::cout << "[Player] DIED!\n";*/
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

    // Phát âm thanh bay đi của mũi tên (âm lượng: 0.5)
    audio.playSound("assets/audio/ten_bay_ra.mp3", false, 0.5f);
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
    lives = maxLives;      
    isDying = false;       
    isGameOver = false;

    healthPotionCount = 0;  
    isAlive = true;

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