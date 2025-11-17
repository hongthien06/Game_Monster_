#include "Enemy.h"
#include "../Systems/MovementSystem.h"
#include "../Config/GameConstants.h"
#include <iostream>
#include <cmath>
#include "EntityUtils.h"

Enemy::Enemy()
    : Character(),
    enemyState(EnemyState::STATE_IDLE),
    previousEnemyState(EnemyState::STATE_IDLE),
    enemyType(EnemyType::MINION),
    enemyCurrentFrame(0),
    enemyAnimationTimer(0.0f),
    targetPosition(0.0f, 0.0f),
    aggroRange(250.0f),
    detectionRange(200.0f),
    attackRange(50.0f),
    attackCooldown(1.5f),
    attackTimer(0.0f),
    attackDamage(10),
    walkSpeed(40.0f),
    runSpeed(120.0f),
    patrolPointA(0.0f, 0.0f),
    patrolPointB(0.0f, 0.0f),
    movingToB(true),
    hurtDuration(0.3f),
    hurtTimer(0.0f),
    canTakeDamage(true),
    iFramesDuration(0.5f),
    iFramesTimer(0.0f),
    isAlive(true),
    deathTimer(0.0f),
    coinDropAmount(1),
    hasDroppedCoins(false),
    enemyCanJump(false),
    enemyIsOnGround(true),
    enemyGravity(GameConstants::GRAVITY),
    renderWidth(GameConstants::MINION_RENDER_WIDTH),
    renderHeight(GameConstants::MINION_RENDER_HEIGHT),
    hitboxWidth(GameConstants::MINION_HITBOX_WIDTH),
    hitboxHeight(GameConstants::MINION_HITBOX_HEIGHT),
    hitboxOffsetX(GameConstants::MINION_HITBOX_OFFSET_X),
    hitboxOffsetY(GameConstants::MINION_HITBOX_OFFSET_Y),
    initialPosition(0.0f, 0.0f),
    initialPatrolPointA(0.0f, 0.0f),
    initialPatrolPointB(0.0f, 0.0f)
{
}

Enemy::Enemy(SDL_Renderer* renderer, glm::vec2 startPos,
    const char* idlePath,
    const char* walkPath,
    const char* runPath,
    const char* jumpPath,
    const char* attackPath,
    const char* hurtPath,
    const char* deadPath,
    EnemyType type)
    : Character(renderer, startPos, idlePath, walkPath, runPath, jumpPath,
        nullptr, nullptr, attackPath, hurtPath, deadPath, nullptr),
    enemyState(EnemyState::STATE_IDLE),
    previousEnemyState(EnemyState::STATE_IDLE),
    enemyType(type),
    enemyCurrentFrame(0),
    enemyAnimationTimer(0.0f),
    targetPosition(0.0f, 0.0f),
    aggroRange(250.0f),
    detectionRange(200.0f),
    attackRange(50.0f),
    attackCooldown(1.5f),
    attackTimer(0.0f),
    attackDamage(10),
    walkSpeed(40.0f),
    runSpeed(120.0f),
    patrolPointA(startPos - glm::vec2(100.0f, 0.0f)),
    patrolPointB(startPos + glm::vec2(100.0f, 0.0f)),
    movingToB(true),
    hurtDuration(0.3f),
    hurtTimer(0.0f),
    canTakeDamage(true),
    iFramesDuration(0.5f),
    iFramesTimer(0.0f),
    isAlive(true),
    deathTimer(0.0f),
    coinDropAmount(1),
    hasDroppedCoins(false),
    enemyCanJump(false),
    enemyIsOnGround(true),
    enemyGravity(GameConstants::GRAVITY),
    renderWidth(48.0f),
    renderHeight(48.0f),
    initialPosition(startPos), // Ghi nhớ vị trí spawn chính xác
    initialPatrolPointA(startPos - glm::vec2(100.0f, 0.0f)), // Lưu điểm A ban đầu
    initialPatrolPointB(startPos + glm::vec2(100.0f, 0.0f))
{
    switch (type) {
    case EnemyType::MINION:
        maxHealth = 50;
        health = 50;
        attackDamage = 10;
        aggroRange = 200.0f;
        attackRange = 40.0f;
        coinDropAmount = 1;
        renderWidth = GameConstants::MINION_RENDER_WIDTH;
        renderHeight = GameConstants::MINION_RENDER_HEIGHT;
        hitboxWidth = GameConstants::MINION_HITBOX_WIDTH;
        hitboxHeight = GameConstants::MINION_HITBOX_HEIGHT;
        hitboxOffsetX = GameConstants::MINION_HITBOX_OFFSET_X;
        hitboxOffsetY = GameConstants::MINION_HITBOX_OFFSET_Y;
        break;
    case EnemyType::ELITE:
        maxHealth = 150;
        health = 150;
        attackDamage = 25;
        aggroRange = 300.0f;
        attackRange = 50.0f;
        runSpeed = 120.0f;
        coinDropAmount = 3;
        renderWidth = GameConstants::ELITE_RENDER_WIDTH;
        renderHeight = GameConstants::ELITE_RENDER_HEIGHT;
        hitboxWidth = GameConstants::ELITE_HITBOX_WIDTH;
        hitboxHeight = GameConstants::ELITE_HITBOX_HEIGHT;
        hitboxOffsetX = GameConstants::ELITE_HITBOX_OFFSET_X;
        hitboxOffsetY = GameConstants::ELITE_HITBOX_OFFSET_Y;
        break;
    case EnemyType::BOSS:
        maxHealth = 500;
        health = 500;
        attackDamage = 40;
        aggroRange = 400.0f;
        attackRange = 80.0f;
        coinDropAmount = 10;
        renderWidth = 96.0f;
        renderHeight = 96.0f;
        break;
    }
}

Enemy::~Enemy() {
}

// Kiểm tra khoảng cách
bool Enemy::IsTargetInAggroRange() const {
    return GetDistanceToTarget() <= aggroRange;
}

float Enemy::GetDistanceToTarget() const {
    glm::vec2 diff = targetPosition - position;
    return glm::length(diff);
}

bool Enemy::IsTargetInRange() const {
    return GetDistanceToTarget() <= detectionRange;
}

bool Enemy::IsTargetInAttackRange() const {
    return GetDistanceToTarget() <= attackRange;
}

void Enemy::SetPatrolPoints(glm::vec2 pointA, glm::vec2 pointB) {
    patrolPointA = pointA;
    patrolPointB = pointB;
    initialPatrolPointA = pointA;
    initialPatrolPointB = pointB;
}

// === 7 STATE HANDLERS ===
void Enemy::HandleIdle(float deltaTime) {
    velocity.x = 0.0f;
    // Enemy đứng yên, không làm gì cả
}

//void Enemy::HandleWalk(float deltaTime) {
//    glm::vec2 targetPoint = movingToB ? patrolPointB : patrolPointA;
//    glm::vec2 direction = targetPoint - position;
//    float distance = glm::length(direction);
//
//    if (distance < 5.0f) {
//        movingToB = !movingToB;
//        velocity.x = 0.0f;
//        return;
//    }
//
//    direction = glm::normalize(direction);
//    velocity.x = direction.x * walkSpeed;
//    flipHorizontal = velocity.x < 0;
//}
void Enemy::HandleWalk(float deltaTime) {
    // Không sử dụng nữa - enemy chỉ idle hoặc chase
    velocity.x = 0.0f;
}

void Enemy::HandleRun(float deltaTime) {
    glm::vec2 direction = targetPosition - position;
    float distance = glm::length(direction);

    if (distance > 0.1f) {
        direction = glm::normalize(direction);
        velocity.x = direction.x * runSpeed;
        flipHorizontal = velocity.x < 0;
    }
}

void Enemy::HandleJump(float deltaTime) {
    if (enemyCanJump && enemyIsOnGround) {
        velocity.y = -GameConstants::JUMP_SPEED * 0.7f;
        enemyIsOnGround = false;
    }
}

void Enemy::HandleAttack(float deltaTime) {
    if (attackTimer > 0) {
        attackTimer -= deltaTime;
    }

    velocity.x = 0.0f;

    if (attackTimer <= 0) {
        PerformAttack();
        attackTimer = attackCooldown;
    }
}

void Enemy::HandleHurt(float deltaTime) {
    hurtTimer -= deltaTime;
    velocity.x = 0.0f;

    if (hurtTimer <= 0) {
        canTakeDamage = true;
        enemyState = EnemyState::STATE_IDLE;
    }
}

// === STATE MACHINE (CHỈ 7 STATE) ===
void Enemy::UpdateEnemyState(float deltaTime, Map& map) {
    if (!isAlive) {
        enemyState = EnemyState::STATE_DEAD;
        velocity.x = 0.0f;
        return;
    }

    // I-frames
    if (iFramesTimer > 0) {
        iFramesTimer -= deltaTime;
    }

    // Hurt
    if (enemyState == EnemyState::STATE_HURT) {
        HandleHurt(deltaTime);
        return;
    }

    // AI: chuyển state theo khoảng cách
    if (IsTargetInAttackRange()) {
        enemyState = EnemyState::STATE_ATTACK;
        HandleAttack(deltaTime);
    }
    else if (IsTargetInAggroRange()) {
        enemyState = EnemyState::STATE_RUN;
        HandleRun(deltaTime);
    }
    else {
        // FIX: Khi không có target trong aggro range -> đứng yên
        enemyState = EnemyState::STATE_IDLE;
        HandleIdle(deltaTime);
    }
}

// === ANIMATION UPDATE (dùng GetFrameConfig từ lớp con) ===
void Enemy::UpdateEnemyAnimation(float deltaTime) {
    // Reset frame khi đổi state
    if (enemyState != previousEnemyState) {
        enemyCurrentFrame = 0;
        enemyAnimationTimer = 0.0f;
        previousEnemyState = enemyState;
    }

    // Lấy config từ lớp con
    FrameConfig cfg = GetFrameConfig(enemyState);

    enemyAnimationTimer += deltaTime;

    if (enemyAnimationTimer >= cfg.frameDuration && cfg.frameCount > 0) {
        enemyAnimationTimer -= cfg.frameDuration;

        if (enemyState == EnemyState::STATE_DEAD) {
            enemyCurrentFrame = std::min(enemyCurrentFrame + 1, cfg.frameCount - 1);
        }
        else {
            enemyCurrentFrame = (enemyCurrentFrame + 1) % cfg.frameCount;
        }
    }
}

// === UPDATE (physics riêng) ===
void Enemy::Update(float deltaTime, Map& map) {
    UpdateEnemyState(deltaTime, map);
    UpdateEnemyAnimation(deltaTime);

    if (enemyState != EnemyState::STATE_DEAD) {
        // Gravity
        if (!enemyIsOnGround) {
            velocity.y += enemyGravity * deltaTime;
        }

        // Di chuyển
        position += velocity * deltaTime;

        // Collision với map
        SDL_FRect bbox = GetBoundingBox();
        enemyIsOnGround = false;

        for (const auto& tile : map.GetCollisionTiles()) {
            if (SDL_HasRectIntersectionFloat(&bbox, &tile)) {
                if (velocity.y > 0) { // Đang rơi xuống và va chạm
                    // Đặt vị trí Y = mép trên của tile - chiều cao hitbox - offset Y của hitbox
                    position.y = tile.y - hitboxHeight - hitboxOffsetY; // <-- ĐÂY LÀ DÒNG ĐÃ SỬA

                    velocity.y = 0;
                    enemyIsOnGround = true;
                }
            }
        }
    }

    // Drop coins khi chết
    if (!isAlive) {
        deathTimer += deltaTime;

        if (!hasDroppedCoins && deathTimer >= 0.5f) {
            hasDroppedCoins = true;
            if (onDeathCallback) {
                onDeathCallback(position, coinDropAmount);
            }
        }
    }
}

// === RENDER (dùng GetFrameConfig) ===
void Enemy::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_Texture* currentTexture = nullptr;
    FrameConfig cfg = GetFrameConfig(enemyState);

    // Chọn texture theo state
    switch (enemyState) {
    case EnemyState::STATE_IDLE:
        currentTexture = idleTex;
        break;
    case EnemyState::STATE_WALK:
        currentTexture = walkTex;
        break;
    case EnemyState::STATE_RUN:
        currentTexture = runTex;
        break;
    case EnemyState::STATE_JUMP:
        currentTexture = jumpTex;
        break;
    case EnemyState::STATE_ATTACK:
        currentTexture = attackTex;
        break;
    case EnemyState::STATE_HURT:
        currentTexture = hurtTex;
        break;
    case EnemyState::STATE_DEAD:
        currentTexture = deadTex;
        break;
    }

    // Vẽ healthbar (chỉ khi sống)
    if (isAlive) {
        DrawHealthBar(renderer, health, maxHealth, position, cameraOffset, renderWidth);
    }

    if (!currentTexture) return;

    int safeFrame = std::min(enemyCurrentFrame, cfg.frameCount - 1);

    SDL_FRect srcRect = {
        (float)(safeFrame * cfg.frameWidth),
        0.0f,
        (float)cfg.frameWidth,
        (float)cfg.frameHeight
    };

    SDL_FRect dstRect = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        renderWidth,
        renderHeight
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

// === TAKE DAMAGE (với i-frames) ===
void Enemy::TakeDamage(int damage) {
    if (!isAlive || !canTakeDamage || iFramesTimer > 0) return;

    health -= damage;

    if (health <= 0) {
        health = 0;
        Die();
    }
    else {
        enemyState = EnemyState::STATE_HURT;
        hurtTimer = hurtDuration;
        canTakeDamage = false;
        iFramesTimer = iFramesDuration;
    }
}

void Enemy::Die() {
    isAlive = false;
    enemyState = EnemyState::STATE_DEAD;
    velocity = glm::vec2(0.0f, 0.0f);
}

// === HITBOX (nhỏ hơn sprite, có offset) ===
SDL_FRect Enemy::GetBoundingBox() const {
    return SDL_FRect{
        position.x + hitboxOffsetX,
        position.y + hitboxOffsetY,
        hitboxWidth,
        hitboxHeight
    };
}
void Enemy::ResetToStartPosition() {
    position = initialPosition;
    velocity = glm::vec2(0.0f, 0.0f);
    enemyIsOnGround = true; 

    health = maxHealth;
    isAlive = true;

    enemyState = EnemyState::STATE_IDLE;
    previousEnemyState = EnemyState::STATE_IDLE;
    enemyCurrentFrame = 0;
    enemyAnimationTimer = 0.0f;
    deathTimer = 0.0f;
    hasDroppedCoins = false; 
    canTakeDamage = true;
    iFramesTimer = 0.0f;

    patrolPointA = initialPatrolPointA;
    patrolPointB = initialPatrolPointB;
    movingToB = true;
}