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
#include "../Core/MainMenu.h"
#include "../Core/TutorialMenu.h"
#include "../Core/GameOverMenu.h"
#include "../Core/GameWinMenu.h"
#include <SDL3_ttf/SDL_ttf.h>
#include "../Components/ScreenTransition.h"

// ===== INCLUDE ENEMY SYSTEM =====
#include "../Entities/Enemy.h"
#include "../Entities/Minion.h"
#include "../Entities/Elite.h"
#include "../Entities/Boss.h"
#include "../Components/Effect.h"


enum class GameState {
    MAIN_MENU,
    TRANSITIONING,
    PLAYING,
    GAME_OVER,
    GAME_WIN,
    TUTORIAL,
    MAP_TRANSITIONING  
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

    std::string currentMapName;
    std::string nextMapName;  

    std::vector<std::unique_ptr<Item>> items;
    SDL_Texture* coinTex;
    SDL_Texture* healthPotionTex;

    // =====  ENEMY SYSTEM =====
    std::vector<std::unique_ptr<Enemy>> enemies;
    EffectManager effectManager;


    GameState currentGameState;
    glm::vec2 playerStartPos;

    TTF_Font* mainFont;
    std::unique_ptr<MainMenu> mainMenu;
    std::unique_ptr<GameWinMenu> gameWinMenu;
    std::unique_ptr<GameOverMenu> gameOverMenu;
    std::unique_ptr<TutorialMenu> tutorialMenu;

    std::unique_ptr<ScreenTransition> screenTransition;
    std::unique_ptr<ScreenTransition> mapTransition; 
    GameState targetState;
    std::string debugMap; 
    SDL_Texture* menuBackgroundTex;
    SDL_Texture* gameBackgroundTex;

public:
    Game();
    ~Game();

    bool init();
    void run();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void cleanup();
    bool pendingNextMap;

    bool loadItemTextures();
    void initItems();
    void checkItemCollisions();
    void spawnCoinAtPosition(glm::vec2 pos, int amount);
    void spawnHealthPotionAtPosition(glm::vec2 pos);
    void LoadNextMap();

    void checkProjectileBounds();

    
    void StartMapTransition(const std::string& nextMap, TransitionType type);
    void ExecuteMapChange();

    // ===== ENEMY METHODS =====
    void initEnemies();
    void updateEnemies(float deltaTime);
    void checkEnemyCollisions();

private:
    void resetGame();
    void startTransition(GameState nextState);
};