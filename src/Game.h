#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <string>

#include "GameConstants.h"
#include "Camera.h"
#include "Map.h"
#include "Player.h"
class Map;
class Player;
class Game {
public:
    Game();
    ~Game();
    void run();

private:
    bool init();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void cleanup();

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isGameRunning;

    Map* map;         
    Camera camera;    
    Player* player;   
    uint64_t prevTime;
};
