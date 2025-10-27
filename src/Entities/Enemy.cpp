#include "Enemy.h"
#include <cmath>
#include <algorithm>

 //  CONSTRUCTOR MẶC ĐỊNH
Enemy::Enemy()
    : idleTex(nullptr), walkTex(nullptr), attackTex(nullptr),
    hurtTex(nullptr), deadTex(nullptr),
    currentFrame(0), animationTimer(0.0f),
    frameWidth(64), frameHeight(64),
    idleFrameCount(10), walkFrameCount(10), attackFrameCount(10),
    hurtFrameCount(10), deadFrameCount(10),
    position(0.0f, 0.0f), velocity(0.0f, 0.0f),
    facingRight(true), isOnGround(true),
    health(100), maxHealth(100), attackDamage(10),
    attackRange(50.0f), attackCooldown(1.0f), attackTimer(0.0f),
    isAlive(true), hurtDuration(0.3f), hurtTimer(0.0f),
    currentState(EnemyState::STATE_IDLE),
    previousState(EnemyState::STATE_IDLE),
    enemyType(EnemyType::TYPE_MINION),
    detectionRange(200.0f), patrolSpeed(50.0f), chaseSpeed(100.0f),
    patrolPointA(0.0f, 0.0f), patrolPointB(100.0f, 0.0f),
    currentTarget(0.0f, 0.0f), movingToB(true),
    playerPosition(nullptr)
{
}

// CONSTRUCTOR ĐẦY ĐỦ
Enemy::Enemy(SDL_Renderer* renderer, glm::vec2 startPos, EnemyType type,
    glm::vec2 patrol_A, glm::vec2 patrol_B)
    : Enemy() // Gọi constructor mặc định trước
{
    position = startPos;
    enemyType = type;
    patrolPointA = patrol_A;
    patrolPointB = patrol_B;
    currentTarget = patrolPointB; // Bắt đầu đi về điểm B

    // Thiết lập thuộc tính dựa trên loại enemy
    switch (type) {
    case EnemyType::TYPE_MINION:
        maxHealth = 50;
        health = 50;
        attackDamage = 5;
        detectionRange = 150.0f;
        patrolSpeed = 40.0f;
        chaseSpeed = 80.0f;
        break;

    case EnemyType::TYPE_ELITE:
        maxHealth = 150;
        health = 150;
        attackDamage = 15;
        detectionRange = 200.0f;
        patrolSpeed = 50.0f;
        chaseSpeed = 100.0f;
        break;

    case EnemyType::TYPE_BOSS:
        maxHealth = 500;
        health = 500;
        attackDamage = 30;
        detectionRange = 300.0f;
        patrolSpeed = 60.0f;
        chaseSpeed = 120.0f;
        break;
    }


}

//DESTRUCTOR
Enemy::~Enemy() {
    // Giải phóng tất cả texture
    if (idleTex) SDL_DestroyTexture(idleTex);
    if (walkTex) SDL_DestroyTexture(walkTex);
    if (attackTex) SDL_DestroyTexture(attackTex);
    if (hurtTex) SDL_DestroyTexture(hurtTex);
    if (deadTex) SDL_DestroyTexture(deadTex);
}

//CẬP NHẬT VẬT LÝ
void Enemy::UpdatePhysics(float deltaTime) {
    // Gravity (nếu không đứng trên mặt đất)
    if (!isOnGround) {
        velocity.y += GameConstants::GRAVITY * deltaTime;
    }

    // Cập nhật vị trí theo velocity
    position.y += velocity.y * deltaTime;

    // Collision với ground
    if (position.y + frameHeight >= GameConstants::FLOOR_Y) {
        position.y = GameConstants::FLOOR_Y - frameHeight;
        velocity.y = 0.0f;
        isOnGround = true;
    }

    // Giới hạn trong map
    position.x = std::max(0.0f, std::min(position.x,
        GameConstants::WORLD_WIDTH - (float)frameWidth));
}

//KIỂM TRA PLAYER TRONG TẦM
bool Enemy::CheckPlayerInRange() {
    if (!playerPosition) return false;

    // Tính khoảng cách tới player
    float dx = playerPosition->x - position.x;
    float dy = playerPosition->y - position.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    return distance <= detectionRange;
}

//DI CHUYỂN VỀ PHÍA MỘT ĐIỂM
void Enemy::MoveTowards(glm::vec2 target, float speed, float deltaTime) {
    // Tính vector hướng
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < 5.0f) return; // Đã đến đích

    // Normalize và nhân với speed
    float dirX = dx / distance;
    float dirY = dy / distance;

    position.x += dirX * speed * deltaTime;
    position.y += dirY * speed * deltaTime;

    // Cập nhật hướng mặt
    facingRight = (dx > 0);
}

//CẬP NHẬT AI
void Enemy::UpdateAI(float deltaTime) {
    // Nếu đã chết hoặc đang hurt, không cập nhật AI
    if (!isAlive || currentState == EnemyState::STATE_HURT) {
        return;
    }

    // Giảm attack cooldown
    if (attackTimer > 0) {
        attackTimer -= deltaTime;
    }

    // Kiểm tra player có trong tầm không
    bool playerDetected = CheckPlayerInRange();

    if (playerDetected && playerPosition) {
        // Tính khoảng cách tới player
        float dx = playerPosition->x - position.x;
        float dy = playerPosition->y - position.y;
        float distanceToPlayer = std::sqrt(dx * dx + dy * dy);

        // Nếu trong tầm đánh
        if (distanceToPlayer <= attackRange) {
            currentState = EnemyState::STATE_ATTACK;

            // Tấn công nếu hết cooldown
            if (attackTimer <= 0) {
                Attack();
                attackTimer = attackCooldown;
            }
        }
        else {
            // Đuổi theo player
            currentState = EnemyState::STATE_CHASE;
            MoveTowards(*playerPosition, chaseSpeed, deltaTime);
        }
    }
    else {
        // Tuần tra giữa 2 điểm
        currentState = EnemyState::STATE_PATROL;

        // Di chuyển về điểm đích hiện tại
        MoveTowards(currentTarget, patrolSpeed, deltaTime);

        // Kiểm tra đã đến điểm đích chưa
        float dx = currentTarget.x - position.x;
        float distance = std::abs(dx);

        if (distance < 10.0f) {
            // Đổi điểm đích
            if (movingToB) {
                currentTarget = patrolPointA;
                movingToB = false;
            }
            else {
                currentTarget = patrolPointB;
                movingToB = true;
            }
        }
    }
}



//CẬP NHẬT ANIMATION
void Enemy::UpdateAnimation(float deltaTime) {
    // Reset animation nếu trạng thái thay đổi
    if (currentState != previousState) {
        currentFrame = 0;
        animationTimer = 0.0f;
        previousState = currentState;
    }

    // Xác định số frame và tốc độ dựa trên trạng thái
    int totalFrames = 0;
    float frameDuration = 0.0f;

    switch (currentState) {
    case EnemyState::STATE_IDLE:
    case EnemyState::STATE_PATROL:
        totalFrames = (currentState == EnemyState::STATE_IDLE) ?
            idleFrameCount : walkFrameCount;
        frameDuration = 0.1f;
        break;

    case EnemyState::STATE_CHASE:
        totalFrames = walkFrameCount;
        frameDuration = 0.07f; // Nhanh hơn khi đuổi
        break;

    case EnemyState::STATE_ATTACK:
        totalFrames = attackFrameCount;
        frameDuration = 0.08f;
        break;

    case EnemyState::STATE_HURT:
        totalFrames = hurtFrameCount;
        frameDuration = hurtDuration / (float)hurtFrameCount;
        break;

    case EnemyState::STATE_DEAD:
        totalFrames = deadFrameCount;
        frameDuration = 0.15f;
        break;
    }

    // Cập nhật timer
    animationTimer += deltaTime;

    // Chuyển frame
    if (animationTimer >= frameDuration && totalFrames > 0) {
        animationTimer -= frameDuration;

        // Animation chết không lặp
        if (currentState == EnemyState::STATE_DEAD) {
            currentFrame = std::min(currentFrame + 1, totalFrames - 1);
        }
        else {
            currentFrame = (currentFrame + 1) % totalFrames;
        }
    }
}

//UPDATE TỔNG THỂ 
void Enemy::Update(float deltaTime) {
    // Xử lý hurt state
    if (currentState == EnemyState::STATE_HURT) {
        hurtTimer -= deltaTime;
        if (hurtTimer <= 0) {
            currentState = EnemyState::STATE_IDLE;
        }
    }

    // Cập nhật AI
    UpdateAI(deltaTime);

    // Cập nhật vật lý
    UpdatePhysics(deltaTime);

    // Cập nhật animation
    UpdateAnimation(deltaTime);
}

//  RENDER 
void Enemy::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // Chọn texture dựa trên trạng thái
    SDL_Texture* currentTexture = nullptr;

    switch (currentState) {
    case EnemyState::STATE_IDLE:
        currentTexture = idleTex;
        break;
    case EnemyState::STATE_PATROL:
    case EnemyState::STATE_CHASE:
        currentTexture = walkTex;
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

    if (!currentTexture) return;

    // Source rect (frame hiện tại)
    SDL_FRect srcRect = {
        (float)(currentFrame * frameWidth),
        0.0f,
        (float)frameWidth,
        (float)frameHeight
    };

    // Destination rect (vị trí trên màn hình)
    SDL_FRect dstRect = {
        position.x - cameraOffset.x,
        position.y - cameraOffset.y,
        (float)frameWidth,
        (float)frameHeight
    };

    // Vẽ với flip nếu quay trái
    SDL_RenderTextureRotated(
        renderer,
        currentTexture,
        &srcRect,
        &dstRect,
        0.0,
        nullptr,
        facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
    );

    // Vẽ thanh máu (debug)
    if (isAlive && health < maxHealth) {
        float healthBarWidth = 50.0f;
        float healthBarHeight = 5.0f;
        float healthPercent = (float)health / (float)maxHealth;

        // Background (đỏ)
        SDL_FRect bgRect = {
            position.x - cameraOffset.x,
            position.y - cameraOffset.y - 10.0f,
            healthBarWidth,
            healthBarHeight
        };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &bgRect);

        // Foreground (xanh lá)
        SDL_FRect fgRect = bgRect;
        fgRect.w = healthBarWidth * healthPercent;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &fgRect);
    }
}

//  COMBAT 

void Enemy::TakeDamage(int damage) {
    if (!isAlive) return;

    health -= damage;

    if (health <= 0) {
        health = 0;
        Die();
    }
    else {
        // Chuyển sang trạng thái hurt
        currentState = EnemyState::STATE_HURT;
        hurtTimer = hurtDuration;
        std::cout << "Enemy nhận " << damage << " sát thương! HP: " << health << "\n";
    }
}

void Enemy::Attack() {
    std::cout << "Enemy tấn công! Damage: " << attackDamage << "\n";
    // TODO: Gọi TakeDamage() của player nếu trong tầm
}

void Enemy::Die() {
    isAlive = false;
    currentState = EnemyState::STATE_DEAD;
    std::cout << "Enemy đã chết!\n";
}

//  BOUNDING BOX 
SDL_FRect Enemy::GetBoundingBox() const {
    return SDL_FRect{
        position.x,
        position.y,
        (float)frameWidth,
        (float)frameHeight
    };
}