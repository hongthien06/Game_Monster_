#pragma once
#include "Enemy.h"

enum class MinionType {
    ORC_BERSERK,
    ORC_SHAMAN,
    ORC_WARRIOR
};
// CLASS MINIONS - KẾ THỪA TỪ ENEMY
class Minions : public Enemy {
private:
    MinionType minionType;
    // ===== ORC SHAMAN - MAGIC SYSTEM =====
    SDL_Texture* magicTex;
    bool isCastingMagic;
    float magicCooldown;
    float magicTimer;
    // ===== BERSERK RAGE SYSTEM =====
    bool isRaging;
    float rageThreshold;
    // ===== PRIVATE METHODS =====
    void CastMagic();
    void ActivateRage();

protected:
    // Override: trả frames từ GameConstants
    virtual FrameConfig GetFrameConfig(EnemyState state) const override;

public:
    // ===== CONSTRUCTOR =====
    Minions(SDL_Renderer* renderer, glm::vec2 startPos, MinionType type);
    virtual ~Minions();
    // ===== OVERRIDE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
    virtual int PerformAttack() override;
    virtual void TakeDamage(int damage) override;
    // ===== GETTERS =====
    MinionType GetMinionType() const { return minionType; }
    virtual float GetSpriteWidth() const override { return 48.0f; }
};