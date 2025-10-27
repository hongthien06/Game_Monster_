#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;
using namespace std;

// ===================== STRUCT DEFINITIONS =====================

// Đại diện cho một layer chứa tile
struct TileLayer {
    string name;
    int width;
    int height;
    vector<int> data;
};

// Hộp va chạm
struct CollisionBox {
    SDL_FRect rect;
};

// ===================== CLASS MAP =====================

class Map {
public:
    // ===== Constructor / Destructor =====
    Map(SDL_Renderer* renderer);
    ~Map();

    // ===== Main functions =====
    bool loadMap(const string& filename);
    void drawMap(const glm::vec2& cameraOffset);

    // ===== Getter =====
    SDL_FPoint GetPlayerSpawn();

private:
    // ===== Internal helper functions =====
    SDL_Texture* LoadTexture(const string& file);
    void LoadTileLayers(const json& mapData);
    bool checkCollision(const SDL_FRect& playerRect);    // Kiem tra va cham voi dat nha Duc Anh
    void LoadCollisionLayer(const json& mapData);
    void LoadObjects(const json& mapData);

private:
    // ===== SDL / Rendering =====
    SDL_Renderer* renderer;
    SDL_Texture* tilesetTexture;

    // ===== Tile / Map properties =====
    int tileWidth;
    int tileHeight;
    int mapWidth;
    int mapHeight;
    int tilesetColumns;

    // ===== Game data =====
    SDL_FPoint playerSpawn;
    vector<TileLayer> tileLayers;
    vector<CollisionBox> collisions;
};
