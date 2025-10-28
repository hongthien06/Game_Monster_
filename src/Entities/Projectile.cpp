#include <iostream>
#include <cmath>
#include <algorithm>
#include "Character.h"
#include "../Config/GameConstants.h"
#include "Projectile.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace std;

Projectile::Projectile(SDL_Renderer* renderer, glm::vec2 startPos,
    glm::vec2 direction, float speed, int damage)
    : position(startPos),
    velocity(direction* speed),
    speed(speed),
    rotation(0.0f),
    damage(damage),
    isActive(true),
    lifetime(3.0f),
    lifeTimer(0.0f),
    renderer(renderer),
    hasTrail(true),
    trailColor{ 255, 200, 100, 255 },
    trailSpawnRate(0.02f),
    trailSpawnTimer(0.0f),
    maxTrailPoints(15)
{
    // Load texture
    texture = IMG_LoadTexture(renderer, "assets/images/Player/Arrow.png");
    if (!texture) {
        std::cerr << "ERROR: Cannot load arrow texture!\n";
        std::cerr << "Path: assets/images/Player/Arrow.png\n";
    }
    else {
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    }

    CalculateRotation();
}

Projectile::~Projectile() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Projectile::CalculateRotation() {
    rotation = std::atan2(velocity.y, velocity.x) * (180.0f / M_PI);
}

void Projectile::Update(float deltaTime) {
    if (!isActive) return;

    // Update position
    position += velocity * deltaTime;

    CalculateRotation();

    // Update trail
    if (hasTrail) {
        UpdateTrail(deltaTime);
    }

    // Check lifetime
    lifeTimer += deltaTime;
    if (lifeTimer >= lifetime) {
        isActive = false;
        return;
    }

    // Check out of bounds
    const float MAP_BUFFER = 100.0f;
    if (position.x < -MAP_BUFFER ||
        position.x > GameConstants::WORLD_WIDTH + MAP_BUFFER ||
        position.y < -MAP_BUFFER ||
        position.y > GameConstants::WORLD_HEIGHT + MAP_BUFFER) {
        isActive = false;
    }
}

void Projectile::UpdateTrail(float deltaTime) {
    // Spawn new trail point
    trailSpawnTimer += deltaTime;
    if (trailSpawnTimer >= trailSpawnRate) {
        trailSpawnTimer = 0.0f;

        TrailPoint newPoint;
        newPoint.position = position;
        newPoint.alpha = 1.0f;
        newPoint.size = 4.0f;

        trailPoints.push_back(newPoint);

        if (trailPoints.size() > maxTrailPoints) {
            trailPoints.erase(trailPoints.begin());
        }
    }

    // Update existing points
    for (auto& point : trailPoints) {
        point.alpha -= deltaTime * 2.5f;
        point.size -= deltaTime * 8.0f;
    }

    // Remove faded points
    trailPoints.erase(
        std::remove_if(trailPoints.begin(), trailPoints.end(),
            [](const TrailPoint& p) {
                return p.alpha <= 0.0f || p.size <= 0.0f;
            }),
        trailPoints.end()
    );
}

void Projectile::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    if (!isActive) return;

    // Render trail first
    if (hasTrail) {
        RenderTrail(renderer, cameraOffset);
    }

    // Render arrow
    if (!texture) return;

    SDL_FRect dstRect = {
        position.x - cameraOffset.x - 8.0f,
        position.y - cameraOffset.y - 4.0f,
        16.0f,
        8.0f
    };

    SDL_FPoint center = { 8.0f, 4.0f };

    SDL_RenderTextureRotated(
        renderer,
        texture,
        nullptr,
        &dstRect,
        rotation,
        &center,
        SDL_FLIP_NONE
    );
}

void Projectile::RenderTrail(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (size_t i = 0; i < trailPoints.size(); ++i) {
        const TrailPoint& point = trailPoints[i];

        float ratio = (float)i / (float)trailPoints.size();
        int alpha = (int)(point.alpha * 255.0f * ratio);
        float size = point.size * ratio;

        if (alpha <= 0 || size <= 0) continue;

        SDL_SetRenderDrawColor(renderer,
            trailColor.r,
            trailColor.g,
            trailColor.b,
            alpha
        );

        SDL_FRect trailRect = {
            point.position.x - cameraOffset.x - size / 2.0f,
            point.position.y - cameraOffset.y - size / 2.0f,
            size,
            size
        };

        SDL_RenderFillRect(renderer, &trailRect);
    }
}