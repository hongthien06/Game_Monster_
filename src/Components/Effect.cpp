#include "Effect.h"
#include <cmath>
#include <iostream>
#include <algorithm>

// ===== EXPLOSION EFFECT =====
ExplosionEffect::ExplosionEffect(glm::vec2 pos, float rad, SDL_Color col)
    : Effect(EffectType::EXPLOSION, pos, 1.0f), radius(rad), color(col)
{
    int particleCount = 30;
    for (int i = 0; i < particleCount; i++) {
        float angle = (float)i / particleCount * 6.28318f;
        float speed = 100.0f + (rand() % 100);

        glm::vec2 velocity(
            cos(angle) * speed,
            sin(angle) * speed
        );

        particles.emplace_back(pos, velocity, color, 0.8f, 3.0f + (rand() % 5));
    }
}

void ExplosionEffect::Update(float deltaTime) {
    Effect::Update(deltaTime);

    for (auto& particle : particles) {
        particle.Update(deltaTime);
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return !p.IsAlive(); }),
        particles.end()
    );

    if (particles.empty()) {
        isActive = false;
    }
}

void ExplosionEffect::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    for (const auto& particle : particles) {
        SDL_Color col = particle.color;
        col.a = (Uint8)particle.GetAlpha();
        SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

        SDL_FRect rect = {
            particle.position.x - cameraOffset.x - particle.size / 2,
            particle.position.y - cameraOffset.y - particle.size / 2,
            particle.size,
            particle.size
        };

        SDL_RenderFillRect(renderer, &rect);
    }
}

// ===== FLASH EFFECT =====
FlashEffect::FlashEffect(glm::vec2 pos, SDL_Color color, float intense, float dur)
    : Effect(EffectType::FLASH, pos, dur), flashColor(color), intensity(intense)
{
}

void FlashEffect::Update(float deltaTime) {
    Effect::Update(deltaTime);

    float progress = timer / duration;
    intensity = (1.0f - progress) * intensity;
}

void FlashEffect::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_Color col = flashColor;
    col.a = (Uint8)(intensity * 255.0f);

    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_RenderFillRect(renderer, nullptr);
}

// ===== TRAIL EFFECT =====
TrailEffect::TrailEffect(glm::vec2 startPos, SDL_Color color, float w)
    : Effect(EffectType::TRAIL, startPos, 0.5f), trailColor(color), width(w)
{
    trailPoints.push_back(startPos);
}

void TrailEffect::AddPoint(glm::vec2 point) {
    trailPoints.push_back(point);

    if (trailPoints.size() > 20) {
        trailPoints.erase(trailPoints.begin());
    }
}

void TrailEffect::Update(float deltaTime) {
    Effect::Update(deltaTime);

    if (!trailPoints.empty() && (rand() % 100) < 30) {
        trailPoints.erase(trailPoints.begin());
    }

    if (trailPoints.empty()) {
        isActive = false;
    }
}

void TrailEffect::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    if (trailPoints.size() < 2) return;

    for (size_t i = 0; i < trailPoints.size() - 1; i++) {
        float alpha = (float)i / trailPoints.size() * 255.0f;
        SDL_Color col = trailColor;
        col.a = (Uint8)alpha;

        SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

        SDL_RenderLine(renderer,
            trailPoints[i].x - cameraOffset.x,
            trailPoints[i].y - cameraOffset.y,
            trailPoints[i + 1].x - cameraOffset.x,
            trailPoints[i + 1].y - cameraOffset.y
        );
    }
}

// ===== BOSS INTRO EFFECT (Fade từ trong ra) =====
BossIntroEffect::BossIntroEffect(glm::vec2 pos, float maxRad, SDL_Color col1, SDL_Color col2)
    : Effect(EffectType::BOSS_INTRO, pos, 3.0f),
    currentRadius(0.0f),
    maxRadius(maxRad),
    primaryColor(col1),
    secondaryColor(col2),
    pulseTimer(0.0f)
{
    // Tạo particles xoay tròn
    int particleCount = 60;
    for (int i = 0; i < particleCount; i++) {
        float angle = (float)i / particleCount * 6.28318f;
        float distance = maxRadius * 0.5f;

        glm::vec2 particlePos(
            pos.x + cos(angle) * distance,
            pos.y + sin(angle) * distance
        );

        // Velocity hướng ra ngoài
        glm::vec2 velocity(
            cos(angle) * 50.0f,
            sin(angle) * 50.0f
        );

        SDL_Color col = (i % 2 == 0) ? col1 : col2;
        particles.emplace_back(particlePos, velocity, col, 3.0f, 5.0f + (rand() % 8));
    }
}

void BossIntroEffect::Update(float deltaTime) {
    Effect::Update(deltaTime);

    // Tăng bán kính dần dần
    currentRadius += (maxRadius / duration) * deltaTime;
    if (currentRadius > maxRadius) {
        currentRadius = maxRadius;
    }

    pulseTimer += deltaTime * 3.0f;

    // Cập nhật particles
    for (auto& particle : particles) {
        particle.Update(deltaTime);
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return !p.IsAlive(); }),
        particles.end()
    );
}

void BossIntroEffect::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // Vẽ vòng tròn mở rộng với hiệu ứng pulse
    float pulseScale = 1.0f + sin(pulseTimer) * 0.1f;
    float drawRadius = currentRadius * pulseScale;

    // Vẽ vòng tròn ngoài (primary color)
    SDL_Color col1 = primaryColor;
    col1.a = (Uint8)((1.0f - timer / duration) * 180.0f);
    SDL_SetRenderDrawColor(renderer, col1.r, col1.g, col1.b, col1.a);

    int segments = 36;
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 6.28318f;
        float angle2 = (float)(i + 1) / segments * 6.28318f;

        SDL_RenderLine(renderer,
            position.x - cameraOffset.x + cos(angle1) * drawRadius,
            position.y - cameraOffset.y + sin(angle1) * drawRadius,
            position.x - cameraOffset.x + cos(angle2) * drawRadius,
            position.y - cameraOffset.y + sin(angle2) * drawRadius
        );
    }

    // Vẽ vòng tròn trong (secondary color)
    SDL_Color col2 = secondaryColor;
    col2.a = (Uint8)((1.0f - timer / duration) * 150.0f);
    SDL_SetRenderDrawColor(renderer, col2.r, col2.g, col2.b, col2.a);

    float innerRadius = drawRadius * 0.7f;
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 6.28318f;
        float angle2 = (float)(i + 1) / segments * 6.28318f;

        SDL_RenderLine(renderer,
            position.x - cameraOffset.x + cos(angle1) * innerRadius,
            position.y - cameraOffset.y + sin(angle1) * innerRadius,
            position.x - cameraOffset.x + cos(angle2) * innerRadius,
            position.y - cameraOffset.y + sin(angle2) * innerRadius
        );
    }

    // Vẽ particles
    for (const auto& particle : particles) {
        SDL_Color col = particle.color;
        col.a = (Uint8)particle.GetAlpha();
        SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

        SDL_FRect rect = {
            particle.position.x - cameraOffset.x - particle.size / 2,
            particle.position.y - cameraOffset.y - particle.size / 2,
            particle.size,
            particle.size
        };

        SDL_RenderFillRect(renderer, &rect);
    }
}

// ===== BOSS ATTACK EFFECT (Sóng năng lượng) =====
BossAttackEffect::BossAttackEffect(glm::vec2 pos, float maxRad, SDL_Color col)
    : Effect(EffectType::BOSS_ATTACK, pos, 0.8f),
    currentRadius(0.0f),
    maxRadius(maxRad),
    attackColor(col),
    rotationAngle(0.0f)
{
    // Tạo shockwave particles
    int particleCount = 40;
    for (int i = 0; i < particleCount; i++) {
        float angle = (float)i / particleCount * 6.28318f;
        float speed = 200.0f + (rand() % 100);

        glm::vec2 velocity(
            cos(angle) * speed,
            sin(angle) * speed
        );

        shockwave.emplace_back(pos, velocity, col, 0.8f, 4.0f + (rand() % 6));
    }
}

void BossAttackEffect::Update(float deltaTime) {
    Effect::Update(deltaTime);

    currentRadius += (maxRadius / duration) * deltaTime;
    rotationAngle += deltaTime * 5.0f;

    for (auto& particle : shockwave) {
        particle.Update(deltaTime);
    }

    shockwave.erase(
        std::remove_if(shockwave.begin(), shockwave.end(),
            [](const Particle& p) { return !p.IsAlive(); }),
        shockwave.end()
    );

    if (shockwave.empty()) {
        isActive = false;
    }
}

void BossAttackEffect::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // Vẽ sóng năng lượng mở rộng
    SDL_Color col = attackColor;
    col.a = (Uint8)((1.0f - timer / duration) * 200.0f);
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

    int segments = 32;
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 6.28318f + rotationAngle;
        float angle2 = (float)(i + 1) / segments * 6.28318f + rotationAngle;

        SDL_RenderLine(renderer,
            position.x - cameraOffset.x + cos(angle1) * currentRadius,
            position.y - cameraOffset.y + sin(angle1) * currentRadius,
            position.x - cameraOffset.x + cos(angle2) * currentRadius,
            position.y - cameraOffset.y + sin(angle2) * currentRadius
        );
    }

    // Vẽ shockwave particles
    for (const auto& particle : shockwave) {
        SDL_Color pCol = particle.color;
        pCol.a = (Uint8)particle.GetAlpha();
        SDL_SetRenderDrawColor(renderer, pCol.r, pCol.g, pCol.b, pCol.a);

        SDL_FRect rect = {
            particle.position.x - cameraOffset.x - particle.size / 2,
            particle.position.y - cameraOffset.y - particle.size / 2,
            particle.size,
            particle.size
        };

        SDL_RenderFillRect(renderer, &rect);
    }
}

// ===== SCREEN SHAKE =====
ScreenShake::ScreenShake()
    : intensity(0.0f), duration(0.0f), timer(0.0f),
    isShaking(false), offset(0.0f, 0.0f)
{
}

void ScreenShake::Trigger(float intense, float dur) {
    intensity = intense;
    duration = dur;
    timer = 0.0f;
    isShaking = true;
}

void ScreenShake::Update(float deltaTime) {
    if (!isShaking) {
        offset = glm::vec2(0.0f, 0.0f);
        return;
    }

    timer += deltaTime;

    if (timer >= duration) {
        isShaking = false;
        offset = glm::vec2(0.0f, 0.0f);
        return;
    }

    float progress = timer / duration;
    float currentIntensity = intensity * (1.0f - progress);

    offset.x = ((rand() % 200 - 100) / 100.0f) * currentIntensity;
    offset.y = ((rand() % 200 - 100) / 100.0f) * currentIntensity;
}

// ===== EFFECT MANAGER =====
EffectManager::EffectManager() {
}

EffectManager::~EffectManager() {
    Clear();
}

void EffectManager::CreateExplosion(glm::vec2 pos, float radius, SDL_Color color) {
    effects.push_back(std::make_unique<ExplosionEffect>(pos, radius, color));
    std::cout << "Tao hieu ung no tai (" << pos.x << ", " << pos.y << ")\n";
}

void EffectManager::CreateFlash(glm::vec2 pos, SDL_Color color, float intensity, float duration) {
    effects.push_back(std::make_unique<FlashEffect>(pos, color, intensity, duration));
}

void EffectManager::CreateTrail(glm::vec2 pos, SDL_Color color, float width) {
    effects.push_back(std::make_unique<TrailEffect>(pos, color, width));
}

void EffectManager::CreateBossIntro(glm::vec2 pos, float radius) {
    SDL_Color primaryColor = { 255, 50, 50, 255 };   // Đỏ
    SDL_Color secondaryColor = { 255, 200, 0, 255 }; // Vàng
    effects.push_back(std::make_unique<BossIntroEffect>(pos, radius, primaryColor, secondaryColor));
    std::cout << "Boss xuat hien voi hieu ung tai (" << pos.x << ", " << pos.y << ")\n";
}

void EffectManager::CreateBossAttack(glm::vec2 pos, float radius, SDL_Color color) {
    effects.push_back(std::make_unique<BossAttackEffect>(pos, radius, color));
    std::cout << "Boss tan cong voi hieu ung tai (" << pos.x << ", " << pos.y << ")\n";
}

void EffectManager::CreateBigExplosion(glm::vec2 pos, float radius) {
    SDL_Color color = { 255, 100, 0, 255 }; // Cam đỏ
    effects.push_back(std::make_unique<ExplosionEffect>(pos, radius, color));
    std::cout << "Tao hieu ung no lon tai (" << pos.x << ", " << pos.y << ")\n";
}

void EffectManager::TriggerScreenShake(float intensity, float duration) {
    screenShake.Trigger(intensity, duration);
    std::cout << "Rung man hinh voi intensity: " << intensity << "\n";
}

void EffectManager::Update(float deltaTime) {
    screenShake.Update(deltaTime);

    for (auto& effect : effects) {
        effect->Update(deltaTime);
    }

    effects.erase(
        std::remove_if(effects.begin(), effects.end(),
            [](const std::unique_ptr<Effect>& e) { return !e->IsActive(); }),
        effects.end()
    );
}

void EffectManager::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    for (auto& effect : effects) {
        effect->Render(renderer, cameraOffset);
    }
}

void EffectManager::Clear() {
    effects.clear();
}