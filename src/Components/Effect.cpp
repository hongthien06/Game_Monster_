#include "Effect.h"
#include <cmath>
#include <iostream>

// ===== EXPLOSION EFFECT =====
ExplosionEffect::ExplosionEffect(glm::vec2 pos, float rad, SDL_Color col)
    : Effect(EffectType::EXPLOSION, pos, 1.0f), radius(rad), color(col)
{
    // T?o các h?t n?
    int particleCount = 30;
    for (int i = 0; i < particleCount; i++) {
        float angle = (float)i / particleCount * 6.28318f; // 2*PI
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

    // C?p nh?t các h?t
    for (auto& particle : particles) {
        particle.Update(deltaTime);
    }

    // Xóa các h?t ?ã h?t lifetime
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return !p.IsAlive(); }),
        particles.end()
    );

    // H?t h?t = h?t hi?u ?ng
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

    // Gi?m intensity theo th?i gian
    float progress = timer / duration;
    intensity = (1.0f - progress) * intensity;
}

void FlashEffect::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // V? hình ch? nh?t ph? toàn màn hình v?i alpha gi?m d?n
    SDL_Color col = flashColor;
    col.a = (Uint8)(intensity * 255.0f);

    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_RenderFillRect(renderer, nullptr); // Full screen
}

// ===== TRAIL EFFECT =====
TrailEffect::TrailEffect(glm::vec2 startPos, SDL_Color color, float w)
    : Effect(EffectType::TRAIL, startPos, 0.5f), trailColor(color), width(w)
{
    trailPoints.push_back(startPos);
}

void TrailEffect::AddPoint(glm::vec2 point) {
    trailPoints.push_back(point);

    // Gi?i h?n s? ?i?m
    if (trailPoints.size() > 20) {
        trailPoints.erase(trailPoints.begin());
    }
}

void TrailEffect::Update(float deltaTime) {
    Effect::Update(deltaTime);

    // Xóa d?n các ?i?m
    if (!trailPoints.empty() && (rand() % 100) < 30) {
        trailPoints.erase(trailPoints.begin());
    }

    if (trailPoints.empty()) {
        isActive = false;
    }
}

void TrailEffect::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    if (trailPoints.size() < 2) return;

    // V? các ?o?n th?ng n?i các ?i?m
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

    // Gi?m intensity theo th?i gian
    float progress = timer / duration;
    float currentIntensity = intensity * (1.0f - progress);

    // Random offset
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

void EffectManager::TriggerScreenShake(float intensity, float duration) {
    screenShake.Trigger(intensity, duration);
    std::cout << "Rung man hinh voi intensity: " << intensity << "\n";
}

void EffectManager::Update(float deltaTime) {
    // C?p nh?t screen shake
    screenShake.Update(deltaTime);

    // C?p nh?t các effect
    for (auto& effect : effects) {
        effect->Update(deltaTime);
    }

    // Xóa các effect không active
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