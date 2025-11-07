#pragma once
#include "Enemy.h"
#include <vector>
#include <memory>

// Phase c?a Boss
enum class BossPhase {
    PHASE_1,    // Phase 1: 100% - 70% HP
    PHASE_2,    // Phase 2: 70% - 40% HP
    PHASE_3     // Phase 3: 40% - 0% HP (Cu?ng n?)
};

// CLASS BOSS - FINAL BOSS
class Boss : public Enemy {
private:
    // ===== PHASE SYSTEM =====
    BossPhase currentPhase;
    bool hasEnteredPhase2;
    bool hasEnteredPhase3;

    // ===== ULTIMATE ATTACK =====
    bool isUsingUltimate;       // ?ang dùng chiêu cu?i?
    float ultimateCooldown;     // Th?i gian h?i ultimate
    float ultimateTimer;        // B? ??m ultimate
    int ultimateDamage;         // Sát th??ng ultimate
    float ultimateRadius;       // Ph?m vi AOE

    // ===== SUMMON MINIONS =====
    bool canSummon;             // Có th? tri?u h?i?
    int summonCount;            // S? l??ng ?ã tri?u h?i
    int maxSummons;             // S? l??ng t?i ?a
    float summonCooldown;       // Th?i gian h?i summon
    float summonTimer;          // B? ??m summon

    // ===== CHARGE ATTACK =====
    bool isCharging;            // ?ang lao v? phía Player?
    float chargeSpeed;          // T?c ?? lao
    glm::vec2 chargeDirection;  // H??ng lao
    float chargeDuration;       // Th?i gian lao
    float chargeTimer;          // B? ??m charge

    // ===== GROUND SLAM =====
    bool isSlamming;            // ?ang ??p ??t?
    float slamRadius;           // Ph?m vi slam
    int slamDamage;             // Sát th??ng slam

    // ===== BOSS INTRO =====
    bool hasIntroPlayed;        // ?ã phát intro?
    bool isInvulnerable;        // B?t t? (trong intro)?
    float introTimer;           // B? ??m intro

    // ===== PRIVATE METHODS =====
    void CheckPhaseTransition(); // Ki?m tra chuy?n phase
    void EnterPhase2();          // Vào phase 2
    void EnterPhase3();          // Vào phase 3 (Rage mode)

    void UseUltimate();          // Dùng chiêu cu?i
    void SummonMinions();        // Tri?u h?i minions
    void ChargeAttack();         // Lao v? phía Player
    void GroundSlam();           // ??p ??t t?o sóng xung kích

    void PlayIntro();            // Phát intro (rung màn hình)

public:
    // ===== CONSTRUCTOR =====
    Boss(SDL_Renderer* renderer, glm::vec2 startPos);
    virtual ~Boss();

    // ===== OVERRIDE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
    virtual void PerformAttack() override;
    virtual void TakeDamage(int damage) override;

    // ===== GETTERS =====
    BossPhase GetCurrentPhase() const { return currentPhase; }
    bool IsInIntro() const { return !hasIntroPlayed; }
    virtual float GetSpriteWidth() const override { return 96.0f; }

    // ===== BOSS SPECIFIC =====
    void TriggerIntro(); // Kích ho?t intro khi Player vào phòng
};