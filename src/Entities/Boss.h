#pragma once
#include "Enemy.h"
#include "../Components/Effect.h" 
#include <vector>
#include <memory>

// Phase c?a Boss
enum class BossPhase {
    PHASE_1,    // 100% - 70% HP
    PHASE_2,    // 70% - 40% HP
    PHASE_3     // 40% - 0% HP 
};

// CLASS BOSS - FINAL BOSS
class Boss : public Enemy {
private:
    // ===== PHASE SYSTEM =====
    BossPhase currentPhase;
    bool hasEnteredPhase2;
    bool hasEnteredPhase3;

    // ===== ULTIMATE ATTACK =====
    bool isUsingUltimate;
    float ultimateCooldown;
    float ultimateTimer;
    int ultimateDamage;
    float ultimateRadius;

    // ===== SUMMON MINIONS =====
    bool canSummon;
    int summonCount;
    int maxSummons;
    float summonCooldown;
    float summonTimer;

    // ===== CHARGE ATTACK =====
    bool isCharging;
    float chargeSpeed;
    glm::vec2 chargeDirection;
    float chargeDuration;
    float chargeTimer;

    // ===== GROUND SLAM =====
    bool isSlamming;
    float slamRadius;
    int slamDamage;

    // ===== BOSS INTRO =====
    bool hasIntroPlayed;
    bool isInvulnerable;
    float introTimer;

    // ===== EFFECT MANAGER ===== 
    EffectManager* effectManager;

    // ===== PRIVATE METHODS =====
    void CheckPhaseTransition();
    void EnterPhase2();
    void EnterPhase3();

    void UseUltimate();
    void SummonMinions();
    void ChargeAttack();
    void GroundSlam();

    void PlayIntro();

protected:
    // Override: GameConstants
    virtual FrameConfig GetFrameConfig(EnemyState state) const override;

public:
    // ===== CONSTRUCTOR =====
    Boss(SDL_Renderer* renderer, glm::vec2 startPos);
    virtual ~Boss();

    // ===== OVERRIDE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
    virtual int PerformAttack() override;
    virtual void TakeDamage(int damage) override;

    // M?I: Setter cho EffectManager
    void SetEffectManager(EffectManager* manager) { effectManager = manager; }


    // ===== GETTERS =====
    BossPhase GetCurrentPhase() const { return currentPhase; }
    bool IsInIntro() const { return !hasIntroPlayed; }
    virtual float GetSpriteWidth() const override { return 256.0f; }

    // ===== BOSS SPECIFIC =====
    void TriggerIntro();
};