#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Game.h"
#include "../Environment/Map.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/TrailSystem.h"
#include "Camera.h"
#include "../Entities/Player.h"
#include "../Core/Audio.h"
#include "../Core/HUD.h"
#include <iostream>
#include "../Environment/Item.h"
#include <algorithm>
#include <string>


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
    coinTex(nullptr),
    audio(nullptr),
    playerHUD(nullptr)
{
}

// Giai phong tai nguyen
Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!TTF_Init()) {
        std::cerr << "TTF Init Error: " << SDL_GetError() << std::endl;
        return false;
    }


    window = SDL_CreateWindow("Game Monster",
        GameConstants::SCREEN_WIDTH,
        GameConstants::SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }
    TrailSystem::Init(renderer);

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

    SDL_FPoint spawn = map->GetPlayerSpawn();
    player = new Player(renderer, glm::vec2(spawn.x, spawn.y));

    // Tao nhan vat 
    spawnInitialItems();

    // ===== THÊM MỚI: SPAWN ENEMIES =====
    initEnemies();

    playerHUD = new HUD(renderer);
    playerHUD->LoadResources();

    audio = new Audio();
    audio->playBGM("assets/audio/breath.mp3", true, 0.4f);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

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

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
            isGameRunning = false;
    }
    SDL_PumpEvents();
}

// Cap nhat logic gameplay
void Game::update(float deltaTime) {
    // Cap nhat chuyen dong, hoat anh nhan vat
    if (player && map)
        player->Update(deltaTime, *map);

    for (const auto& item : items)
        if (!item->IsCollected()) item->Update(deltaTime);

    //kiem tra item da nhat chua
    checkItemCollisions();

    // ===== THÊM MỚI: CẬP NHẬT ENEMIES =====
    updateEnemies(deltaTime);

    // ===== THÊM MỚI: KIỂM TRA VA CHẠM ENEMIES =====
    checkEnemyCollisions();

    // ===== THÊM MỚI: CẬP NHẬT EFFECTS =====
    effectManager.Update(deltaTime);

    camera.update(player->GetPosition(), deltaTime);

    if (playerHUD) playerHUD->Update(deltaTime);
    if (audio) audio->update(deltaTime);
}

// Ve len man hinh
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 200, 255);
    SDL_RenderClear(renderer);

    // ===== THÊM MỚI: APPLY CAMERA SHAKE =====
    glm::vec2 shake = effectManager.GetCameraShake();
    glm::vec2 offset = camera.getOffset() + shake;

    if (map) map->drawMap(offset);

    if (player) player->Render(renderer, offset);
    MovementSystem::RenderDashTrails(renderer);

    // ===== THÊM MỚI: RENDER ENEMIES =====
    for (auto& enemy : enemies) {
        enemy->Render(renderer, offset);
    }

    // Dong nay dong de ve ra khung vien do cua cac o dat co hop va cham. Khi khong can nua thi comment dong nay, khong can xoa
    map->drawCollisionDebug(offset);

    for (const auto& item : items)
        item->Render(renderer, offset);

    // ===== THÊM MỚI: RENDER EFFECTS =====
    effectManager.Render(renderer, offset);

    if (playerHUD) playerHUD->Render(offset);

    SDL_RenderPresent(renderer);
}

// Giai phong tai nguyen
void Game::cleanup() {
    delete player;
    delete map;
    delete audio;
    delete playerHUD;
    items.clear();

    // ===== THÊM MỚI: XÓA ENEMIES =====
    enemies.clear();
    TrailSystem::Cleanup();

    SDL_DestroyTexture(coinTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
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

void Game::spawnInitialItems() {
    items.push_back(std::make_unique<Item>(glm::vec2(280.0f, 260.0f), coinTex, ItemType::COIN));
    items.push_back(std::make_unique<Item>(glm::vec2(520.0f, 160.0f), coinTex, ItemType::COIN));
    items.push_back(std::make_unique<Item>(glm::vec2(70.0f, 130.0f), coinTex, ItemType::COIN));
}

void Game::checkItemCollisions() {
    if (!player || !playerHUD) return;
    SDL_FRect playerBox = player->GetBoundingBox();

    items.erase(
        std::remove_if(items.begin(), items.end(),
            [&](const std::unique_ptr<Item>& item) {
                if (!item || item->IsCollected()) return true;

                SDL_FRect itemBox = item->GetCollider();
                bool collided = SDL_HasRectIntersectionFloat(&playerBox, &itemBox);

                if (collided && item->GetType() == ItemType::COIN) {
                    item->Collect();
                    int coinValue = 10;
                    glm::vec2 itemPos(itemBox.x, itemBox.y);
                    playerHUD->AddScore(coinValue);
                    playerHUD->AddScorePopup(itemPos, coinValue);
                    std::cout << "Player da nhat Coin! Diem: " << playerHUD->GetScore() << "\n";
                    return true;
                }
                return false;
            }),
        items.end());
}

// ===== THÊM MỚI: KHỞI TẠO ENEMIES =====
void Game::initEnemies() {
    std::cout << "===== BAT DAU SPAWN ENEMIES =====\n";

    // ===== VÙNG 1: SPAWN 3 ORC MINIONS (0-300) =====

    // ORC 1
    enemies.push_back(std::make_unique<Minions>(
        renderer,
        glm::vec2(150.0f, 200.0f),
        MinionType::ORC_BERSERK
    ));
    enemies.back()->SetPatrolPoints(glm::vec2(120.0f, 200.0f), glm::vec2(180.0f, 200.0f));

    // ORC 2
    enemies.push_back(std::make_unique<Minions>(
        renderer,
        glm::vec2(250.0f, 200.0f),
        MinionType::ORC_SHAMAN
    ));
    enemies.back()->SetPatrolPoints(glm::vec2(220.0f, 200.0f), glm::vec2(280.0f, 200.0f));

    // ORC 3
    enemies.push_back(std::make_unique<Minions>(
        renderer,
        glm::vec2(350.0f, 200.0f),
        MinionType::ORC_WARRIOR
    ));
    enemies.back()->SetPatrolPoints(glm::vec2(320.0f, 200.0f), glm::vec2(380.0f, 200.0f));

    // ===== VÙNG 2: SPAWN 3 TROLL ELITES (500-800) =====

    // TROLL 1
    enemies.push_back(std::make_unique<Elites>(
        renderer,
        glm::vec2(550.0f, 200.0f),
        TrollType::TROLL_1
    ));
    enemies.back()->SetPatrolPoints(glm::vec2(520.0f, 200.0f), glm::vec2(580.0f, 200.0f));

    // TROLL 2
    enemies.push_back(std::make_unique<Elites>(
        renderer,
        glm::vec2(700.0f, 200.0f),
        TrollType::TROLL_2
    ));
    enemies.back()->SetPatrolPoints(glm::vec2(670.0f, 200.0f), glm::vec2(730.0f, 200.0f));

    // TROLL 3
    enemies.push_back(std::make_unique<Elites>(
        renderer,
        glm::vec2(850.0f, 200.0f),
        TrollType::TROLL_3
    ));
    enemies.back()->SetPatrolPoints(glm::vec2(820.0f, 200.0f), glm::vec2(880.0f, 200.0f));

    // ===== VÙNG 3: SPAWN BOSS (1200+) =====
    enemies.push_back(std::make_unique<Boss>(
        renderer,
        glm::vec2(1200.0f, 200.0f)
    ));

    std::cout << "===== DA SPAWN " << enemies.size() << " ENEMIES =====\n";
}

// ===== THÊM MỚI: CẬP NHẬT ENEMIES =====
void Game::updateEnemies(float deltaTime) {
    if (!player) return;

    // Set vị trí Player cho tất cả enemies
    glm::vec2 playerPos = player->GetPosition();

    for (auto& enemy : enemies) {
        enemy->SetTargetPosition(playerPos);
        enemy->Update(deltaTime, *map);
    }

    // Xóa enemies đã chết sau 2 giây
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<Enemy>& e) {
                return !e->IsAlive() && e->GetDeathTimer() > 2.0f;
            }),
        enemies.end()
    );
}

// ===== THÊM MỚI: KIỂM TRA VA CHẠM =====
void Game::checkEnemyCollisions() {
    if (!player) return;

    SDL_FRect playerBox = player->GetBoundingBox();

    // ===== VA CHẠM 1: PROJECTILE CỦA PLAYER -> ENEMY =====
    for (auto& proj : player->GetProjectiles()) {
        if (!proj->IsActive()) continue;

        for (auto& enemy : enemies) {
            if (!enemy->IsAlive()) continue;

            SDL_FRect projBox = proj->GetBoundingBox();
            SDL_FRect enemyBox = enemy->GetBoundingBox();

            if (SDL_HasRectIntersectionFloat(&projBox, &enemyBox)) {
                // Enemy nhận damage
                enemy->TakeDamage(proj->GetDamage());

                // Projectile biến mất
                proj->Deactivate();

                // Tạo hiệu ứng nổ
                effectManager.CreateExplosion(
                    proj->GetPosition(),
                    15.0f,
                    SDL_Color{ 255, 150, 0, 255 }
                );

                std::cout << "Projectile trung Enemy!\n";
                break;
            }
        }
    }

    // ===== VA CHẠM 2: ENEMY TẤN CÔNG -> PLAYER =====
    for (auto& enemy : enemies) {
        if (!enemy->IsAlive()) continue;

        // Chỉ gây damage khi đang tấn công
        if (enemy->GetEnemyState() == EnemyState::STATE_ATTACK) {
            SDL_FRect enemyBox = enemy->GetBoundingBox();

            if (SDL_HasRectIntersectionFloat(&playerBox, &enemyBox)) {
                int damage = enemy->PerformAttack();
                if (damage > 0) {
                    player->TakeDamage(damage);

                    // Flash effect khi Player bị đánh
                    effectManager.CreateFlash(
                        player->GetPosition(),
                        SDL_Color{ 255, 0, 0, 100 },
                        0.3f,
                        0.15f
                    );

                    std::cout << "Enemy tan cong Player! Damage: " << damage << "\n";
                }
            }
        }
    }
}