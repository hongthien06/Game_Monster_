#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Map.h"
#include <fstream>
#include <iostream>

using namespace std;

// ===================== CONSTRUCTOR / DESTRUCTOR =====================

Map::Map(SDL_Renderer* renderer)
    : renderer(renderer), tilesetTexture(nullptr),
      tileWidth(0), tileHeight(0),
      mapWidth(0), mapHeight(0),
      tilesetColumns(0)
{
    playerSpawn = {0, 0};
}

Map::~Map() {
    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = nullptr;
    }
}

// ===================== LOAD MAP =====================

bool Map::loadMap(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open map file: " << filename << endl;
        return false;
    }

    json mapData;
    file >> mapData;
    file.close();

    tileWidth = mapData["tilewidth"];
    tileHeight = mapData["tileheight"];
    mapWidth = mapData["width"];
    mapHeight = mapData["height"];

    // Load tileset
     std::filesystem::path mapDir = std::filesystem::path(filename).parent_path();
    std::filesystem::path tilesetFullPath = mapDir / mapData["tilesets"][0]["image"].get<std::string>();

    tilesetTexture = LoadTexture(tilesetFullPath.string());
    if (!tilesetTexture) {
        cerr << "Failed to load tileset: " << tilesetFullPath.string() << endl;
        return false;
    }
    
    // Tính s? c?t trong tileset
    int imageWidth = mapData["tilesets"][0]["imagewidth"];
    tilesetColumns = imageWidth / tileWidth;

    // Load layers
    LoadTileLayers(mapData);
    LoadCollisionLayer(mapData);
    LoadObjects(mapData);

    cout << "Map loaded successfully: " << filename << endl;
    cout << "Tile size: " << tileWidth << "x" << tileHeight << endl;
    cout << "Map size: " << mapWidth << "x" << mapHeight << endl;

    return true;
}


// ===================== INTERNAL LOADERS =====================

SDL_Texture* Map::LoadTexture(const string& file) {
    SDL_Texture* tex = IMG_LoadTexture(renderer, file.c_str());
    if (!tex)
        cerr << "IMG_LoadTexture failed: " << SDL_GetError() << endl;
    return tex;
}

void Map::LoadTileLayers(const json& mapData) {
    for (auto& layer : mapData["layers"]) {
        if (layer["type"] == "tilelayer") {   // "tilelayer" là lo?i layer ch?a d? li?u tile
            TileLayer tileLayer;
            tileLayer.name = layer["name"];   // Tên layer
            tileLayer.width = layer["width"];
            tileLayer.height = layer["height"];
            tileLayer.data = layer["data"].get<vector<int>>();
            tileLayers.push_back(tileLayer);
        }
    }
}

void Map::LoadCollisionLayer(const json& mapData) {
    for (auto& layer : mapData["layers"]) {
        if (layer["name"] == "Va_Cham") {
            int w = layer["width"];
            int h = layer["height"];
            auto data = layer["data"].get<vector<int>>();

            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    int tileID = data[y * w + x];
                    if (tileID != 0) {
                        CollisionBox box;
                        box.rect = {
                            static_cast<float>(x * tileWidth),
                            static_cast<float>(y * tileHeight),
                            static_cast<float>(tileWidth),
                            static_cast<float>(tileHeight)
                        };
                        collisions.push_back(box);
                    }
                }
            }
        }
    }

    if (collisions.empty()) {
        cerr << "No collision layer found in map file!" << endl;
    } else {
        cout << "Loaded " << collisions.size() << " collision boxes." << endl;
    }
}

void Map::LoadObjects(const json& mapData) {
    for (auto& layer : mapData["layers"]) {
        if (layer["type"] == "objectgroup" && layer["name"] == "Spawn") {
            for (auto& obj : layer["objects"]) {
                // Duy?t qua properties d? tìm "Player_1"
                if (obj.contains("properties")) {
                    for (auto& prop : obj["properties"]) {
                        if (prop["name"] == "Player_1") {
                            playerSpawn.x = obj["x"];
                            playerSpawn.y = obj["y"];
                            return;
                        }
                    }
                }
            }
        }
    }
}

// ===================== GET PLAYER SPAWN =====================

SDL_FPoint Map::GetPlayerSpawn() {
    return playerSpawn;
}

// ===================== DRAW =====================

void Map::drawMap(const glm::vec2& cameraOffset) {

    if (!tilesetTexture) {
    std::cout << "Tileset texture is NULL!\n";
    return;
}

    for (const auto& layer : tileLayers) {
        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                int tileID = layer.data[y * layer.width + x];
                if (tileID == 0) continue; // Tile r?ng

                tileID--; // Tiled b?t d?u t? 1

                SDL_FRect src{
                    static_cast<float>((tileID % tilesetColumns) * tileWidth),
                    static_cast<float>((tileID / tilesetColumns) * tileHeight),
                    static_cast<float>(tileWidth),
                    static_cast<float>(tileHeight)
                };

                SDL_FRect dst{
                    static_cast<float>(x * tileWidth) - cameraOffset.x,
                    static_cast<float>(y * tileHeight) - cameraOffset.y,
                    static_cast<float>(tileWidth),
                    static_cast<float>(tileHeight)
                };

                if (SDL_RenderTexture(renderer, tilesetTexture, &src, &dst) < 0) {
                 cout << "Render error: " << SDL_GetError() << endl;
                }

                SDL_RenderTexture(renderer, tilesetTexture, &src, &dst);
            }
        }
    }
}

