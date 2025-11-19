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
    runSpeed(100.0f),
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
    initialPatrolPointB(0.0f, 0.0f),
    audioSystem(nullptr)
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
    initialPatrolPointB(startPos + glm::vec2(100.0f, 0.0f)),
    audioSystem(nullptr)
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
void Enemy::HandleWalk(float deltaTime, Map& map) {
    glm::vec2 targetPoint = movingToB ? patrolPointB : patrolPointA;
    float direction = (targetPoint.x > position.x) ? 1.0f : -1.0f;

    // 1. Check vực thẳm (quan trọng để không rơi)
    if (!CheckGroundAhead(map, direction)) {
        movingToB = !movingToB; // Quay đầu ngay
        velocity.x = 0.0f;
        return;
    }

    // 2. Check khoảng cách đích (trục X)
    float distanceX = std::abs(targetPoint.x - position.x);
    if (distanceX < 10.0f) {
        movingToB = !movingToB;
        velocity.x = 0.0f;
        return;
    }

    velocity.x = direction * walkSpeed;
    flipHorizontal = (velocity.x < 0);
}

// ===== 3. THÊM HÀM HANDLE FLEE (BỎ CHẠY) =====
void Enemy::HandleFlee(float deltaTime, Map& map) {
    // Tính hướng ngược lại với người chơi (Vector từ Player -> Enemy)
    glm::vec2 directionVec = position - targetPosition;

    float dirSign = (directionVec.x > 0) ? 1.0f : -1.0f;

    // 1. CHECK VỰC THẲM KHI BỎ CHẠY
    // Nếu phía sau lưng là vực -> Không chạy lùi được nữa -> Đứng lại chịu trận hoặc đánh trả
    if (!CheckGroundAhead(map, dirSign)) {
        velocity.x = 0.0f;
        // Tùy chọn: Nếu bị dồn vào chân tường thì quay sang đánh luôn (hoặc đứng run sợ)
        // enemyState = EnemyState::STATE_IDLE; 
        return;
    }

    // 2. XỬ LÝ DI CHUYỂN
    if (glm::length(directionVec) > 0) {
        velocity.x = dirSign * runSpeed; // Chạy với tốc độ Run
        flipHorizontal = velocity.x < 0;
    }
}

void Enemy::HandleRun(float deltaTime, Map& map) {
    glm::vec2 directionVec = targetPosition - position;
    float distance = glm::length(directionVec);
    float dirSign = (directionVec.x > 0) ? 1.0f : -1.0f;

    // 1. CHECK VỰC THẲM TRƯỚC KHI CHẠY TỚI
    if (!CheckGroundAhead(map, dirSign)) {
        // Gặp vực -> Dừng lại ngay
        velocity.x = 0.0f;

        // Chuyển trạng thái về ĐI TUẦN (Quay về)
        enemyState = EnemyState::STATE_WALK;

        // Reset điểm tuần tra ngay tại chỗ để nó quay đầu đi hướng khác
        // Logic: Đặt điểm A và B xung quanh vị trí hiện tại
        patrolPointA = position - glm::vec2(50, 0);
        patrolPointB = position + glm::vec2(50, 0);

        // Đảo chiều di chuyển tuần tra để nó quay lưng đi ngay lập tức
        if (dirSign > 0) movingToB = false; // Đang định đi phải gặp vực -> ép đi về trái (A)
        else movingToB = true;              // Đang định đi trái gặp vực -> ép đi về phải (B)

        return;
    }

    // 2. DI CHUYỂN BÌNH THƯỜNG
    if (distance > 0.1f) {
        velocity.x = dirSign * runSpeed;
        flipHorizontal = velocity.x < 0;
    }
    else {
        velocity.x = 0.0f;
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
    velocity.x = 0.0f; // Bị đau thì khựng lại chút

    if (hurtTimer <= 0) {
        canTakeDamage = true;

        // LOGIC MỚI: Hết bị choáng thì làm gì?

        // 1. Nếu Player đang ở ngay trước mặt (trong tầm đánh) -> ĐÁNH LUÔN
        if (IsTargetInAttackRange()) {
            enemyState = EnemyState::STATE_ATTACK;
            // Reset timer để đảm bảo đánh được ngay (nếu logic Enrage bên trên đã xử lý)
            if (attackTimer > 0.1f) attackTimer = 0.0f;
        }
        // 2. Nếu Player ở xa -> ĐUỔI THEO LUÔN (Thay vì đứng im IDLE)
        else {
            enemyState = EnemyState::STATE_RUN;
        }
    }
}

// === STATE MACHINE (CHỈ 7 STATE) ===
void Enemy::UpdateEnemyState(float deltaTime, Map& map) {
    if (!isAlive) {
        enemyState = EnemyState::STATE_DEAD;
        velocity.x = 0.0f;
        return;
    }

    if (iFramesTimer > 0) iFramesTimer -= deltaTime;
    if (enemyState == EnemyState::STATE_HURT) {
        HandleHurt(deltaTime);
        return;
    }

    // --- LOGIC AI MỚI ---
    float distToPlayer = GetDistanceToTarget();

    // [MỚI] 1. Ưu tiên cao nhất: BỎ CHẠY KHI MÁU YẾU
    // Nếu máu < 30% VÀ Player đang ở gần (trong tầm nhìn) -> Bỏ chạy
    if (IsLowHealth() && distToPlayer < detectionRange) {
        // Dùng animation RUN cho hành động bỏ chạy
        enemyState = EnemyState::STATE_RUN;
        HandleFlee(deltaTime, map); // Gọi hàm bỏ chạy (có truyền map để check vực)
        return; // Kết thúc logic, không làm gì khác
    }

    // 2. Tấn công (Nếu máu còn nhiều)
    if (IsTargetInAttackRange()) {
        enemyState = EnemyState::STATE_ATTACK;
        HandleAttack(deltaTime);
    }
    // 3. Xử lý trạng thái ĐUỔI THEO (RUN)
    else if (enemyState == EnemyState::STATE_RUN) {
        // Logic Hysteresis: Đuổi xa hơn tầm aggro một chút mới bỏ
        if (distToPlayer <= aggroRange * 1.5f) {
            HandleRun(deltaTime, map); // <-- Truyền map vào đây
        }
        else {
            // Player chạy thoát -> Quay về đi bộ
            enemyState = EnemyState::STATE_WALK;
            patrolPointA = position - glm::vec2(50, 0);
            patrolPointB = position + glm::vec2(50, 0);
        }
    }
    // 4. Xử lý trạng thái ĐI TUẦN (WALK)
    else {
        if (distToPlayer <= aggroRange) {
            enemyState = EnemyState::STATE_RUN;
            HandleRun(deltaTime, map); // <-- Truyền map vào đây
        }
        else {
            enemyState = EnemyState::STATE_WALK;
            HandleWalk(deltaTime, map);
        }
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
        DrawHealthBar(renderer, health, maxHealth, position, cameraOffset, renderWidth-15.0f);
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

    // 1. Trừ máu
    health -= damage;

    // 2. LOGIC PHẢN XẠ (MỚI) ================================

    // A. Kích hoạt Aggro ngay lập tức
    // Dù đang đứng xa hay đang đi tuần, bị đánh là phải đuổi theo ngay
    if (enemyState == EnemyState::STATE_IDLE || enemyState == EnemyState::STATE_WALK) {
        enemyState = EnemyState::STATE_RUN;
    }

    // B. Quay mặt về phía sát thương (Giả sử targetPosition là Player)
    // Nếu Player đang ở bên trái mà Enemy đang nhìn bên phải -> Quay lại ngay
    float dirToPlayer = targetPosition.x - position.x;
    if (dirToPlayer > 0) flipHorizontal = false;
    else flipHorizontal = true;

    // C. Cơ chế "Nổi điên" (Enrage)
    // Nếu đang đợi hồi chiêu tấn công (ví dụ còn 1s nữa mới đánh)
    // Bị đánh cái đau quá -> Rút ngắn thời gian chờ xuống còn 0.2s để đánh trả liền
    if (attackTimer > 0.2f) {
        attackTimer = 0.2f;
    }
    // ========================================================

    if (health <= 0) {
        health = 0;
        Die();
    }
    else {
        enemyState = EnemyState::STATE_HURT;
        hurtTimer = hurtDuration;
        canTakeDamage = false;
        iFramesTimer = iFramesDuration;

        // Dừng di chuyển khi bị đau
        velocity.x = 0.0f;
    }
}

void Enemy::Die() {
    isAlive = false;
    enemyState = EnemyState::STATE_DEAD;
    velocity = glm::vec2(0.0f, 0.0f);

    // Phát âm thanh chết
    if (audioSystem) {
        switch (enemyType) {
        case EnemyType::MINION:
            audioSystem->playSound("assets/audio/goblin_death.mp3", false, 1.0f);
            break;
        case EnemyType::ELITE:
            audioSystem->playSound("assets/audio/goblin_death.mp3", false, 1.0f);
            break;
        case EnemyType::BOSS:
            audioSystem->playSound("assets/audio/goblin_death.mp3", false, 1.0f);
            break;
        }
    }
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

// ===== HÀM KIỂM TRA ĐẤT PHÍA TRƯỚC =====
bool Enemy::CheckGroundAhead(Map& map, float directionX) {
    // ====================================================
     // BƯỚC 1: CẤU HÌNH CẢM BIẾN
     // ====================================================
    float sensorDist = 10.0f;   // Khoảng cách check phía trước
    float maxFallHeight = 300.0f; // Độ cao tối đa cho phép rơi (tùy chỉnh theo map của bạn)

    SDL_FRect sensor;
    sensor.w = 10.0f; // Bề ngang nhỏ để chính xác

    // Xác định vị trí X của sensor (trước mặt Enemy)
    if (directionX > 0) {
        sensor.x = position.x + hitboxOffsetX + hitboxWidth + sensorDist;
    }
    else {
        sensor.x = position.x + hitboxOffsetX - sensorDist;
    }

    // ====================================================
    // BƯỚC 2: CHECK NGAY DƯỚI CHÂN (Đi bộ bình thường)
    // ====================================================
    // Đặt sensor ngay sát dưới chân
    sensor.y = position.y + hitboxHeight + hitboxOffsetY + 2.0f;
    sensor.h = 10.0f; // Chiều cao nhỏ

    // Nếu ngay dưới chân có đất -> AN TOÀN TUYỆT ĐỐI -> Đi tiếp
    if (map.checkCollision(sensor)) {
        return true;
    }

    // ====================================================
    // BƯỚC 3: CHECK TẦNG DƯỚI (Nếu bước 1 hụt chân)
    // ====================================================
    // Nếu không có đất ngay chân, ta kéo dài sensor xuống dưới
    // để xem có tầng nào ở dưới hứng không.

    sensor.h = maxFallHeight; // Kéo dài sensor xuống (ví dụ 300px)

    // Kiểm tra va chạm với sensor dài này
    if (map.checkCollision(sensor)) {
        // Có đất ở dưới trong phạm vi an toàn -> AN TOÀN -> Cho phép rơi xuống
        return true;
    }

    // ====================================================
    // KẾT LUẬN: VỰC THẲM
    // ====================================================
    // Nếu check cả ngay chân lẫn check sâu xuống dưới đều không thấy gì
    // -> Đây là vực thẳm hoặc rơi ra khỏi map -> KHÔNG ĐI
    return false;
}