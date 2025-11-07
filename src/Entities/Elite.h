#pragma once
#include "Enemy.h"
#include <vector>
#include <memory>

// Loại Troll
enum class TrollType {
    TROLL_1,  // Troll cấp 1 - Cơ bản
    TROLL_2,  // Troll cấp 2 - Mạnh hơn
    TROLL_3   // Troll cấp 3 - Mạnh nhất
};

// CLASS ELITES - TROLLS
class Elites : public Enemy {
private:
    TrollType trollType;

    // ===== SPECIAL ATTACK SYSTEM =====
    bool isPerformingSpecial;   // Đang thực hiện chiêu đặc biệt?
    float specialCooldown;      // Thời gian hồi chiêu đặc biệt
    float specialTimer;         // Bộ đếm special
    int specialDamage;          // Sát thương chiêu đặc biệt

    // ===== HEAVY ATTACK =====
    bool isHeavyAttack;         // Đang heavy attack?
    float heavyAttackChargeTime;// Thời gian nạp đòn
    float heavyChargeTimer;     // Bộ đếm charge

    // ===== ROAR/BUFF SYSTEM =====
    bool hasRoared;             // Đã gầm?
    float roarRadius;           // Phạm vi buff cho đồng đội
    float buffDuration;         // Thời gian buff
    float buffTimer;            // Bộ đếm buff

    // ===== PRIVATE METHODS =====
    void PerformSpecialAttack(); // Thực hiện chiêu đặc biệt
    void HeavyAttack();          // Heavy attack
    void Roar();                 // Gầm lên buff đồng đội

public:
    // ===== CONSTRUCTOR =====
    Elites(SDL_Renderer* renderer, glm::vec2 startPos, TrollType type);
    virtual ~Elites();

    // ===== OVERRIDE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
    virtual void PerformAttack() override;
    virtual void TakeDamage(int damage) override;

    // ===== GETTERS =====
    TrollType GetTrollType() const { return trollType; }
    virtual float GetSpriteWidth() const override { return 64.0f; }
    bool IsBuffActive() const { return buffTimer > 0; }
};