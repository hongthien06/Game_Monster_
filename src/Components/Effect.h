#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

// Loại hiệu ứng
enum class EffectType {
    EXPLOSION,      // Nổ
    FLASH,          // Nhấp nháy
    SCREEN_SHAKE,   // Rung màn hình
    PARTICLE,       // Hạt
    TRAIL,          // Vệt sáng
    AURA,           // Hào quang
    BOSS_INTRO,     // MỚI: Hiệu ứng xuất hiện boss
    BOSS_ATTACK     // MỚI: Hiệu ứng đánh boss
};

// ===== PARTICLE STRUCT =====
struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    SDL_Color color;
    float lifetime;
    float maxLifetime;
    float size;

    Particle(glm::vec2 pos, glm::vec2 vel, SDL_Color col, float life, float sz)
        : position(pos), velocity(vel), color(col),
        lifetime(life), maxLifetime(life), size(sz) {
    }

    void Update(float deltaTime) {
        position += velocity * deltaTime;
        lifetime -= deltaTime;
    }

    bool IsAlive() const { return lifetime > 0; }

    float GetAlpha() const {
        return (lifetime / maxLifetime) * 255.0f;
    }
};

// ===== EFFECT CLASS =====
class Effect {
protected:
    EffectType type;
    glm::vec2 position;
    bool isActive;
    float duration;
    float timer;

public:
    Effect(EffectType effectType, glm::vec2 pos, float dur)
        : type(effectType), position(pos), isActive(true),
        duration(dur), timer(0.0f) {
    }

    virtual ~Effect() {}

    virtual void Update(float deltaTime) {
        timer += deltaTime;
        if (timer >= duration) {
            isActive = false;
        }
    }

    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) = 0;

    bool IsActive() const { return isActive; }
    EffectType GetType() const { return type; }
};

// ===== EXPLOSION EFFECT =====
class ExplosionEffect : public Effect {
private:
    std::vector<Particle> particles;
    float radius;
    SDL_Color color;

public:
    ExplosionEffect(glm::vec2 pos, float rad, SDL_Color col);
    virtual void Update(float deltaTime) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
};

// ===== FLASH EFFECT =====
class FlashEffect : public Effect {
private:
    SDL_Color flashColor;
    float intensity;

public:
    FlashEffect(glm::vec2 pos, SDL_Color color, float intense, float dur);
    virtual void Update(float deltaTime) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
};

// ===== TRAIL EFFECT =====
class TrailEffect : public Effect {
private:
    std::vector<glm::vec2> trailPoints;
    SDL_Color trailColor;
    float width;

public:
    TrailEffect(glm::vec2 startPos, SDL_Color color, float w);
    void AddPoint(glm::vec2 point);
    virtual void Update(float deltaTime) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
};

// ===== BOSS INTRO EFFECT (Fade từ trong ra) =====
class BossIntroEffect : public Effect {
private:
    std::vector<Particle> particles;
    float currentRadius;
    float maxRadius;
    SDL_Color primaryColor;
    SDL_Color secondaryColor;
    float pulseTimer;

public:
    BossIntroEffect(glm::vec2 pos, float maxRad, SDL_Color col1, SDL_Color col2);
    virtual void Update(float deltaTime) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
};

// ===== BOSS ATTACK EFFECT (Sóng năng lượng) =====
class BossAttackEffect : public Effect {
private:
    std::vector<Particle> shockwave;
    float currentRadius;
    float maxRadius;
    SDL_Color attackColor;
    float rotationAngle;

public:
    BossAttackEffect(glm::vec2 pos, float maxRad, SDL_Color col);
    virtual void Update(float deltaTime) override;
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) override;
};

// ===== SCREEN SHAKE =====
class ScreenShake {
private:
    float intensity;
    float duration;
    float timer;
    bool isShaking;
    glm::vec2 offset;

public:
    ScreenShake();

    void Trigger(float intense, float dur);
    void Update(float deltaTime);
    glm::vec2 GetOffset() const { return offset; }
    bool IsShaking() const { return isShaking; }
};

// ===== EFFECT MANAGER =====
class EffectManager {
private:
    std::vector<std::unique_ptr<Effect>> effects;
    ScreenShake screenShake;

public:
    EffectManager();
    ~EffectManager();

    // Tạo hiệu ứng cơ bản
    void CreateExplosion(glm::vec2 pos, float radius, SDL_Color color);
    void CreateFlash(glm::vec2 pos, SDL_Color color, float intensity, float duration);
    void CreateTrail(glm::vec2 pos, SDL_Color color, float width);

    // MỚI: Hiệu ứng Boss
    void CreateBossIntro(glm::vec2 pos, float radius = 200.0f);
    void CreateBossAttack(glm::vec2 pos, float radius = 150.0f, SDL_Color color = { 255, 50, 50, 255 });
    void CreateBigExplosion(glm::vec2 pos, float radius = 100.0f); // Explosion lớn hơn

    // Rung màn hình
    void TriggerScreenShake(float intensity, float duration);
    glm::vec2 GetCameraShake() const { return screenShake.GetOffset(); }

    // Update/Render
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    // Clear
    void Clear();
};