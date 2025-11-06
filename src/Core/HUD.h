#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class HUD {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* coinIconTex;

    struct ScorePopup {
        glm::vec2 position;
        int value;
        float lifetime;
        float alpha;
    };

    int score;
    std::vector<ScorePopup> scorePopups;

public:
    HUD(SDL_Renderer* renderer);
    ~HUD();

    bool LoadResources();
    void AddScore(int amount);
    int GetScore() const { return score; }

    void AddScorePopup(glm::vec2 position, int value);
    void Update(float deltaTime);
    void Render(glm::vec2 cameraOffset);

    void Cleanup();
};
