#pragma once
#include "Enemy.h"

// Loại Minion
enum class MinionType {
    ORC_BERSERK,  // Orc cuồng nộ - tấn công nhanh, máu thấp
    ORC_SHAMAN,   // Orc phù thủy - tấn công từ xa
    ORC_WARRIOR   // Orc chiến binh - cân bằng
};

// CLASS MINIONS - KẾ THỪA TỪ ENEMY
class Minions : public Enemy {
private:
    MinionType minionType;

    // ===== ORC SHAMAN - MAGIC SYSTEM =====
    SDL_Texture* magicTex1;     // Texture magic 1
    SDL_Texture* magicTex2;     // Texture magic 2
    bool isCastingMagic;        // Đang thi pháp thuật?
    float magicCooldown;        // Thời gian hồi pháp thuật
    float magicTimer;           // Bộ đếm magic

    // ===== BERSERK RAGE SYSTEM =====
    bool isRaging;              // Đang trong trạng thái cuồng nộ?
    float rageThreshold;        // Ngưỡng máu kích hoạt rage (%)

    // ===== PRIVATE METHODS =====
    void CastMagic();           // Thi pháp thuật (Shaman)
    void ActivateRage();        // Kích hoạt rage (Berserk)

public:
    // ===== CONSTRUCTOR =====
    Minions(SDL_Renderer* renderer, glm::vec2 startPos, MinionType type);
    virtual ~Minions();

    // ===== OVERRIDE METHODS =====
    virtual void Update(float deltaTime, Map& map) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
    virtual void PerformAttack() override;
    virtual void TakeDamage(int damage) override;

    // ===== GETTERS =====
    MinionType GetMinionType() const { return minionType; }
    virtual float GetSpriteWidth() const override { return 48.0f; }
};