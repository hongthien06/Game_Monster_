#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <glm/glm.hpp>  // ✅ thêm để dùng glm::vec2 cho offset

struct AfterImage {
    SDL_Texture* texture;
    SDL_FRect src;
    SDL_FRect dst;
    SDL_Color color;
    float lifetime;
    float maxLifetime;
    bool flipHorizontal;
};

class AfterImageSystem {
public:
    static void AddImage(std::vector<AfterImage>& list, SDL_Texture* tex,
        const SDL_FRect& src, const SDL_FRect& dst,
        SDL_Color color, bool flipHorizontal);

    static void Update(std::vector<AfterImage>& list, float deltaTime);

    // ✅ thêm offset camera để vẽ đúng vị trí trên màn hình
    static void Render(SDL_Renderer* renderer, const std::vector<AfterImage>& list, const glm::vec2& offset);
};
