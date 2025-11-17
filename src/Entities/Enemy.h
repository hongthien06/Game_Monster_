#pragma once
#include "Character.h"
#include <functional>

// CHỈ 7 TRẠNG THÁI
enum class EnemyState {
    STATE_IDLE,
    STATE_WALK,
    STATE_RUN,
    STATE_JUMP,
    STATE_ATTACK,
    STATE_HURT,
    STATE_DEAD
};

enum class EnemyType {
    MINION,
    ELITE,
    BOSS
};

// Struct chứa config frames
struct FrameConfig {
    int frameCount;
    float frameDuration;
    int frameWidth;
    int frameHeight;
};

class Enemy : public Character {
protected:
    EnemyState enemyState;
    EnemyState previousEnemyState;
    EnemyType enemyType;

    int enemyCurrentFrame;
    float enemyAnimationTimer;

    glm::vec2 targetPosition;

    float aggroRange;
    float detectionRange;
    float attackRange;
    float attackCooldown;
    float attackTimer;
    int attackDamage;

    float walkSpeed;
    float runSpeed;

    float hitboxWidth;
    float hitboxHeight;
    float hitboxOffsetX;
    float hitboxOffsetY;

    glm::vec2 patrolPointA;
    glm::vec2 patrolPointB;
    bool movingToB;

    float hurtDuration;
    float hurtTimer;
    bool canTakeDamage;

    float iFramesDuration;
    float iFramesTimer;

    bool isAlive;
    float deathTimer;

    int coinDropAmount;
    bool hasDroppedCoins;
    std::function<void(glm::vec2, int)> onDeathCallback;

    bool enemyCanJump;
    bool enemyIsOnGround;
    float enemyGravity;

    // Render size (khác với frame size)
    float renderWidth;
    float renderHeight;


    glm::vec2 initialPosition;
    glm::vec2 initialPatrolPointA;
    glm::vec2 initialPatrolPointB;

    // VIRTUAL: lớp con override để trả frames riêng
    virtual FrameConfig GetFrameConfig(EnemyState state) const = 0;

    // AI Logic
    virtual void UpdateEnemyState(float deltaTime, Map& map);
    virtual void UpdateEnemyAnimation(float deltaTime);

    // Handlers cho 7 state
    virtual void HandleIdle(float deltaTime);
    virtual void HandleWalk(float deltaTime);
    virtual void HandleRun(float deltaTime);
    virtual void HandleJump(float deltaTime);
    virtual void HandleAttack(float deltaTime);
    virtual void HandleHurt(float deltaTime);

    float GetDistanceToTarget() const;
    bool IsTargetInRange() const;
    bool IsTargetInAttackRange() const;
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

    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;

    virtual void TakeDamage(int damage) override;
    virtual void Die();

    virtual void ResetToStartPosition();

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
    void SetRenderSize(float w, float h) { renderWidth = w; renderHeight = h; }

    bool IsAlive() const { return isAlive; }
    EnemyState GetEnemyState() const { return enemyState; }
    EnemyType GetEnemyType() const { return enemyType; }
    int GetAttackDamage() const { return attackDamage; }
    float GetDeathTimer() const { return deathTimer; }
    bool IsInvulnerable() const { return iFramesTimer > 0; }

    virtual SDL_FRect GetBoundingBox() const override;
    virtual int PerformAttack() { return attackDamage; }
    virtual float GetSpriteWidth() const { return renderWidth; }
};