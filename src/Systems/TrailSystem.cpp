#include "TrailSystem.h"
#include <SDL3_image/SDL_image.h>

SDL_Texture* TrailSystem::trailTexture = nullptr;

void TrailSystem::Init(SDL_Renderer* renderer) {
    trailTexture = IMG_LoadTexture(renderer, "assets/images/Player/Trail.png");
    if (!trailTexture)
        SDL_Log("Failed to load glow_trail.png: %s", SDL_GetError());
}

void TrailSystem::Cleanup() {
    if (trailTexture) {
        SDL_DestroyTexture(trailTexture);
        trailTexture = nullptr;
    }
}

void TrailSystem::AddTrail(std::vector<Trail>& trails, const SDL_FRect& rect, SDL_Color color) {
    Trail t;
    t.rect = rect;
    t.color = color;
    t.maxLifetime = t.lifetime = 0.25f;
    trails.push_back(t);
}

void TrailSystem::Update(std::vector<Trail>& trails, float deltaTime) {
    for (auto it = trails.begin(); it != trails.end(); ) {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0.0f)
            it = trails.erase(it);
        else
            ++it;
    }
}

void TrailSystem::Render(SDL_Renderer* renderer, const std::vector<Trail>& trails) {
    for (const auto& t : trails) {
        if (!trailTexture) continue;

        float alpha = (t.lifetime / t.maxLifetime);
        SDL_SetTextureAlphaMod(trailTexture, static_cast<Uint8>(alpha * 255));
        SDL_SetTextureColorMod(trailTexture, t.color.r, t.color.g, t.color.b);

        SDL_RenderTexture(renderer, trailTexture, nullptr, &t.rect);
    }
}
