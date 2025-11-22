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
    minion1_Spawns;
    minion2_Spawns;
    minion3_Spawns;
    elite1_Spawns;
    elite2_Spawns;
    elite3_Spawns;
    boss_Spawn = {0, 0};
    Heal_Spawn;
    Coin_Spawn;
    bgFar  = IMG_LoadTexture(renderer, "assets/images/Layers/1.png");
    bgMid  = IMG_LoadTexture(renderer, "assets/images/Layers/2.png");
    bgNear = IMG_LoadTexture(renderer, "assets/images/Layers/3.png");
}

Map::~Map() {
    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = nullptr;
    }

    if (bgFar && bgMid && bgNear) {
    SDL_DestroyTexture(bgFar);
    SDL_DestroyTexture(bgMid);
    SDL_DestroyTexture(bgNear);
    bgFar = nullptr;
    bgMid = nullptr;
    bgNear = nullptr;
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

    if (tilesetPath[0] != '/' && tilesetPath[1] != ':') {  
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
    
    
    int imageWidth = mapData["tilesets"][0]["imagewidth"];
    tilesetColumns = imageWidth / tileWidth;

    // Load layers
    LoadTileLayers(mapData);
    LoadTileCollisions(mapData);   
    LoadCollisionLayer(mapData);
    LoadObjects(mapData);


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
        if (layer["type"] == "tilelayer") {   
            TileLayer tileLayer;
            tileLayer.name = layer["name"];   
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
                    if (tileID == 0) continue;

                   
                    int localID = tileID - 1;

                   
                    if (tilePolygons.count(localID)) {
                        auto poly = tilePolygons[localID];

                        
                        for (auto& p : poly) {
                            p.x += x * tileWidth;
                            p.y += y * tileHeight;
                        }

                        // Lưu vào danh sách collision polygon
                        collisionPolygons.push_back(poly);
                    } 
                    // Luu hinh chu nhat con vao trong danh sach tileBoxes
                    else if (tileBoxes.count(localID)) {
                        auto box = tileBoxes[localID].rect;
                        box.x += x * tileWidth;
                        box.y += y * tileHeight;
                        collisions.push_back({ box });
                    }
                    else {
                        // Nếu không có polygon thì dùng full hình chữ nhật như cũ
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
    } 
}

void Map::LoadObjects(const json& mapData) {
    for (auto& layer : mapData["layers"]) {
        if (layer["type"] == "objectgroup" && layer["name"] == "Spawn") {
            for (auto& obj : layer["objects"]) {
                if (obj.contains("properties")) {
                    for (auto& prop : obj["properties"]) {
                        std::string propName = prop["name"];
                        float x = obj["x"];
                        float y = obj["y"];

                        if (propName == "Player_1") {
                            playerSpawn = {x, y};
                        }
                        else if (propName == "Elite_1") {
                            elite1_Spawns.push_back({x, y});
                        }
                        else if (propName == "Elite_2") {
                            elite2_Spawns.push_back({x, y});
                        }
                        else if (propName == "Elite_3") {
                            elite3_Spawns.push_back({x, y});
                        }
                        else if (propName == "Minion_1") {
                            minion1_Spawns.push_back({x, y});
                        }
                        else if (propName == "Minion_2") {
                            minion2_Spawns.push_back({x, y});
                        }
                        else if (propName == "Minion_3") {
                            minion3_Spawns.push_back({x, y});
                        }
                        else if (propName == "Boss_") {
                            boss_Spawn = {x, y};
                        }
                        else if (propName == "Heal_") {
                            Heal_Spawn.push_back({x, y});
                        }
                        else if (propName == "Coin_") {
                            Coin_Spawn.push_back({x, y});
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

               
                return true;
            }
        }
    }
     
     // Kiểm tra collision với polygon
    for (auto& poly : collisionPolygons) {
        // kiểm tra 4 góc của player có nằm trong polygon không
        SDL_FPoint corners[4] = {
            { playerRect.x, playerRect.y },
            { playerRect.x + playerRect.w, playerRect.y },
            { playerRect.x, playerRect.y + playerRect.h },
            { playerRect.x + playerRect.w, playerRect.y + playerRect.h }
        };

        for (auto& corner : corners) {
            if (PointInPolygon(corner, poly)) {
                return true;
            }
        }
    }


    return false;
}

bool Map::PointInPolygon(const SDL_FPoint& p, const std::vector<SDL_FPoint>& poly) {
    bool inside = false;
    for (size_t i = 0, j = poly.size() - 1; i < poly.size(); j = i++) {
        bool intersect = ((poly[i].y > p.y) != (poly[j].y > p.y)) &&
            (p.x < (poly[j].x - poly[i].x) * (p.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x);
        if (intersect)
            inside = !inside;
    }
    return inside;
}


// ===================== GET PLAYER SPAWN =====================

vector<SDL_FPoint> Map::GetSpawn(int x) {
    if (x == 0)
      return {playerSpawn};
    else if (x == 1)
      return minion1_Spawns;
    else if (x == 2)
      return minion2_Spawns;
    else if (x == 3)
      return minion3_Spawns;  
    else if (x == 4)
      return elite1_Spawns;  
    else if (x == 5)
      return elite2_Spawns;
    else if (x == 6)
      return elite3_Spawns;
    else if (x == 7)
      return {boss_Spawn};
    else if (x == 8)
      return Heal_Spawn;
    else if (x == 9)
      return Coin_Spawn;      
    return {{0.0f,0.0f}};  
}

// ===================== DRAW =====================

void Map::drawMap(const glm::vec2& cameraOffset) {

    if (!tilesetTexture) {
    return;
}

    auto drawParallax = [&](SDL_Texture* tex, float factor) {
        if (!tex) return;

        float texW, texH;
        SDL_GetTextureSize(tex, &texW, &texH);

        // offset theo tỉ lệ parallax
        float offsetX = -cameraOffset.x * factor;

        
        float x = fmod(offsetX, (float)texW);
        if (x > 0) x -= texW;

        for (; x < 400; x += texW) { 
            SDL_FRect dst = { x, 0, (float)texW, (float)texH };
            SDL_RenderTexture(renderer, tex, NULL, &dst);
        }
    };

    // thứ tự từ xa đến gần
    drawParallax(bgFar,  0.2f);
    drawParallax(bgMid,  0.5f);
    drawParallax(bgNear, 0.8f);



    for (const auto& layer : tileLayers) {
        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                int tileID = layer.data[y * layer.width + x];
                if (tileID == 0) continue; 

                tileID--; 

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
                SDL_RenderTexture(renderer, tilesetTexture, &src, &dst);
            }
        }
    }
}


void Map::drawCollisionDebug(const glm::vec2& cameraOffset) {
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);

    for (auto& box : collisions) {
        SDL_FRect r = {
            box.rect.x - cameraOffset.x,
            box.rect.y - cameraOffset.y,
            box.rect.w,
            box.rect.h
        };
        SDL_RenderRect(renderer, &r);
    }

    for (auto& poly : collisionPolygons) {
        if (poly.size() < 2) continue;
        for (size_t i = 0; i < poly.size(); ++i) {
            SDL_FPoint p1 = {
                poly[i].x - cameraOffset.x,
                poly[i].y - cameraOffset.y
            };
            SDL_FPoint p2 = {
                poly[(i + 1) % poly.size()].x - cameraOffset.x,
                poly[(i + 1) % poly.size()].y - cameraOffset.y
            };
            SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
        }
    }
}

void Map::LoadTileCollisions(const json& mapData) {
    // Kiểm tra tileset có chứa thông tin collision không
    if (!mapData["tilesets"][0].contains("tiles")) {
        return;
    }

    for (auto& tile : mapData["tilesets"][0]["tiles"]) {
        int tileId = tile["id"];
        if (!tile.contains("objectgroup")) continue;

        for (auto& obj : tile["objectgroup"]["objects"]) {

            // Nếu là polygon
            if (obj.contains("polygon")) {
                std::vector<SDL_FPoint> pts;
                float baseX = obj["x"];
                float baseY = obj["y"];

                for (auto& p : obj["polygon"]) {
                    pts.push_back({
                        baseX + (float)p["x"],
                        baseY + (float)p["y"]
                    });
                }

                tilePolygons[tileId] = pts;
            }

            // Nếu là rectangle (x, y, width, height)
            else if (obj.contains("width") && obj.contains("height")) {
                CollisionBox box;
                box.rect.x = obj["x"];
                box.rect.y = obj["y"];
                box.rect.w = obj["width"];
                box.rect.h = obj["height"];

                tileBoxes[tileId] = box; 
            }
        }
    }
}
const std::vector<SDL_FRect> Map::GetCollisionTiles() const {
    std::vector<SDL_FRect> rects;
    rects.reserve(collisions.size());
    for (const auto& c : collisions)
        rects.push_back(c.rect);
    return rects;
}
