#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Map.h"
#include "Game.h"
#include "Camera.h"
#include <iostream>

using namespace std;

//Constructor
Map::Map(SDL_Renderer* renderer) : renderer(renderer) {
    grassTile = nullptr;
    dirtTile = nullptr;
    dirtLeft = nullptr;
    dirtRight = nullptr;
}

//Destructor
Map::~Map() {
    if (grassTile) {
        SDL_DestroyTexture(grassTile);
    }
    if (dirtTile) {
        SDL_DestroyTexture(dirtTile);
    }
    if (dirtLeft) {
        SDL_DestroyTexture(dirtLeft);
    }
    if (dirtRight) {
        SDL_DestroyTexture(dirtRight);
    }
}

//Load Tiles and Map Data
void Map::LoadTiles(){
    grassTile = IMG_LoadTexture(renderer, "Asset/tileset/Tile_02.png");
    if (!grassTile) {
        cerr << "Failed to load grass tile: " << SDL_GetError() << endl;
    }
    dirtLeft = IMG_LoadTexture(renderer, "Asset/tileset/Tile_47.png");
    if (!dirtLeft) {
        cerr << "Failed to load dirt left tile: " << SDL_GetError() << endl;
    }

    dirtRight = IMG_LoadTexture(renderer, "Asset/tileset/Tile_48.png");
    if (!dirtRight) {
        cerr << "Failed to load dirt right tile: " << SDL_GetError() << endl;
    }

    dirtTile = IMG_LoadTexture(renderer, "Asset/tileset/Tile_12.png");
    if (!dirtTile) {
        cerr << "Failed to load dirt tile: " << SDL_GetError() << endl;
    }

    // Ví dụ dữ liệu bản đồ
    mapData = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 4, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 4, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 5, 0, 0, 0, 0, 4, 1, 1, 1, 5, 0, 0, 0, 0, 0, 4, 1, 5, 0, 0, 0, 0, 4, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 4, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 5, 0, 0, 4, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    
    /* 
        0 - Empty
        1 - Tile_02
        2 - Tile_12
        3 - Player
        4 - dirtLetf
        5 - dirtRight
    */

    cout << "DA TAI MAP THANH CONG" << endl;
}

//Get Player Spawn Point
SDL_FPoint Map::GetSpawnPoint() {
    SDL_FPoint pos = {0, 0};

    for (int y = 0; y < (int)mapData.size(); y++) {
        for (int x = 0; x < (int)mapData[y].size(); x++) {
            if (mapData[y][x] == 3) { // 3 = vị trí nhân vật
                pos.x = x * TILE_SIZE;
                pos.y = y * TILE_SIZE;
                return pos;
            }
        }
    }
    return pos; // nếu không tìm thấy, trả về (0,0)
}

//Clean Spawn Tile
void Map::CleanSpawnTile(){
    for (int y = 0; y < (int)mapData.size(); y++) {
        for (int x = 0; x < (int)mapData[y].size(); x++) {
            if (mapData[y][x] == 3) { // 3 = vị trí nhân vật
                mapData[y][x] = 0; // Xóa vị trí nhân vật trên bản đồ
                return;
            }
        }
    }
}

//Draw Map 
void Map::DrawMap(const glm::vec2& offset) {
    int tileSize = TILE_SIZE;
    float mapHeight = mapData.size() * tileSize;
    float yOffset = GameConstants::LOGICAL_HEIGHT - mapHeight;

    for (int y = 0; y < (int)mapData.size(); y++) {
        for (int x = 0; x < (int)mapData[y].size(); x++) {

            // Tính vị trí thật của từng tile (trừ offset camera)
            SDL_FRect dest = {
                static_cast<float>(x * tileSize) - offset.x,
                static_cast<float>(y * tileSize + yOffset) - offset.y,
                static_cast<float>(tileSize),
                static_cast<float>(tileSize)
            };

            switch (mapData[y][x]) {
                case 1:
                    SDL_RenderTexture(renderer, grassTile, nullptr, &dest);
                    break;
                case 2:
                    SDL_RenderTexture(renderer, dirtTile, nullptr, &dest);
                    break;
                case 4:
                    SDL_RenderTexture(renderer, dirtLeft, nullptr, &dest);
                    break;
                case 5:
                    SDL_RenderTexture(renderer, dirtRight, nullptr, &dest);
                    break;    
            }
        }
    }
}