//#include "ItemManager.h"
//
//
//ItemManager::loadItemTextures() {
//    coinTex = IMG_LoadTexture(renderer, "assets/items/coin3_20x20.png");
//    if (!coinTex) {
//        std::cerr << "ERROR: Khong the load Coin texture: " << SDL_GetError() << "\n";
//        return false;
//    }
//    SDL_SetTextureScaleMode(coinTex, SDL_SCALEMODE_NEAREST);
//    return true;
//}
//
//// Thêm hàm tạo Item
//void Game::spawnInitialItems() {
//    items.push_back(std::make_unique<Item>(glm::vec2(200.0f, 400.0f), coinTex, ItemType::COIN));
//    items.push_back(std::make_unique<Item>(glm::vec2(300.0f, 400.0f), coinTex, ItemType::COIN));
//    items.push_back(std::make_unique<Item>(glm::vec2(400.0f, 400.0f), coinTex, ItemType::COIN));
//}