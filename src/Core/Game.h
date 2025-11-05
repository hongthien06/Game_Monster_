#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <string>
#include<vector>

#include "../Config/GameConstants.h"
#include "../Core/Camera.h"
#include "../Environment/Map.h"
#include "../Entities/Player.h"
#include "../Environment/Item.h"
#include "../Core/Audio.h"


class Map;
class Audio;
class Player;
class Item;
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

    Map* map;      //Để tai map   
    Audio* audio;   //De tai nhac nen
    Camera camera;    
    Player* player;   
    uint64_t prevTime;

    int score = 0;

    //quản lý item
    std::vector<std::unique_ptr<Item>> items; 
    SDL_Texture* coinTex;

    bool loadItemTextures();
    void spawnInitialItems();
    void checkItemCollisions();

    void AddScore(int points) { score += points; }
    int GetScore() const { return score; }
};
