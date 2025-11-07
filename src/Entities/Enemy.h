#pragma once
#include "Character.h"
#include <functional>

// Trạng thái Enemy cơ bản
enum class EnemyState {
    STATE_IDLE,       // Đứng yên
    STATE_PATROL,     // Tuần tra
    STATE_CHASE,      // Đuổi theo Player
    STATE_ATTACK,     // Tấn công
    STATE_HURT,       // Bị thương
    STATE_DEAD        // Chết
};

// Loại Enemy (dùng để phân biệt hành vi)
enum class EnemyType {
    MINION,    // Quái thường
    ELITE,     // Quái ưu tú (Troll)
    BOSS       // Boss cuối
};

// BASE CLASS cho tất cả Enemy
class Enemy : public Character {
protected:
    // ===== ENEMY STATE =====
    EnemyState enemyState;
    EnemyState previousEnemyState;
    EnemyType enemyType;

    // ===== ENEMY ANIMATION =====
    int enemyCurrentFrame;
    float enemyAnimationTimer;

    // ===== AI & COMBAT =====
    float detectionRange;       // Phạm vi phát hiện Player
    float attackRange;          // Phạm vi tấn công
    float attackCooldown;       // Thời gian hồi chiêu
    float attackTimer;          // Bộ đếm cooldown
    int attackDamage;           // Sát thương

    float patrolSpeed;          // Tốc độ tuần tra
    float chaseSpeed;           // Tốc độ đuổi theo

    // ===== PATROL SYSTEM =====
    glm::vec2 patrolPointA;     // Điểm tuần tra A
    glm::vec2 patrolPointB;     // Điểm tuần tra B
    bool movingToB;             // Đang di chuyển đến B?

    // ===== HURT STATE =====
    float hurtDuration;         // Thời gian bị choáng
    float hurtTimer;            // Bộ đếm hurt
    bool canTakeDamage;         // Có thể nhận sát thương?

    // ===== DEATH =====
    bool isAlive;               // Còn sống?
    float deathTimer;           // Thời gian trước khi xóa

    // ===== REFERENCE =====
    class Player* targetPlayer; // Con trỏ đến Player (để tính khoảng cách)

    // ===== PROTECTED METHODS =====
    virtual void UpdateEnemyState(float deltaTime, Map& map);
    virtual void UpdateEnemyAnimation(float deltaTime);
    virtual void HandlePatrol(float deltaTime);
    virtual void HandleChase(float deltaTime);
    virtual void HandleAttack(float deltaTime);

    // Tính khoảng cách đến Player
    float GetDistanceToPlayer() const;

    // Kiểm tra Player có trong tầm phát hiện không
    bool IsPlayerInRange() const;

    // Kiểm tra Player có trong tầm tấn công không
    bool IsPlayerInAttackRange() const;

public:
    // ===== CONSTRUCTOR =====
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

    // ===== MAIN METHODS (OVERRIDE) =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;

    // ===== COMBAT (OVERRIDE) =====
    virtual void TakeDamage(int damage) override;
    virtual void Die();

    // ===== SETTERS =====
    void SetTargetPlayer(Player* player) { targetPlayer = player; }
    void SetPatrolPoints(glm::vec2 pointA, glm::vec2 pointB);
    void SetDetectionRange(float range) { detectionRange = range; }
    void SetAttackRange(float range) { attackRange = range; }
    void SetAttackDamage(int damage) { attackDamage = damage; }
    void SetAttackCooldown(float cooldown) { attackCooldown = cooldown; }

    // ===== GETTERS =====
    bool IsAlive() const { return isAlive; }
    EnemyState GetEnemyState() const { return enemyState; }
    EnemyType GetEnemyType() const { return enemyType; }
    int GetAttackDamage() const { return attackDamage; }

    // ===== BOUNDING BOX (OVERRIDE) =====
    virtual SDL_FRect GetBoundingBox() const override;

    // ===== VIRTUAL METHOD CHO SUBCLASS =====
    virtual void PerformAttack() {} // Thực hiện tấn công (override ở subclass)
};