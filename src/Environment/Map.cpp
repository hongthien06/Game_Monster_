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
    string tilesetPath = mapData["tilesets"][0]["image"].get<string>();
    // Nếu đường dẫn là relative path, thêm đường dẫn thư mục map
    if (tilesetPath[0] != '/' && tilesetPath[1] != ':') {  // Không phải absolute path
        size_t lastSlash = filename.find_last_of("/\\");
        if (lastSlash != string::npos) {
            tilesetPath = filename.substr(0, lastSlash + 1) + tilesetPath;
        }
    }
    
    tilesetTexture = LoadTexture(tilesetPath);
    if (!tilesetTexture) {
        cerr << "Failed to load tileset: " << tilesetPath << endl;
        return false;
    }
    
    // T�nh s? c?t trong tileset
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
        if (layer["type"] == "tilelayer") {   // "tilelayer" l� lo?i layer ch?a d? li?u tile
            TileLayer tileLayer;
            tileLayer.name = layer["name"];   // T�n layer
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
                // Duyet qua properties d? t�m "Player_1"
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

//Ham de check va cham voi nen dat!!
bool Map::checkCollision(const SDL_FRect& playerRect) {
    for (auto& tile : collisions) {
        if (SDL_HasRectIntersectionFloat(&playerRect, &tile.rect)) {
            // Tinh thu cong vung giao nhau cua hai hitbox
            float interLeft   = max(playerRect.x, tile.rect.x);
            float interTop    = max(playerRect.y, tile.rect.y);
            float interRight  = min(playerRect.x + playerRect.w, tile.rect.x + tile.rect.w);
            float interBottom = min(playerRect.y + playerRect.h, tile.rect.y + tile.rect.h);

            float interWidth  = interRight - interLeft;
            float interHeight = interBottom - interTop;

            if (interWidth > 0 && interHeight > 0) {
                float centerX = interLeft + interWidth / 2.0f;
                float centerY = interTop + interHeight / 2.0f;

                cout << "Intersection center: (" << centerX << ", " << centerY << ")\n";  //In ra de de debug ne nha.  in ra tam vung chong lan
                return true;
            }
        }
    }
    return false;
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

                tileID--; // Tiled bat dau tu 1

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

