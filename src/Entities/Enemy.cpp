#include "Enemy.h"
#include "../Systems/MovementSystem.h"
#include <iostream>
#include <cmath>
#include "EntityUtils.h"

// ===== CONSTRUCTOR MẶC ĐỊNH =====
Enemy::Enemy()
    : Character(),
    enemyState(EnemyState::STATE_IDLE),
    previousEnemyState(EnemyState::STATE_IDLE),
    enemyType(EnemyType::MINION),
    enemyCurrentFrame(0),
    enemyAnimationTimer(0.0f),
    targetPosition(0.0f, 0.0f),
    detectionRange(200.0f),
    attackRange(50.0f),
    attackCooldown(1.5f),
    attackTimer(0.0f),
    attackDamage(10),
    patrolSpeed(50.0f),
    chaseSpeed(100.0f),
    patrolPointA(0.0f, 0.0f),
    patrolPointB(0.0f, 0.0f),
    movingToB(true),
    hurtDuration(0.3f),
    hurtTimer(0.0f),
    canTakeDamage(true),
    isAlive(true),
    deathTimer(0.0f)
{
}

// ===== CONSTRUCTOR CÓ THAM SỐ =====
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
        nullptr, attackPath, hurtPath, deadPath),
    enemyState(EnemyState::STATE_PATROL),
    previousEnemyState(EnemyState::STATE_PATROL),
    enemyType(type),
    enemyCurrentFrame(0),
    enemyAnimationTimer(0.0f),
    targetPosition(0.0f, 0.0f),
    detectionRange(200.0f),
    attackRange(50.0f),
    attackCooldown(1.5f),
    attackTimer(0.0f),
    attackDamage(10),
    patrolSpeed(50.0f),
    chaseSpeed(100.0f),
    patrolPointA(startPos - glm::vec2(100.0f, 0.0f)),
    patrolPointB(startPos + glm::vec2(100.0f, 0.0f)),
    movingToB(true),
    hurtDuration(0.3f),
    hurtTimer(0.0f),
    canTakeDamage(true),
    isAlive(true),
    deathTimer(0.0f)
{
    // Thiết lập thông số theo loại Enemy
    switch (type) {
    case EnemyType::MINION:
        maxHealth = 50;
        health = 50;
        attackDamage = 10;
        detectionRange = 150.0f;
        break;
    case EnemyType::ELITE:
        maxHealth = 150;
        health = 150;
        attackDamage = 25;
        detectionRange = 250.0f;
        chaseSpeed = 120.0f;
        break;
    case EnemyType::BOSS:
        maxHealth = 500;
        health = 500;
        attackDamage = 40;
        detectionRange = 300.0f;
        attackRange = 80.0f;
        break;
    }
}

// ===== DESTRUCTOR =====
Enemy::~Enemy() {
}

// ===== TÍNH KHOẢNG CÁCH ĐẾN TARGET =====
float Enemy::GetDistanceToTarget() const {
    glm::vec2 diff = targetPosition - position;
    return glm::length(diff);
}

// ===== KIỂM TRA TARGET TRONG TẦM PHÁT HIỆN =====
bool Enemy::IsTargetInRange() const {
    return GetDistanceToTarget() <= detectionRange;
}

// ===== KIỂM TRA TARGET TRONG TẦM TẤN CÔNG =====
bool Enemy::IsTargetInAttackRange() const {
    return GetDistanceToTarget() <= attackRange;
}

// ===== SET ĐIỂM TUẦN TRA =====
void Enemy::SetPatrolPoints(glm::vec2 pointA, glm::vec2 pointB) {
    patrolPointA = pointA;
    patrolPointB = pointB;
}

// ===== XỬ LÝ TUẦN TRA =====
void Enemy::HandlePatrol(float deltaTime) {
    glm::vec2 targetPoint = movingToB ? patrolPointB : patrolPointA;
    glm::vec2 direction = targetPoint - position;
    float distance = glm::length(direction);

    if (distance < 5.0f) {
        movingToB = !movingToB;
        velocity.x = 0.0f;
        return;
    }

    direction = glm::normalize(direction);
    velocity.x = direction.x * patrolSpeed;

    if (velocity.x < 0) flipHorizontal = true;
    else if (velocity.x > 0) flipHorizontal = false;
}

// ===== XỬ LÝ ĐUỔI THEO =====
void Enemy::HandleChase(float deltaTime) {
    glm::vec2 direction = targetPosition - position;
    float distance = glm::length(direction);

    if (distance > 0.1f) {
        direction = glm::normalize(direction);
        velocity.x = direction.x * chaseSpeed;

        if (velocity.x < 0) flipHorizontal = true;
        else if (velocity.x > 0) flipHorizontal = false;
    }
}

// ===== XỬ LÝ TẤN CÔNG =====
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

// ===== CẬP NHẬT TRẠNG THÁI ENEMY =====
void Enemy::UpdateEnemyState(float deltaTime, Map& map) {
    if (!isAlive) {
        enemyState = EnemyState::STATE_DEAD;
        velocity.x = 0.0f;
        return;
    }

    if (enemyState == EnemyState::STATE_HURT) {
        hurtTimer -= deltaTime;
        velocity.x = 0.0f;

        if (hurtTimer <= 0) {
            canTakeDamage = true;
            enemyState = EnemyState::STATE_IDLE;
        }
        return;
    }

    // AI Logic
    if (IsTargetInAttackRange()) {
        enemyState = EnemyState::STATE_ATTACK;
        HandleAttack(deltaTime);
    }
    else if (IsTargetInRange()) {
        enemyState = EnemyState::STATE_CHASE;
        HandleChase(deltaTime);
    }
    else {
        enemyState = EnemyState::STATE_PATROL;
        HandlePatrol(deltaTime);
    }
}

// ===== CẬP NHẬT ANIMATION ENEMY =====
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
    case EnemyState::STATE_PATROL:
    case EnemyState::STATE_CHASE:
        totalFrames = GameConstants::WALK_FRAMES;
        frameDuration = GameConstants::WALK_FRAME_DURATION;
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

// ===== UPDATE CHÍNH =====
void Enemy::Update(float deltaTime, Map& map) {
    UpdateEnemyState(deltaTime, map);
    UpdateEnemyAnimation(deltaTime);

    if (enemyState != EnemyState::STATE_DEAD) {
        MovementSystem::HandleMovement(*this, deltaTime, map);
    }

    if (!isAlive) {
        deathTimer += deltaTime;
    }
}

// ===== RENDER =====
void Enemy::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_Texture* currentTexture = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
    int totalFrames = 0;

    switch (enemyState) {
    case EnemyState::STATE_IDLE:
        currentTexture = idleTex;
        frameWidth = GameConstants::IDLE_FRAME_WIDTH;
        frameHeight = GameConstants::IDLE_FRAME_HEIGHT;
        totalFrames = GameConstants::IDLE_FRAMES;
        break;
    case EnemyState::STATE_PATROL:
    case EnemyState::STATE_CHASE:
        currentTexture = walkTex;
        frameWidth = GameConstants::WALK_FRAME_WIDTH;
        frameHeight = GameConstants::WALK_FRAME_HEIGHT;
        totalFrames = GameConstants::WALK_FRAMES;
        break;
    case EnemyState::STATE_ATTACK:
        currentTexture = attackTex;
        frameWidth = GameConstants::ATTACK_FRAME_WIDTH;
        frameHeight = GameConstants::ATTACK_FRAME_HEIGHT;
        totalFrames = GameConstants::ATTACK_FRAMES;
        break;
    case EnemyState::STATE_HURT:
        currentTexture = hurtTex;
        frameWidth = GameConstants::HURT_FRAME_WIDTH;
        frameHeight = GameConstants::HURT_FRAME_HEIGHT;
        totalFrames = GameConstants::HURT_FRAMES;
        break;
    case EnemyState::STATE_DEAD:
        currentTexture = deadTex;
        frameWidth = GameConstants::DEAD_FRAME_WIDTH;
        frameHeight = GameConstants::DEAD_FRAME_HEIGHT;
        totalFrames = GameConstants::DEAD_FRAMES;
        break;
    }

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

// ===== NHẬN SÁT THƯƠNG =====
void Enemy::TakeDamage(int damage) {
    if (!isAlive || !canTakeDamage) return;

    health -= damage;

    if (health <= 0) {
        health = 0;
        Die();
    }
    else {
        enemyState = EnemyState::STATE_HURT;
        hurtTimer = hurtDuration;
        canTakeDamage = false;
        std::cout << "Enemy nhan " << damage << " sat thuong! HP: " << health << "\n";
    }
}

// ===== CHẾT =====
void Enemy::Die() {
    isAlive = false;
    enemyState = EnemyState::STATE_DEAD;
    velocity = glm::vec2(0.0f, 0.0f);
    std::cout << "Enemy da chet!\n";
}

// ===== GET BOUNDING BOX =====
SDL_FRect Enemy::GetBoundingBox() const {
    return SDL_FRect{
        position.x + 8.0f,
        position.y + 8.0f,
        32.0f,
        40.0f
    };
}