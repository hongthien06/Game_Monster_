#pragma once
#include <SDL3/SDL.h>
#include <vector>

struct Trail {
    SDL_FRect rect;
    float lifetime;
    float maxLifetime;
    SDL_Color color;
};

class TrailSystem {
public:
    static void Init(SDL_Renderer* renderer);
    static void AddTrail(std::vector<Trail>& trails, const SDL_FRect& rect, SDL_Color color);
    static void Update(std::vector<Trail>& trails, float deltaTime);
    static void Render(SDL_Renderer* renderer, const std::vector<Trail>& trails);
    static void Cleanup();

private:
    static SDL_Texture* trailTexture;
};
