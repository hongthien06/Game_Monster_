//#pragma once
//#pragma once
//#include <vector>
//#include <memory>
//#include <SDL3/SDL.h>
//#include "Item.h"
//#include "../Entities/Player.h"
//
//class ItemManager {
//private:
//    std::vector<std::unique_ptr<Item>> items;
//    SDL_Texture* coinTex; 
//    //health
//
//public:
//    ItemManager();
//    ~ItemManager();
//    bool LoadTextures(SDL_Renderer* renderer);
//    void SpawnInitialItems();
//    void Update(float deltaTime);
//    void Render(SDL_Renderer* renderer, glm::vec2 cameraOffset);
//
//    // Xử lý va chạm 
//    void CheckCollisions(Player& player);
//
//    // Getter để Game lấy số lượng item
//    size_t GetItemCount() const { return items.size(); }
//};