#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <vector>
#include "../Config/GameConstants.h"

class Projectile {
protected:
    // ===== VỊ TRÍ VÀ CHUYỂN ĐỘNG =====
    glm::vec2 position;
    glm::vec2 velocity;
    float speed;
    float rotation;

    // ===== COMBAT =====
    int damage;
    bool isActive;

    // ===== LIFETIME =====
    float lifetime;
    float lifeTimer;

    // ===== RENDERING =====
    SDL_Texture* texture;
    SDL_Renderer* renderer;

    // ===== TRAIL EFFECT =====
    struct TrailPoint {
        glm::vec2 position;
        float alpha;
        float size;
    };

    std::vector<TrailPoint> trailPoints;
    bool hasTrail;
    SDL_Color trailColor;
    float trailSpawnRate;
    float trailSpawnTimer;
    int maxTrailPoints;

public:
    Projectile(SDL_Renderer* renderer, glm::vec2 startPos,
        glm::vec2 direction, float speed, int damage);

    virtual ~Projectile();
    virtual void Update(float deltaTime);
    virtual void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);

    // ===== GETTERS =====
    bool IsActive() const { return isActive; }
    glm::vec2 GetPosition() const { return position; }
    int GetDamage() const { return damage; }
    float GetRotation() const { return rotation; }

    // ===== THÊM MỚI: BoundingBox và Deactivate =====
    SDL_FRect GetBoundingBox() const {
        return SDL_FRect{
            position.x - 8.0f,
            position.y - 4.0f,
            16.0f,
            8.0f
        };
    }

    void Deactivate() {
        isActive = false;
    }

    // ===== SETTERS =====
    void SetTrailEnabled(bool enabled) { hasTrail = enabled; }
    void SetTrailColor(SDL_Color color) { trailColor = color; }

    void UpdateTrail(float deltaTime);
    void RenderTrail(SDL_Renderer* renderer, glm::vec2 cameraOffset);
    void CalculateRotation();
};