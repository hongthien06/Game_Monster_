#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>   

#include "../Config/GameConstants.h"
#include "../Core/Camera.h"
#include "../Environment/Map.h"
#include "../Entities/Player.h"
#include "../Environment/Item.h"
#include "../Core/Audio.h"
#include "../Core/HUD.h" 
#include "../Core/GameOverMenu.h"
#include <SDL3_ttf/SDL_ttf.h>

// ===== INCLUDE ENEMY SYSTEM =====
#include "../Entities/Enemy.h"
#include "../Entities/Minion.h"
#include "../Entities/Elite.h"
#include "../Entities/Boss.h"
#include "../Components/Effect.h"

enum class GameState {
    PLAYING,
    GAME_OVER
};

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isGameRunning;

    Map* map;
    Player* player;
    Minions* minion;
    Boss* boss;
    Camera camera;
    Audio* audio;
    HUD* playerHUD;

    std::vector<std::unique_ptr<Item>> items;
    SDL_Texture* coinTex;
    SDL_Texture* healthPotionTex;

    // =====  ENEMY SYSTEM =====
    std::vector<std::unique_ptr<Enemy>> enemies;
    EffectManager effectManager;


    GameState currentGameState;
    glm::vec2 playerStartPos;

    TTF_Font* mainFont; // Font dùng chung (bạn sẽ cần load nó trong init)
    std::unique_ptr<GameOverMenu> gameOverMenu;

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
    void initItems();
    void checkItemCollisions();
    void spawnCoinAtPosition(glm::vec2 pos, int amount);
    void spawnHealthPotionAtPosition(glm::vec2 pos);


    // ===== ENEMY METHODS =====
    void initEnemies();
    void updateEnemies(float deltaTime);
    void checkEnemyCollisions();

private:
    void resetGame();
};