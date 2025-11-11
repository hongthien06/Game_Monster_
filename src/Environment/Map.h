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
    bool checkCollision(const SDL_FRect& playerRect);    // Kiem tra va cham voi dat nha Duc Anh
    
    void drawCollisionDebug(const glm::vec2& cameraOffset);  // Ve hitbox quanh cac o dat
    const std::vector<SDL_FRect> GetCollisionTiles() const;
private:
    // ===== Internal helper functions =====
    SDL_Texture* LoadTexture(const string& file);
    void LoadTileLayers(const json& mapData);
    void LoadTileCollisions(const json& mapData); // <-- Them dong nay
    void LoadCollisionLayer(const json& mapData);
    void LoadObjects(const json& mapData);
    bool PointInPolygon(const SDL_FPoint& p, const std::vector<SDL_FPoint>& poly);

private:
    // ===== SDL / Rendering =====
    SDL_Renderer* renderer;
    SDL_Texture* tilesetTexture;
    SDL_Texture* bgFar = nullptr;     // lớp xa (màu xanh tím)
    SDL_Texture* bgMid = nullptr;     // lớp giữa (rừng tối)
    SDL_Texture* bgNear = nullptr;    // lớp gần (cây to nhiều chi tiết)

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

    // Cac polygon va cham (cho tile nghieng, hinh phuc tap)
    vector<std::vector<SDL_FPoint>> collisionPolygons;

    // Bản đồ lưu polygon gốc theo tileID trong tileset
    unordered_map<int, std::vector<SDL_FPoint>> tilePolygons;
};
