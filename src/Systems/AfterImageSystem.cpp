#include "AfterImageSystem.h"

void AfterImageSystem::AddImage(std::vector<AfterImage>& list, SDL_Texture* tex,
    const SDL_FRect& src, const SDL_FRect& dst,
    SDL_Color color, bool flipHorizontal) {
    AfterImage img;
    img.texture = tex;
    img.src = src;
    img.dst = dst;
    img.color = color;
    img.lifetime = img.maxLifetime = 0.25f;
    img.flipHorizontal = flipHorizontal;
    list.push_back(img);
}

void AfterImageSystem::Update(std::vector<AfterImage>& list, float deltaTime) {
    for (auto it = list.begin(); it != list.end();) {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0.0f)
            it = list.erase(it);
        else
            ++it;
    }
}

void AfterImageSystem::Render(SDL_Renderer* renderer, const std::vector<AfterImage>& list) {
    for (const auto& img : list) {
        float alpha = img.lifetime / img.maxLifetime;
        Uint8 finalAlpha = static_cast<Uint8>(alpha * img.color.a);

        // Áp dụng hiệu ứng mờ dần
        SDL_SetTextureAlphaMod(img.texture, finalAlpha);
        SDL_SetTextureColorMod(img.texture, img.color.r, img.color.g, img.color.b);

        SDL_RenderTextureRotated(renderer, img.texture, &img.src, &img.dst,
            0, nullptr, img.flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

        // ✅ Khôi phục lại texture để nhân vật chính không bị trong suốt
        SDL_SetTextureAlphaMod(img.texture, 255);
        SDL_SetTextureColorMod(img.texture, 255, 255, 255);
    }
}
