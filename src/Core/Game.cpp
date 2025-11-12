#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Game.h"
#include "../Environment/Map.h"
#include "../Systems/MovementSystem.h"
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
    healthPotionTex(nullptr),
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

    SDL_FPoint spawn = map->GetSpawn(0);
    player = new Player(renderer, glm::vec2(spawn.x, spawn.y));

    // ===== SPAWN ENEMIES =====
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

    // ===== KIỂM TRA VA CHẠM ENEMIES =====
    checkEnemyCollisions();

    // ===== CẬP NHẬT EFFECTS =====
    effectManager.Update(deltaTime);

    camera.update(player->GetPosition(), deltaTime);

    if (playerHUD) playerHUD->Update(deltaTime);
    if (audio) audio->update(deltaTime);
}

// Ve len man hinh
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 200, 255);
    SDL_RenderClear(renderer);

    // ===== APPLY CAMERA SHAKE =====
    glm::vec2 shake = effectManager.GetCameraShake();
    glm::vec2 offset = camera.getOffset() + shake;

    if (map) map->drawMap(offset);

    if (player) player->Render(renderer, offset);

    // =====RENDER ENEMIES =====
    for (auto& enemy : enemies) {
        enemy->Render(renderer, offset);
    }

    // DEBUG: VẼ HITBOX CHO ENEMY (Comment khi release)
    #ifdef DEBUG_DRAW_HITBOX  // Thêm dòng này
    for (auto& enemy : enemies) {
        if (enemy->IsAlive()) {
            SDL_FRect box = enemy->GetBoundingBox();
            SDL_FRect screenBox = {
                box.x - offset.x,
                box.y - offset.y,
                box.w,
                box.h
            };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
            SDL_RenderRect(renderer, &screenBox);
        }
    }
    #endif  // Thêm dòng này

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

    healthPotionTex = IMG_LoadTexture(renderer, "assets/items/potion4.png");
    if (!coinTex) {
        std::cerr << "ERROR: Khong the load Coin texture: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_SetTextureScaleMode(healthPotionTex, SDL_SCALEMODE_NEAREST);
    return true;
}

void Game::spawnCoinAtPosition(glm::vec2 pos, int amount) {
    // Spawn nhiều coins rải rác xung quanh vị trí
    for (int i = 0; i < amount; i++) {
        float offsetX = (rand() % 30) - 15.0f; // Random -15 đến +15
        float offsetY = (rand() % 20) - 10.0f; // Random -10 đến +10

        glm::vec2 coinPos = pos + glm::vec2(offsetX, offsetY + 15.0f);

        auto coin = std::make_unique<Item>(coinPos, coinTex, ItemType::COIN);

        // Tuỳ chỉnh float effect
        coin->floatAmplitude = 6.0f;
        coin->floatSpeed = 3.0f;

        items.push_back(std::move(coin));
    }
    std::cout << "Spawn " << amount << " coins tai (" << pos.x << ", " << pos.y << ")\n";
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

                if (collided) {
                    item->Collect();
                    glm::vec2 itemPos(itemBox.x, itemBox.y);
                    if (item->GetType() == ItemType::COIN) {
                        int coinValue = 10;
                        playerHUD->AddScore(coinValue);
                        playerHUD->AddScorePopup(itemPos, coinValue);
                        std::cout << "Player da nhat Coin! Diem: " << playerHUD->GetScore() << "\n";
                        return true;
                    }
                    else if (item->GetType() == ItemType::HEALTH_POTION) {
                        int healAmount = GameConstants::HEALTH_POTION_HEAL_AMOUNT;
                        player->Heal(healAmount);

                        // Hiệu ứng flash xanh lá khi hồi máu
                        effectManager.CreateFlash(
                            player->GetPosition(),
                            SDL_Color{ 0, 255, 100, 150 },
                            0.5f,   // duration
                            0.2f    // pulseSpeed
                        );

                        // Hiển thị popup "+HP"
                        //playerHUD->AddScorePopup(itemPos, healAmount);

                        std::cout << "Player da nhat Health Potion! HP: "
                            << player->GetHealth() << "/" << player->GetMaxHealth() << "\n";
                        return true;
                    }
                }
                
                
                return false;
            }),
        items.end());
}

void Game::spawnHealthPotionAtPosition(glm::vec2 pos) {
    auto potion = std::make_unique<Item>(pos, healthPotionTex, ItemType::HEALTH_POTION);

    potion->floatAmplitude = 8.0f;
    potion->floatSpeed = 2.0f;

    items.push_back(std::move(potion));
    std::cout << "Spawn Health Potion tai (" << pos.x << ", " << pos.y << ")\n";
}

// ===== KHỞI TẠO ENEMIES =====
void Game::initEnemies() {
    std::cout << "===== BAT DAU SPAWN ENEMIES =====\n";

    // ===== VÙNG 1: SPAWN 3 ORC MINIONS (X: 200-400) =====

    // ORC BERSERK
    enemies.push_back(std::make_unique<Minions>(
        renderer,
        glm::vec2(200.0f, GameConstants::FLOOR_Y - 48.0f),  // Spawn trên mặt đất
        MinionType::ORC_BERSERK
    ));
    enemies.back()->SetPatrolPoints(
        glm::vec2(150.0f, GameConstants::FLOOR_Y - 48.0f),
        glm::vec2(250.0f, GameConstants::FLOOR_Y - 48.0f)
    );
    enemies.back()->SetCoinDropAmount(1);
    enemies.back()->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
        spawnCoinAtPosition(pos, amount);
        spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
        });

    // ORC SHAMAN
    enemies.push_back(std::make_unique<Minions>(
        renderer,
        glm::vec2(300.0f, GameConstants::FLOOR_Y - 48.0f),
        MinionType::ORC_SHAMAN
    ));
    enemies.back()->SetPatrolPoints(
        glm::vec2(270.0f, GameConstants::FLOOR_Y - 48.0f),
        glm::vec2(330.0f, GameConstants::FLOOR_Y - 48.0f)
    );
    enemies.back()->SetCoinDropAmount(1);
    enemies.back()->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
        spawnCoinAtPosition(pos, amount);
        spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
        });

    // ORC WARRIOR
    enemies.push_back(std::make_unique<Minions>(
        renderer,
        glm::vec2(400.0f, GameConstants::FLOOR_Y - 48.0f),
        MinionType::ORC_WARRIOR
    ));
    enemies.back()->SetPatrolPoints(
        glm::vec2(370.0f, GameConstants::FLOOR_Y - 48.0f),
        glm::vec2(430.0f, GameConstants::FLOOR_Y - 48.0f)
    );
    enemies.back()->SetCoinDropAmount(1);
    enemies.back()->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
        spawnCoinAtPosition(pos, amount);
        spawnHealthPotionAtPosition(pos + glm::vec2(0,0));
        });

    // ===== VÙNG 2: SPAWN 3 TROLL ELITES (X: 600-900) =====

    // TROLL 1
    enemies.push_back(std::make_unique<Elites>(
        renderer,
        glm::vec2(600.0f, GameConstants::FLOOR_Y - 64.0f),  // Lớn hơn nên offset lớn hơn
        TrollType::TROLL_1
    ));
    enemies.back()->SetPatrolPoints(
        glm::vec2(550.0f, GameConstants::FLOOR_Y - 64.0f),
        glm::vec2(650.0f, GameConstants::FLOOR_Y - 64.0f)
    );
    enemies.back()->SetCoinDropAmount(3);
    enemies.back()->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
        spawnCoinAtPosition(pos, amount);
        spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
        });

    // TROLL 2
    enemies.push_back(std::make_unique<Elites>(
        renderer,
        glm::vec2(750.0f, GameConstants::FLOOR_Y - 64.0f),
        TrollType::TROLL_2
    ));
    enemies.back()->SetPatrolPoints(
        glm::vec2(700.0f, GameConstants::FLOOR_Y - 64.0f),
        glm::vec2(800.0f, GameConstants::FLOOR_Y - 64.0f)
    );
    enemies.back()->SetCoinDropAmount(3);
    enemies.back()->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
        spawnCoinAtPosition(pos, amount);
        spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
        });

    // TROLL 3
    enemies.push_back(std::make_unique<Elites>(
        renderer,
        glm::vec2(900.0f, GameConstants::FLOOR_Y - 64.0f),
        TrollType::TROLL_3
    ));
    enemies.back()->SetPatrolPoints(
        glm::vec2(850.0f, GameConstants::FLOOR_Y - 64.0f),
        glm::vec2(950.0f, GameConstants::FLOOR_Y - 64.0f)
    );
    enemies.back()->SetCoinDropAmount(3);
    enemies.back()->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
        spawnCoinAtPosition(pos, amount);
        spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
        });

    // ===== VÙNG 3: SPAWN BOSS (X: 1300+) =====
    enemies.push_back(std::make_unique<Boss>(
        renderer,
        glm::vec2(1300.0f, GameConstants::FLOOR_Y - 96.0f)  // Boss lớn nhất
    ));
    enemies.back()->SetCoinDropAmount(10);
    enemies.back()->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
        spawnCoinAtPosition(pos, amount);
        spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
        });

    std::cout << "===== DA SPAWN " << enemies.size() << " ENEMIES =====\n";
}

// ===== CẬP NHẬT ENEMIES =====
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

// ===== KIỂM TRA VA CHẠM =====
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
                if (damage > 0 && !player->IsInvulnerable()) {
                    player->TakeDamage(damage);

                    effectManager.CreateFlash(
                        player->GetPosition(),
                        SDL_Color{ 255, 0, 0, 100 },
                        0.3f,
                        0.15f
                    );
                }
            }
        }
    }
}