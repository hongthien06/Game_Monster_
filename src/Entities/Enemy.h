#pragma once
#include "Character.h"
#include <functional>

// FIX: THÊM ĐẦY ĐỦ 7 TRẠNG THÁI
enum class EnemyState {
    STATE_IDLE,
    STATE_WALK,      // TUẦN TRA
    STATE_PATROL,
    STATE_RUN,       // ĐUỔI THEO
    STATE_CHASE,
    STATE_JUMP,      // NHẢY (KHÔNG DÙNG)
    STATE_ATTACK,
    STATE_HURT,
    STATE_DEAD
};

enum class EnemyType {
    MINION,
    ELITE,
    BOSS
};

class Enemy : public Character {
protected:
    EnemyState enemyState;
    EnemyState previousEnemyState;
    EnemyType enemyType;

    int enemyCurrentFrame;
    float enemyAnimationTimer;

    glm::vec2 targetPosition;

    // FIX: TÁCH RÕ AGGRO & ATTACK RANGE
    float aggroRange;         // TẦM KÍCH HOẠT
    float detectionRange;
    float attackRange;
    float attackCooldown;
    float attackTimer;
    int attackDamage;

    float patrolSpeed;
    float walkSpeed;          // THÊM MỚI
    float chaseSpeed;
    float runSpeed;           // THÊM MỚI

    glm::vec2 patrolPointA;
    glm::vec2 patrolPointB;
    bool movingToB;

    float hurtDuration;
    float hurtTimer;
    bool canTakeDamage;

    // FIX: THÊM I-FRAMES
    float iFramesDuration;
    float iFramesTimer;

    bool isAlive;
    float deathTimer;

    int coinDropAmount;
    bool hasDroppedCoins;
    std::function<void(glm::vec2, int)> onDeathCallback;

    // FIX: PHYSICS RIÊNG CHO ENEMY (KHÔNG DÙNG CHUNG VỚI PLAYER)
    bool enemyCanJump;
    bool enemyIsOnGround;
    float enemyGravity;

    // AI LOGIC
    virtual void UpdateEnemyState(float deltaTime, Map& map);
    virtual void UpdateEnemyAnimation(float deltaTime);

    // FIX: HANDLER CHO TỪNG STATE
    virtual void HandleIdle(float deltaTime);
    virtual void HandleWalk(float deltaTime);
    virtual void HandlePatrol(float deltaTime);
    virtual void HandleRun(float deltaTime);
    virtual void HandleChase(float deltaTime);
    virtual void HandleJump(float deltaTime);
    virtual void HandleAttack(float deltaTime);
    virtual void HandleHurt(float deltaTime);

    float GetDistanceToTarget() const;
    bool IsTargetInRange() const;
    bool IsTargetInAttackRange() const;

    // FIX: THÊM CHECK AGGRO RANGE
    bool IsTargetInAggroRange() const;

public:
    Enemy();
    Enemy(SDL_Renderer* renderer, glm::vec2 startPos,
        const char* idlePath,
        const char* walkPath,
        const char* runPath,
        const char* jumpPath,
        const char* attackPath,
        const char* hurtPath,
        const char* deadPath,
        EnemyType type = EnemyType::MINION);

    virtual ~Enemy();

    // MAIN LOOP
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;

    // COMBAT
    virtual void TakeDamage(int damage) override;
    virtual void Die();

    // SETTERS
    void SetTargetPosition(glm::vec2 pos) { targetPosition = pos; }
    void SetPatrolPoints(glm::vec2 pointA, glm::vec2 pointB);
    void SetAggroRange(float range) { aggroRange = range; }
    void SetDetectionRange(float range) { detectionRange = range; }
    void SetAttackRange(float range) { attackRange = range; }
    void SetAttackDamage(int damage) { attackDamage = damage; }
    void SetAttackCooldown(float cooldown) { attackCooldown = cooldown; }
    void SetCanJump(bool canJump) { enemyCanJump = canJump; }
    void SetCoinDropAmount(int amount) { coinDropAmount = amount; }
    void SetOnDeathCallback(std::function<void(glm::vec2, int)> callback) {
        onDeathCallback = callback;
    }

    // GETTERS
    bool IsAlive() const { return isAlive; }
    EnemyState GetEnemyState() const { return enemyState; }
    EnemyType GetEnemyType() const { return enemyType; }
    int GetAttackDamage() const { return attackDamage; }
    float GetDeathTimer() const { return deathTimer; }
    bool IsInvulnerable() const { return iFramesTimer > 0; }

    // FIX: HITBOX NHỎ HƠN, CÓ OFFSET
    virtual SDL_FRect GetBoundingBox() const override;

    virtual int PerformAttack() { return attackDamage; }
};