#pragma once
#include "Enemy.h"
#include <vector>
#include <memory>

// Loại Troll
enum class TrollType {
    TROLL_1,
    TROLL_2,
    TROLL_3
};

// CLASS ELITES - TROLLS
class Elites : public Enemy {
private:
    TrollType trollType;

    // ===== SPECIAL ATTACK SYSTEM =====
    bool isPerformingSpecial;
    float specialCooldown;
    float specialTimer;
    int specialDamage;

    // ===== HEAVY ATTACK =====
    bool isHeavyAttack;
    float heavyAttackChargeTime;
    float heavyChargeTimer;

    // ===== ROAR/BUFF SYSTEM =====
    bool hasRoared;
    float roarRadius;
    float buffDuration;
    float buffTimer;

    // ===== PRIVATE METHODS =====
    void PerformSpecialAttack();
    void HeavyAttack();
    void Roar();

public:
    // ===== CONSTRUCTOR =====
    Elites(SDL_Renderer* renderer, glm::vec2 startPos, TrollType type);
    virtual ~Elites();

    // ===== OVERRIDE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
    virtual int PerformAttack() override;
    virtual void TakeDamage(int damage) override;

    // ===== GETTERS =====
    TrollType GetTrollType() const { return trollType; }
    virtual float GetSpriteWidth() const override { return 64.0f; }
    bool IsBuffActive() const { return buffTimer > 0; }
};