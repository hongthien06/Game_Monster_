#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>  // ← THÊM: Cho std::unique_ptr

#include "../Config/GameConstants.h"
#include "../Core/Camera.h"
#include "../Environment/Map.h"
#include "../Entities/Player.h"
#include "../Environment/Item.h"
#include "../Core/Audio.h"
#include "../Core/HUD.h" 
#include <SDL3_ttf/SDL_ttf.h>

// ===== THÊM MỚI: INCLUDE ENEMY SYSTEM =====
#include "../Entities/Enemy.h"
#include "../Entities/Minion.h"
#include "../Entities/Elite.h"
#include "../Entities/Boss.h"
#include "../Components/Effect.h"

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isGameRunning;

    Map* map;
    Player* player;
    Camera camera;
    Audio* audio;
    HUD* playerHUD;

    std::vector<std::unique_ptr<Item>> items;
    SDL_Texture* coinTex;

    // ===== THÊM MỚI: ENEMY SYSTEM =====
    std::vector<std::unique_ptr<Enemy>> enemies;
    EffectManager effectManager;

public:
    Game();
    ~Game();

    bool init();
    void run();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void cleanup();

    bool loadItemTextures();
   // void spawnInitialItems();
    void checkItemCollisions();
    void spawnCoinAtPosition(glm::vec2 pos, int amount);


    // ===== THÊM MỚI: ENEMY METHODS =====
    void initEnemies();
    void updateEnemies(float deltaTime);
    void checkEnemyCollisions();
};