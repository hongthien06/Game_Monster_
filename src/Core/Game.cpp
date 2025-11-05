#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Game.h"
#include "../Environment/Map.h"
#include "Camera.h"
#include "../Entities/Player.h"
#include "../Core/Audio.h"
#include <iostream>
#include "../Environment/Item.h"
#include <algorithm>


// Khoi tao
Game::Game()
    : window(nullptr),
    renderer(nullptr),
    isGameRunning(true),
    map(nullptr),
    player(nullptr),

    camera(
        GameConstants::LOGICAL_WIDTH,
        GameConstants::LOGICAL_HEIGHT,
        GameConstants::WORLD_WIDTH,
        GameConstants::WORLD_HEIGHT),
    coinTex(nullptr)
{
}

// Giai phong tai nguyen
Game::~Game() {
    cleanup();
}
// Khoi tao SDL, Renderer, cua so va nhan vat
bool Game::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }


    // Khoi tao cua so 
    window = SDL_CreateWindow("Game Monster",
        GameConstants::SCREEN_WIDTH,
        GameConstants::SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // Khoi tao renderer
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // Thiet lap ti le logic de hien thi
    SDL_SetRenderLogicalPresentation(renderer,
        GameConstants::LOGICAL_WIDTH,
        GameConstants::LOGICAL_HEIGHT,
        SDL_LOGICAL_PRESENTATION_STRETCH);

    //tao Item
    if (!loadItemTextures()) return false;


    map = new Map(renderer);
    if (!map->loadMap("assets/tileset/Map_game.tmj")) {
        std::cerr << "Failed to load map." << std::endl;
    }

    SDL_FPoint spawn = map->GetPlayerSpawn();   // Lay toa do spawn trong Map

    // Tao nhan vat 
    player = new Player(renderer,glm::vec2(spawn.x, spawn.y));    // Sua o day. Them bien toa do de ve nhan vat spawn o vi tri bat dau

    spawnInitialItems();
    
    // Them o day. Nhac nen game
    audio = new Audio();
    audio->playBGM("assets/audio/breath.mp3", true, 0.03f);
    
    // audio.setBGMVolume(0.4f);   // nhạc còn ~40% . Dung xoa dong comment này


    return true;
}

// Vong lap game
void Game::run() {
    if (!init()) return;

    uint64_t prevTime = SDL_GetTicks();
    // Vong lap game toi khi nguoi choi thoat
    while (isGameRunning) {
        uint64_t nowTime = SDL_GetTicks();
        float deltaTime = (nowTime - prevTime) / 1000.0f;
        prevTime = nowTime;

        if (deltaTime > (1.0f / 60.0f)) deltaTime = (1.0f / 60.0f);

        // Xu ly su kien (nhan phim, thoat, v.v)
        handleEvents();
        // Cap nhat logic game
        update(deltaTime);
        // Ve khung hinh ra man hinh
        render();
    }
}
// Xu ly su kien nguoi dung
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isGameRunning = false;
        }
    }
    SDL_PumpEvents();
}
// Cap nhat logic gameplay
void Game::update(float deltaTime) {
    SDL_PumpEvents();

    // Cap nhat chuyen dong, hoat anh nhan vat
    if (player && map)
        player->Update(deltaTime, *map);

    //cap nhat tat ca item
    for (const auto& item : items) {
        if (!item->IsCollected()) {
            item->Update(deltaTime);
        }
    }

    //kiem tra nhat item
    checkItemCollisions();

    camera.update(player->GetPosition(), deltaTime);

    audio->update(deltaTime);

    audio->fadeOutBGM(0.4f);

}

// Ve len man hinh
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 200, 255);
    SDL_RenderClear(renderer);

    glm::vec2 offset = camera.getOffset();
    if (player) player->Render(renderer, offset);
    if (map) map->drawMap(offset);
    
    // Dong nay dong de ve ra khung vien do cua cac o dat co hop va cham. Khi khong can nua thi comment dong nay, khong can xoa
    map->drawCollisionDebug(offset);   


    for (const auto& item : items) {
        item->Render(renderer, offset);
    }




    SDL_RenderPresent(renderer);
}

// Giai phong tai nguyen
void Game::cleanup() {

    delete player;
    delete map;
    delete audio;
    items.clear();

    //delete playerHUD;

    SDL_DestroyTexture(coinTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


bool Game::loadItemTextures() {
    coinTex = IMG_LoadTexture(renderer, "assets/items/coin2_20x20.png");
    if (!coinTex) {
        std::cerr << "ERROR: Khong the load Coin texture: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_SetTextureScaleMode(coinTex, SDL_SCALEMODE_NEAREST);
    return true;
}

// Thêm hàm tạo Item
void Game::spawnInitialItems() {
    items.push_back(std::make_unique<Item>(glm::vec2(280.0f, 260.0f), coinTex, ItemType::COIN));
    items.push_back(std::make_unique<Item>(glm::vec2(520.0f, 160.0f), coinTex, ItemType::COIN));
    items.push_back(std::make_unique<Item>(glm::vec2(70.0f, 130.0f), coinTex, ItemType::COIN));
}

void Game::checkItemCollisions() {
    if (!player) return;

    SDL_FRect playerBox = player->GetBoundingBox();

    //loại bỏ các item đã nhặt
    items.erase(
        std::remove_if(items.begin(), items.end(),
            [&](const std::unique_ptr<Item>& item) {
                if (!item || item->IsCollected()) return true;

                SDL_FRect itemBox = item->GetCollider();
                bool collided = SDL_HasRectIntersectionFloat(&playerBox, &itemBox);

                if (collided) {
                    item->Collect();

                    if (item->GetType() == ItemType::COIN) {
                        // CẬP NHẬT ĐIỂM SỐ TẠI ĐÂY
                        AddScore(10); // Ví dụ: mỗi xu được 10 điểm
                        std::cout << "Player da nhặt Coin! Diem: " << score << "\n";
                    }
                    return true;
                }
                return false;
            }),
        items.end());
}