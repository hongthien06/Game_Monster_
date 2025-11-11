#include "Enemy.h"
#include "../Systems/MovementSystem.h"
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
    patrolSpeed(50.0f),
    walkSpeed(40.0f),
    chaseSpeed(100.0f),
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
    enemyGravity(GameConstants::GRAVITY)
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
    patrolSpeed(50.0f),
    walkSpeed(40.0f),
    chaseSpeed(100.0f),
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
    enemyGravity(GameConstants::GRAVITY)
{
    switch (type) {
    case EnemyType::MINION:
        maxHealth = 50;
        health = 50;
        attackDamage = 10;
        aggroRange = 200.0f;
        attackRange = 40.0f;
        coinDropAmount = 1;
        break;
    case EnemyType::ELITE:
        maxHealth = 150;
        health = 150;
        attackDamage = 25;
        aggroRange = 300.0f;
        attackRange = 50.0f;
        chaseSpeed = 120.0f;
        coinDropAmount = 3;
        break;
    case EnemyType::BOSS:
        maxHealth = 500;
        health = 500;
        attackDamage = 40;
        aggroRange = 400.0f;
        attackRange = 80.0f;
        coinDropAmount = 10;
        break;
    }
}

Enemy::~Enemy() {
}

// FIX: CHECK AGGRO RANGE RIÊNG
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
}

// FIX: HANDLER CHO TỪNG STATE
void Enemy::HandleIdle(float deltaTime) {
    velocity.x = 0.0f;
}

void Enemy::HandleWalk(float deltaTime) {
    glm::vec2 targetPoint = movingToB ? patrolPointB : patrolPointA;
    glm::vec2 direction = targetPoint - position;
    float distance = glm::length(direction);

    if (distance < 5.0f) {
        movingToB = !movingToB;
        velocity.x = 0.0f;
        return;
    }

    direction = glm::normalize(direction);
    velocity.x = direction.x * walkSpeed;

    flipHorizontal = velocity.x < 0;
}

void Enemy::HandlePatrol(float deltaTime) {
    HandleWalk(deltaTime);
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

void Enemy::HandleChase(float deltaTime) {
    glm::vec2 direction = targetPosition - position;
    float distance = glm::length(direction);

    if (distance > 0.1f) {
        direction = glm::normalize(direction);
        velocity.x = direction.x * chaseSpeed;
        flipHorizontal = velocity.x < 0;
    }
}

void Enemy::HandleJump(float deltaTime) {
    // CHỈ NHẢY KHI ĐƯỢC PHÉP VÀ ĐANG Ở MẶT ĐẤT
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

// FIX: STATE MACHINE LOGIC HOÀN CHỈNH
void Enemy::UpdateEnemyState(float deltaTime, Map& map) {
    if (!isAlive) {
        enemyState = EnemyState::STATE_DEAD;
        velocity.x = 0.0f;
        return;
    }

    // XỬ LÝ I-FRAMES
    if (iFramesTimer > 0) {
        iFramesTimer -= deltaTime;
    }

    // XỬ LÝ HURT
    if (enemyState == EnemyState::STATE_HURT) {
        HandleHurt(deltaTime);
        return;
    }

    // AI LOGIC - CHUYỂN STATE THEO ƯU TIÊN
    if (IsTargetInAttackRange()) {
        enemyState = EnemyState::STATE_ATTACK;
        HandleAttack(deltaTime);
    }
    else if (IsTargetInAggroRange()) {
        enemyState = EnemyState::STATE_RUN;
        HandleRun(deltaTime);
    }
    else if (IsTargetInRange()) {
        enemyState = EnemyState::STATE_CHASE;
        HandleChase(deltaTime);
    }
    else {
        enemyState = EnemyState::STATE_WALK;
        HandleWalk(deltaTime);
    }
}

void Enemy::UpdateEnemyAnimation(float deltaTime) {
    if (enemyState != previousEnemyState) {
        enemyCurrentFrame = 0;
        enemyAnimationTimer = 0.0f;
        previousEnemyState = enemyState;
    }

    int totalFrames = 0;
    float frameDuration = 0.0f;

    switch (enemyState) {
    case EnemyState::STATE_IDLE:
        totalFrames = GameConstants::IDLE_FRAMES;
        frameDuration = GameConstants::IDLE_FRAME_DURATION;
        break;
    case EnemyState::STATE_WALK:
    case EnemyState::STATE_PATROL:
        totalFrames = GameConstants::WALK_FRAMES;
        frameDuration = GameConstants::WALK_FRAME_DURATION;
        break;
    case EnemyState::STATE_RUN:
    case EnemyState::STATE_CHASE:
        totalFrames = GameConstants::RUN_FRAMES;
        frameDuration = GameConstants::RUN_FRAME_DURATION;
        break;
    case EnemyState::STATE_ATTACK:
        totalFrames = GameConstants::ATTACK_FRAMES;
        frameDuration = GameConstants::ATTACK_FRAME_DURATION;
        break;
    case EnemyState::STATE_HURT:
        totalFrames = GameConstants::HURT_FRAMES;
        frameDuration = hurtDuration / std::max(1, totalFrames);
        break;
    case EnemyState::STATE_DEAD:
        totalFrames = GameConstants::DEAD_FRAMES;
        frameDuration = GameConstants::DEAD_FRAME_DURATION;
        break;
    }

    enemyAnimationTimer += deltaTime;

    if (enemyAnimationTimer >= frameDuration && totalFrames > 0) {
        enemyAnimationTimer -= frameDuration;

        if (enemyState == EnemyState::STATE_DEAD) {
            enemyCurrentFrame = std::min(enemyCurrentFrame + 1, totalFrames - 1);
        }
        else {
            enemyCurrentFrame = (enemyCurrentFrame + 1) % totalFrames;
        }
    }
}

// FIX: DÙNG PHYSICS RIÊNG, KHÔNG GỌI MovementSystem
void Enemy::Update(float deltaTime, Map& map) {
    UpdateEnemyState(deltaTime, map);
    UpdateEnemyAnimation(deltaTime);

    if (enemyState != EnemyState::STATE_DEAD) {
        // FIX: DÙNG GRAVITY RIÊNG
        if (!enemyIsOnGround) {
            velocity.y += enemyGravity * deltaTime;
        }

        // DI CHUYỂN
        position += velocity * deltaTime;

        // CHECK COLLISION VỚI MAP
        SDL_FRect bbox = GetBoundingBox();

        // CHECK GROUND
        enemyIsOnGround = false;
        for (const auto& tile : map.GetCollisionTiles()) {
            if (SDL_HasRectIntersectionFloat(&bbox, &tile)) {
                // XỬ LÝ VA CHẠM
                if (velocity.y > 0) {
                    position.y = tile.y - (bbox.h);
                    velocity.y = 0;
                    enemyIsOnGround = true;
                }
            }
        }
    }

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

void Enemy::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_Texture* currentTexture = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
    int totalFrames = 0;

    switch (enemyState) {
    case EnemyState::STATE_IDLE:
        currentTexture = idleTex;
        frameWidth = GameConstants::IDLE_FRAME_WIDTH_ENEMY;
        frameHeight = GameConstants::IDLE_FRAME_HEIGHT_ENEMY;
        totalFrames = GameConstants::IDLE_FRAMES_ENEMY;
        break;
    case EnemyState::STATE_WALK:
    case EnemyState::STATE_PATROL:
        currentTexture = walkTex;
        frameWidth = GameConstants::WALK_FRAME_WIDTH_ENEMY;
        frameHeight = GameConstants::WALK_FRAME_HEIGHT_ENEMY;
        totalFrames = GameConstants::WALK_FRAMES_ENEMY;
        break;
    case EnemyState::STATE_RUN:
    case EnemyState::STATE_CHASE:
        currentTexture = runTex;
        frameWidth = GameConstants::RUN_FRAME_WIDTH_ENEMY;
        frameHeight = GameConstants::RUN_FRAME_HEIGHT_ENEMY;
        totalFrames = GameConstants::RUN_FRAMES_ENEMY;
        break;
    case EnemyState::STATE_ATTACK:
        currentTexture = attackTex;
        frameWidth = GameConstants::ATTACK_FRAME_WIDTH_ENEMY;
        frameHeight = GameConstants::ATTACK_FRAME_HEIGHT_ENEMY;
        totalFrames = GameConstants::ATTACK_FRAMES_ENEMY;
        break;
    case EnemyState::STATE_HURT:
        currentTexture = hurtTex;
        frameWidth = GameConstants::HURT_FRAME_WIDTH_ENEMY;
        frameHeight = GameConstants::HURT_FRAME_HEIGHT_ENEMY;
        totalFrames = GameConstants::HURT_FRAMES_ENEMY;
        break;
    case EnemyState::STATE_DEAD:
        currentTexture = deadTex;
        frameWidth = GameConstants::DEAD_FRAME_WIDTH_ENEMY;
        frameHeight = GameConstants::DEAD_FRAME_HEIGHT_ENEMY;
        totalFrames = GameConstants::DEAD_FRAMES_ENEMY;
        break;
    }

    // FIX: CHỈ VẼ HEALTHBAR KHI SỐNG
    if (isAlive) {
        DrawHealthBar(renderer, health, maxHealth, position, cameraOffset, GetSpriteWidth());
    }

    if (!currentTexture) return;

    int safeFrame = std::min(enemyCurrentFrame, totalFrames - 1);

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

// FIX: THÊM I-FRAMES
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

// FIX: HITBOX NHỎ HƠN, CÓ OFFSET
SDL_FRect Enemy::GetBoundingBox() const {
    float spriteW = GetSpriteWidth();
    return SDL_FRect{
        position.x + spriteW * 0.2f,
        position.y + spriteW * 0.15f,
        spriteW * 0.6f,
        spriteW * 0.8f
    };
}