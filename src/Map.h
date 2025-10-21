#pragma once
#include <SDL3/SDL.h>
#include "Game.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

using namespace std;

class Map{
    public:
        Map(SDL_Renderer* renderer);
        
        SDL_FPoint GetSpawnPoint();
        void CleanSpawnTile();

        ~Map();
        void LoadTiles();
        void DrawMap(const glm::vec2& offset); 
        float GetMapWidth() const { return mapData.empty() ? 0 : mapData[0].size() * TILE_SIZE; }
        float GetMapHeight() const { return mapData.size() * TILE_SIZE; }

    private:
        SDL_Renderer* renderer;

        SDL_Texture* grassTile;
        SDL_Texture* dirtTile;
        SDL_Texture* dirtLeft;
        SDL_Texture* dirtRight;

        vector<vector<int>> mapData;
        const int TILE_SIZE = 32;
    };