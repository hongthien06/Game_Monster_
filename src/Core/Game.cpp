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
#include "../Entities/Minion.h"


Game::Game()
    : window(nullptr),
    renderer(nullptr),
    isGameRunning(true),
    map(nullptr),
    pendingNextMap(false),
    player(nullptr),
    camera(
        GameConstants::LOGICAL_WIDTH,
        GameConstants::LOGICAL_HEIGHT,
        GameConstants::WORLD_WIDTH,
        GameConstants::WORLD_HEIGHT),
    coinTex(nullptr),
    healthPotionTex(nullptr),
    audio(nullptr),
    playerHUD(nullptr),
    currentGameState(GameState::MAIN_MENU), 
    targetState(GameState::MAIN_MENU), 
    menuBackgroundTex(nullptr),         
    gameBackgroundTex(nullptr),         
    mainFont(nullptr),
    mainMenu(nullptr)
    //gameOverMenu(nullptr),    
    //showGameOverMenu(false)
{
}


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

    mainFont = TTF_OpenFont("assets/fonts/StackSansHeadline-VariableFont_wght.ttf", 30);
    if (!mainFont) {
        std::cerr << "Failed to load mainFont: " << SDL_GetError() << std::endl;
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
    if (SDL_SetRenderVSync(renderer, 1) != 0) {
    SDL_Log("Failed to enable VSYNC: %s", SDL_GetError());
}
    //SDL_SetRenderVSync(renderer, 0); // tắt VSYNC
    
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
    if (!map->loadMap("assets/tileset/Map_1.tmj")) {   // Xong thi doi map ve Map_1.tmj
    }
    currentMapName = "assets/tileset/Map_1.tmj";  // Lưu tên map hiện tại


 // Tai vi tri spawn
    auto playerSpawns = map->GetSpawn(0);
    player = new Player(renderer, glm::vec2(playerSpawns[0].x, playerSpawns[0].y));
    player->SnapToGround(*map); 
    playerStartPos = player->GetPosition();

    playerHUD = new HUD(renderer);
    playerHUD->LoadResources();

    if (player) {
        playerHUD->SetPlayerReference(player);
        std::cout << "[Game] PlayerHUD reference set successfully!" << std::endl;
        std::cout << "[Game] Player potion count: " << player->GetHealthPotionCount()
            << "/" << player->GetMaxHealthPotions() << std::endl;
    }
    else {
        std::cerr << "[Game] ERROR: Player is NULL when setting HUD reference!" << std::endl;
    }


    for (auto& pos : map->GetSpawn(1)) {
    std::cout << "Minion1: " << pos.x << "," << pos.y << "\n";
}
    for (auto& pos : map->GetSpawn(2)) {
    std::cout << "Minion2: " << pos.x << "," << pos.y << "\n";
}
    for (auto& pos : map->GetSpawn(3)) {
    std::cout << "Minion3: " << pos.x << "," << pos.y << "\n";
}

    // ===== SPAWN ENEMIES =====
    initEnemies();

    // ===== SPAWN ITEMS =====

    initItems();

    playerHUD = new HUD(renderer);
    playerHUD->LoadResources();
    playerHUD->SetPlayerReference(player);

    audio = new Audio();
    audio->playBGM("assets/audio/breath.mp3", true, 0.4f);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    mainMenu = std::make_unique<MainMenu>(renderer, mainFont);
    gameWinMenu = std::make_unique<GameWinMenu>(renderer, mainFont);
    gameOverMenu = std::make_unique<GameOverMenu>(renderer, mainFont);
    tutorialMenu = std::make_unique<TutorialMenu>(renderer, mainFont);

    // THÊM: Khởi tạo ScreenTransition
    screenTransition = std::make_unique<ScreenTransition>(
        renderer,
        GameConstants::LOGICAL_WIDTH,
        GameConstants::LOGICAL_HEIGHT,
        TransitionType::FADE,
        0.5f
    );

    // ✅ THÊM: Transition riêng cho chuyển map (mặc định FADE, sẽ đổi động)
    mapTransition = std::make_unique<ScreenTransition>(
        renderer,
        GameConstants::LOGICAL_WIDTH,
        GameConstants::LOGICAL_HEIGHT,
        TransitionType::FADE,
        0.3f
    );
    // THÊM: Load background textures (optional)
   // THÊM: Load background textures
    menuBackgroundTex = IMG_LoadTexture(renderer, "assets/images/Layers/bg_mainmenu.png");
    if (!menuBackgroundTex) {
        std::cerr << "WARNING: Cannot load menu background: " << SDL_GetError() << std::endl;
        std::cerr << "Check file exists at: assets/images/Layers/bg_mainmenu.png" << std::endl;
    }

    gameBackgroundTex = IMG_LoadTexture(renderer, "assets/images/Layers//bg_mainmenu.png");
    if (!gameBackgroundTex) {
        std::cerr << "WARNING: Cannot load game background: " << SDL_GetError() << std::endl;
    }

    currentGameState = GameState::MAIN_MENU;

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
        if (currentGameState == GameState::MAIN_MENU) {
            mainMenu->HandleKeyboardInput();
        }
        else if (currentGameState == GameState::GAME_OVER) {
            gameOverMenu->HandleInput();
        }
        else if (currentGameState == GameState::TUTORIAL) {
            tutorialMenu->HandleInput();
        }
        else if (currentGameState == GameState::GAME_WIN) {
            gameWinMenu->HandleInput();
        }
    }
    SDL_PumpEvents();
}
// ===== SỬA HÀM update() - KIỂM TRA HẾT QUÁI =====
void Game::update(float deltaTime) {
    switch (currentGameState) {

    case GameState::MAIN_MENU:
    {
        mainMenu->Update(deltaTime);
        MainMenuChoice choice = mainMenu->GetChoice();

        if (choice == MainMenuChoice::START_GAME) {
            startTransition(GameState::PLAYING);
        }
        else if (choice == MainMenuChoice::QUIT) {
            isGameRunning = false;
        }
        else if (choice == MainMenuChoice::TUTORIAL) {
            mainMenu->ResetChoice();
            startTransition(GameState::TUTORIAL);
        }
        break;
    }

    case GameState::TRANSITIONING:
    {
        screenTransition->Update(deltaTime);

        if (!screenTransition->IsActive()) {
            currentGameState = targetState;

            if (targetState == GameState::PLAYING) {
                resetGame();
            }

            mainMenu->ResetChoice();
            mainMenu->SetTransitioning(false);
        }
        break;
    }// ✅ THÊM CASE MỚI

    case GameState::MAP_TRANSITIONING:
    {
        mapTransition->Update(deltaTime);

        // Khi transition đến giữa chặng (màn hình tối nhất)
        if (mapTransition->ShouldSwitchScreen() && !nextMapName.empty()) {
            ExecuteMapChange();  // Thực hiện đổi map
            nextMapName.clear();  // Đánh dấu đã xử lý
        }

        // Khi transition hoàn tất
        if (!mapTransition->IsActive()) {
            currentGameState = GameState::PLAYING;
            std::cout << "[Game] Map transition complete!\n";
        }
        break;
    }

    case GameState::PLAYING:
    {
        if (player && map)
            player->Update(deltaTime, *map);

        for (const auto& item : items)
            if (!item->IsCollected())
                item->Update(deltaTime);

        checkItemCollisions();

        checkProjectileBounds();

        // ✅ UPDATE ENEMIES TRƯỚC
        updateEnemies(deltaTime);

        // ✅ DEBUG: Kiểm tra số lượng enemies
        static int frameCount = 0;
        if (++frameCount % 60 == 0) { // Log mỗi 60 frames (~1 giây)
            int aliveCount = 0;
            int deadCount = 0;
            for (const auto& enemy : enemies) {
                if (enemy->IsAlive()) {
                    aliveCount++;
                    // ✅ THÊM: In vị trí của enemy còn sống
                    glm::vec2 pos = enemy->GetPosition();
                    std::cout << "[Game] Alive enemy at: (" << pos.x << ", " << pos.y << ")\n";
                }
                else {
                    deadCount++;
                }
            }
            std::cout << "[Game] Enemies alive: " << aliveCount
                << " | dead: " << deadCount
                << " | total: " << enemies.size() << "\n";

            // ✅ THÊM: In vị trí player để so sánh
            if (player) {
                glm::vec2 playerPos = player->GetPosition();
                std::cout << "[Game] Player at: (" << playerPos.x << ", " << playerPos.y << ")\n";
            }
        }

        // ✅ SAU ĐÓ mới check collisions
        checkEnemyCollisions();

        effectManager.Update(deltaTime);
        // ===== ✅ SNAP CAMERA NGAY KHI RESPAWN =====
        if (player && player->NeedsCameraSnap()) {
            // Gọi update nhiều lần để camera "lerp" nhanh về vị trí
            for (int i = 0; i < 50; i++) {
                camera.update(player->GetPosition(), 0.1f);
            }
            player->ClearCameraSnapFlag();
            std::cout << "[Game] Camera snapped to player respawn position!\n";
        }

        if (playerHUD)
            playerHUD->Update(deltaTime);
        if (audio)
            audio->update(deltaTime);

        // ✅ KIỂM TRA CHẾT
        if (player->IsGameOver()) {  // Thay vì !player->IsAlive()
            currentGameState = GameState::GAME_OVER;
            gameOverMenu->Reset();
        }

        // ✅ KIỂM TRA HẾT QUÁI (sau khi đã update & xóa enemy chết)
        if (enemies.empty() && !pendingNextMap) {
            std::cout << "[Game] ===== ALL ENEMIES DEFEATED =====\n";
            std::cout << "[Game] Current map: " << currentMapName << "\n";

            // ✅ XÁC ĐỊNH MAP TIẾP THEO VÀ HIỆU ỨNG
            if (currentMapName == "assets/tileset/Map_1.tmj") {
                StartMapTransition("assets/tileset/Map_2.tmj", TransitionType::FADE);
            }
            else if (currentMapName == "assets/tileset/Map_2.tmj") {
                StartMapTransition("assets/tileset/Map_3.tmj", TransitionType::ZOOM_FLASH);
            }
            else {
                std::cout << "[Game] No more maps! Victory!\n";
                currentGameState = GameState::GAME_WIN;
                if (gameWinMenu) gameWinMenu->Reset();
            }

            pendingNextMap = true;
        }

        break;
    }

    case GameState::GAME_WIN:
    {
        if (gameWinMenu) {
            gameWinMenu->Update(deltaTime);
            gameWinMenu->HandleInput();
            GameWinChoice choice = gameWinMenu->GetChoice();

            if (choice == GameWinChoice::REPLAY) {
                startTransition(GameState::PLAYING);
            }
            else if (choice == GameWinChoice::QUIT) {
                isGameRunning = false;
            }
        }
        break;
    }

    case GameState::GAME_OVER:
    {
        gameOverMenu->Update(deltaTime);
        gameOverMenu->HandleInput();

        GameOverChoice choice = gameOverMenu->GetChoice();

        if (choice == GameOverChoice::REPLAY) {
            startTransition(GameState::PLAYING);
        }
        else if (choice == GameOverChoice::QUIT) {
            isGameRunning = false;
        }
        break;
    }

    case GameState::TUTORIAL:
    {
        if (tutorialMenu) {
            tutorialMenu->Update(deltaTime);
            TutorialChoice choice = tutorialMenu->GetChoice();

            if (choice == TutorialChoice::BACK) {
                tutorialMenu->ResetChoice();
                startTransition(GameState::MAIN_MENU);
            }
        }
        break;
    }
    }
    
    //// ✅ XỬ LÝ CHUYỂN MAP (cuối hàm update)
    //if (pendingNextMap) {
    //    std::cout << "[Game] ===== EXECUTING MAP TRANSITION =====\n";
    //    LoadNextMap();
    //    pendingNextMap = false;
    //    std::cout << "[Game] pendingNextMap reset to false\n";
    //}
}

// Ve len man hinh
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 101, 67, 33, 255);
    SDL_RenderClear(renderer);

    // Xác định nên render screen nào
    bool renderGameplay = (currentGameState == GameState::PLAYING ||
        currentGameState == GameState::MAP_TRANSITIONING);

    if (currentGameState == GameState::TRANSITIONING) {
        if (screenTransition->ShouldSwitchScreen()) {
            renderGameplay = (targetState == GameState::PLAYING);
        }
        else {
            renderGameplay = false;
        }
    }


    // RENDER BACKGROUND
    if (renderGameplay) {
        // Render game background
        if (gameBackgroundTex) {
            SDL_RenderTexture(renderer, gameBackgroundTex, nullptr, nullptr);
        }
    }
    else {
        // Render menu background
        if (menuBackgroundTex) {
            SDL_RenderTexture(renderer, menuBackgroundTex, nullptr, nullptr);
        }
    }

    // RENDER GAME ELEMENTS
    if (renderGameplay) {
        glm::vec2 shake = effectManager.GetCameraShake();
        glm::vec2 offset = camera.getOffset() + shake;

        if (map) map->drawMap(offset);
        if (player) player->Render(renderer, offset);

        for (auto& enemy : enemies) {
            enemy->Render(renderer, offset);
        }
        for (const auto& item : items)
            item->Render(renderer, offset);

        effectManager.Render(renderer, offset);
        if (playerHUD) playerHUD->Render(offset);
    }

    // RENDER UI OVERLAYS
    if (currentGameState == GameState::GAME_OVER) {
        gameOverMenu->Render();
    }
    else if (currentGameState == GameState::GAME_WIN) {
        gameWinMenu->Render();
    }
    else if (currentGameState == GameState::MAIN_MENU) {
        mainMenu->Render();
    }
    else if (currentGameState == GameState::TUTORIAL) {
        if (tutorialMenu) {
            tutorialMenu->Render();
        }
    }

    // RENDER TRANSITION EFFECT (vẽ cuối cùng)
    if (currentGameState == GameState::TRANSITIONING) {
        screenTransition->Render();
    }
    // ✅ THÊM: Render map transition
    else if (currentGameState == GameState::MAP_TRANSITIONING) {
        mapTransition->Render();
    }
    // ===== THÊM MỚI: RENDER DEATH TRANSITION (VẼ CUỐI CÙNG) =====
    if (player && player->IsInDeathTransition()) {
        player->RenderDeathTransition(renderer);
    }

     pendingNextMap = false;

    SDL_RenderPresent(renderer);
}
// 4️⃣ THÊM HÀM MỚI: StartMapTransition()
void Game::StartMapTransition(const std::string& nextMap, TransitionType type) {
    std::cout << "[Game] Starting map transition to: " << nextMap
        << " with effect: " << (int)type << "\n";

    nextMapName = nextMap;
    currentGameState = GameState::MAP_TRANSITIONING;

    float duration;
    if (type == TransitionType::ZOOM_FLASH) {
        duration = 0.25f;  // ZOOM_FLASH nhanh hơn
    }
    else if (type == TransitionType::FADE) {
        duration = 0.15f;  // FADE cực nhanh
    }
    else {
        duration = 0.2f;   // Các loại khác
    }

    mapTransition = std::make_unique<ScreenTransition>(
        renderer,
        GameConstants::LOGICAL_WIDTH,
        GameConstants::LOGICAL_HEIGHT,
        type,
        duration  // ✅ Dùng duration động
    );

    mapTransition->Start();
}

// 5️⃣ THÊM HÀM MỚI: ExecuteMapChange()
void Game::ExecuteMapChange() {
    std::cout << "[Game] ===== EXECUTING MAP CHANGE =====\n";
    std::cout << "[Game] Loading: " << nextMapName << "\n";

    currentMapName = nextMapName;

    // Clear dữ liệu cũ
    enemies.clear();
    std::cout << "[Game] Cleared old data.\n";

    // Xóa map cũ
    if (map) {
        delete map;
        map = nullptr;
    }

    map = new Map(renderer);
    if (!map) {
        std::cerr << "[Game] ERROR: Failed to create new map!\n";
        isGameRunning = false;
        return;
    }

    // Load map mới
    if (!map->loadMap(currentMapName)) {
        std::cerr << "[Game] ERROR: Failed to load map: " << currentMapName << std::endl;
        isGameRunning = false;
        return;
    }

    std::cout << "[Game] Map loaded successfully: " << currentMapName << std::endl;

    // Spawn mới
    try {
        initEnemies();
        initItems();
        std::cout << "[Game] Spawned " << enemies.size() << " enemies and "
            << items.size() << " items.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[Game] ERROR during spawn: " << e.what() << std::endl;
        isGameRunning = false;
        return;
    }

    // Cập nhật lại vị trí player
    auto spawn = map->GetSpawn(0);
    if (!spawn.empty()) {
        player->SetPosition(glm::vec2(spawn[0].x, spawn[0].y));
        player->SnapToGround(*map);
        std::cout << "[Game] Player spawned at (" << spawn[0].x << ", " << spawn[0].y << ")\n";
    }
    else {
        std::cerr << "[Game] WARNING: No player spawn point found!\n";
    }

    // Reset HUD
    if (playerHUD) {
        playerHUD->SetPlayerReference(player);
    }

    // Đổi nhạc (tuỳ chọn)
    if (audio) {
        audio->stopBGM();
        audio->playBGM("assets/audio/breath.mp3", true, 0.4f);
    }

    std::cout << "[Game] ===== MAP CHANGE COMPLETE =====\n";
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

    TTF_CloseFont(mainFont);

    SDL_DestroyTexture(coinTex);
    SDL_DestroyTexture(menuBackgroundTex);      // THÊM
    SDL_DestroyTexture(gameBackgroundTex);      // THÊM
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

// ===== KHỞI TẠO CÁC ITEM TRÊN BẢN ĐỒ =====
void Game::initItems() {
    std::cout << "===== BAT DAU SPAWN ITEMS =====\n";
    
    // Spawn các Health Potion từ Heal_Spawn points
    for (auto& pos : map->GetSpawn(8)) {
        auto potion = std::make_unique<Item>(glm::vec2(pos.x, pos.y), healthPotionTex, ItemType::HEALTH_POTION);
        potion->floatAmplitude = 8.0f;
        potion->floatSpeed = 2.0f;
        items.push_back(std::move(potion));
        std::cout << "Spawn Health Potion tai (" << pos.x << ", " << pos.y << ")\n";
    }
    
    // Spawn các Coin từ Coin_Spawn points
    for (auto& pos : map->GetSpawn(9)) {
        auto coin = std::make_unique<Item>(glm::vec2(pos.x, pos.y), coinTex, ItemType::COIN);
        coin->floatAmplitude = 6.0f;
        coin->floatSpeed = 3.0f;
        items.push_back(std::move(coin));
        std::cout << "Spawn Coin tai (" << pos.x << ", " << pos.y << ")\n";
    }
    
    std::cout << "===== DA SPAWN " << items.size() << " ITEMS =====\n";
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
                        // ===== THAY ĐỔI: THÊM VÀO TÚI THAY VÌ HỒI NGAY =====
                        player->AddHealthPotion();

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
// ===== SỬA TRONG HÀM initEnemies() CỦA Game.cpp =====

void Game::initEnemies() {
    std::cout << "===== BAT DAU SPAWN ENEMIES =====\n";

    // ===== ORC BERSERK =====
    for (auto& pos : map->GetSpawn(1)) {
        auto enemy = std::make_unique<Minions>(
            renderer,
            glm::vec2(pos.x, pos.y),
            MinionType::ORC_BERSERK
        );

        enemy->SetPatrolPoints(
            glm::vec2(pos.x - 100.0f, pos.y - 48.0f),
            glm::vec2(pos.x + 100.0f, pos.y - 48.0f)
        );

        enemy->SetAggroRange(100.0f);
        enemy->SetAttackRange(20.0f);
        enemy->SetCoinDropAmount(1);
        enemy->SetAudioSystem(audio);

        // ✅ SET CALLBACK TRƯỚC KHI MOVE VÀO VECTOR
        enemy->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
            spawnCoinAtPosition(pos, amount);
            spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
            });

        enemies.push_back(std::move(enemy));
    }

    // ===== ORC SHAMAN =====
    for (auto& pos : map->GetSpawn(2)) {
        auto enemy = std::make_unique<Minions>(
            renderer,
            glm::vec2(pos.x, pos.y),
            MinionType::ORC_SHAMAN
        );

        enemy->SetPatrolPoints(
            glm::vec2(pos.x - 150.0f, pos.y - 48.0f),
            glm::vec2(pos.x + 150.0f, pos.y - 48.0f)
        );

        enemy->SetAggroRange(100.0f);
        enemy->SetAttackRange(20.0f);
        enemy->SetCoinDropAmount(1);
        enemy->SetAudioSystem(audio);

        enemy->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
            spawnCoinAtPosition(pos, amount);
            spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
            });

        enemies.push_back(std::move(enemy));
    }

    // ===== ORC WARRIOR =====
    for (auto& pos : map->GetSpawn(3)) {
        auto enemy = std::make_unique<Minions>(
            renderer,
            glm::vec2(pos.x, pos.y),
            MinionType::ORC_WARRIOR
        );

        enemy->SetPatrolPoints(
            glm::vec2(pos.x - 80.0f, pos.y - 48.0f),
            glm::vec2(pos.x + 80.0f, pos.y - 48.0f)
        );

        enemy->SetAggroRange(100.0f);
        enemy->SetAttackRange(20.0f);
        enemy->SetCoinDropAmount(1);
        enemy->SetAudioSystem(audio);

        enemy->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
            spawnCoinAtPosition(pos, amount);
            spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
            });

        enemies.push_back(std::move(enemy));
    }

    // ===== TROLL 1 =====
    for (auto& pos : map->GetSpawn(4)) {
        auto enemy = std::make_unique<Elites>(
            renderer,
            glm::vec2(pos.x, pos.y),
            TrollType::TROLL_1
        );

        enemy->SetPatrolPoints(
            glm::vec2(pos.x - 120.0f, pos.y - 48.0f),
            glm::vec2(pos.x + 120.0f, pos.y - 48.0f)
        );

        enemy->SetAggroRange(100.0f);
        enemy->SetAttackRange(20.0f);
        enemy->SetCoinDropAmount(3);
        enemy->SetAudioSystem(audio);

        enemy->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
            spawnCoinAtPosition(pos, amount);
            spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
            });

        enemies.push_back(std::move(enemy));
    }

    // ===== TROLL 2 =====
    for (auto& pos : map->GetSpawn(5)) {
        auto enemy = std::make_unique<Elites>(
            renderer,
            glm::vec2(pos.x, pos.y),
            TrollType::TROLL_2
        );

        enemy->SetPatrolPoints(
            glm::vec2(pos.x - 100.0f, pos.y - 48.0f),
            glm::vec2(pos.x + 100.0f, pos.y - 48.0f)
        );

        enemy->SetAggroRange(150.0f);
        enemy->SetAttackRange(20.0f);
        enemy->SetCoinDropAmount(3);
        enemy->SetAudioSystem(audio);

        enemy->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
            spawnCoinAtPosition(pos, amount);
            spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
            });

        enemies.push_back(std::move(enemy));
    }

    // ===== TROLL 3 =====
    for (auto& pos : map->GetSpawn(6)) {
        auto enemy = std::make_unique<Elites>(
            renderer,
            glm::vec2(pos.x, pos.y),
            TrollType::TROLL_3
        );

        enemy->SetPatrolPoints(
            glm::vec2(pos.x - 90.0f, pos.y - 48.0f),
            glm::vec2(pos.x + 90.0f, pos.y - 48.0f)
        );

        enemy->SetAggroRange(100.0f);
        enemy->SetAttackRange(20.0f);
        enemy->SetCoinDropAmount(3);
        enemy->SetAudioSystem(audio);

        enemy->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
            spawnCoinAtPosition(pos, amount);
            spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
            });

        enemies.push_back(std::move(enemy));
    }

    // ===== BOSS =====
    if (currentMapName == "assets/tileset/Map_3.tmj") {          // XOng thi doi debugMap thanh currentMapName de ko bi lap lai va doi thanh map Map_1.tmj khi xong
        auto bossSpawn = map->GetSpawn(7);

        std::unique_ptr<Boss> bossPtr;
        if (!bossSpawn.empty()) {
            bossPtr = std::make_unique<Boss>(
                renderer,
                glm::vec2(bossSpawn[0].x, bossSpawn[0].y)
            );
        }
        else {
            bossPtr = std::make_unique<Boss>(
                renderer,
                glm::vec2(1300.0f, GameConstants::FLOOR_Y - 96.0f)
            );
        }

        Boss* boss = bossPtr.get();
        if (boss) {
            boss->SetEffectManager(&effectManager);
            boss->TriggerIntro();
        }

        bossPtr->SetCoinDropAmount(10);
        bossPtr->SetAudioSystem(audio);

        bossPtr->SetOnDeathCallback([this](glm::vec2 pos, int amount) {
            spawnCoinAtPosition(pos, amount);
            spawnHealthPotionAtPosition(pos + glm::vec2(0, 0));
            });

        enemies.push_back(std::move(bossPtr));
    }

    std::cout << "===== DA SPAWN " << enemies.size() << " ENEMIES =====\n";
}
// ===== SỬA HÀM updateEnemies() =====
void Game::updateEnemies(float deltaTime) {
    if (!player) return;

    glm::vec2 playerPos = player->GetPosition();

    // ✅ Duyệt và update tất cả enemies trước
    for (auto& enemy : enemies) {
        enemy->SetTargetPosition(playerPos);

        // ✅ THÊM: Kiểm tra enemy có rơi xuống hố không
        glm::vec2 enemyPos = enemy->GetPosition();
        if (enemyPos.y > GameConstants::WORLD_HEIGHT + 200.0f) {
            std::cout << "[Game] Enemy fell off map! Killing it.\n";
            enemy->TakeDamage(9999); // Kill ngay
        }

        // Trigger boss intro khi player đến gần
        if (enemy->GetEnemyType() == EnemyType::BOSS) {
            Boss* boss = dynamic_cast<Boss*>(enemy.get());
            if (boss && boss->IsInIntro()) {
                float distToBoss = glm::length(playerPos - boss->GetPosition());
                if (distToBoss < 300.0f) {
                    boss->TriggerIntro();
                    std::cout << "[Game] Boss fight begins!\n";
                }
            }
        }

        enemy->Update(deltaTime, *map);
    }

    // ✅ SAU ĐÓ mới xóa enemy chết (NGOÀI vòng lặp trên)
    size_t beforeSize = enemies.size();
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<Enemy>& e) {
                bool shouldRemove = !e->IsAlive() && e->GetDeathTimer() >= 0.6f;
                if (shouldRemove) {
                    std::cout << "[Game] Removing dead enemy after drop delay\n";
                }
                return shouldRemove;
            }),
        enemies.end()
    );
    size_t afterSize = enemies.size();
    
    if (beforeSize != afterSize) {
        std::cout << "[Game] Removed " << (beforeSize - afterSize) 
                  << " dead enemies. Remaining: " << afterSize << "\n";
    }
}

// ===== KIỂM TRA VA CHẠM =====
void Game::checkEnemyCollisions() {
    if (!player) return;

    SDL_FRect playerBox = player->GetBoundingBox();

    // --------------------------------------------------------------
    // PHẦN 1: PROJECTILE CỦA PLAYER BẮN TRÚNG ENEMY
    // --------------------------------------------------------------
    for (auto& proj : player->GetProjectiles()) {
        if (!proj->IsActive()) continue;

        for (auto& enemy : enemies) {
            if (!enemy->IsAlive()) continue;

            SDL_FRect projBox = proj->GetBoundingBox();
            SDL_FRect enemyBox = enemy->GetBoundingBox();

            if (SDL_HasRectIntersectionFloat(&projBox, &enemyBox)) {
                // 1. Enemy nhận damage
                enemy->TakeDamage(proj->GetDamage());

                // 2. Projectile biến mất
                proj->Deactivate();

                // 3. Tạo hiệu ứng nổ
                effectManager.CreateExplosion(
                    proj->GetPosition(),
                    15.0f,
                    SDL_Color{ 255, 150, 0, 255 }
                );

                // 4. Phát âm thanh
                if (audio) {
                    audio->playSound("assets/audio/bantrung.mp3", false, 0.5f);
                }

                // std::cout << "Projectile trung Enemy!\n";
                break; // Mũi tên trúng 1 quái rồi mất, không xuyên táo
            }
        }
    }

    // --------------------------------------------------------------
    // PHẦN 2: ENEMY TẤN CÔNG PLAYER (GỒM CẢ BOSS VŨ KHÍ & BODY)
    // --------------------------------------------------------------
    for (auto& enemy : enemies) {
        if (!enemy->IsAlive()) continue;

        // >>>>> [NEW] LOGIC HITBOX VŨ KHÍ RIÊNG CHO BOSS <<<<<
        if (enemy->GetEnemyType() == EnemyType::BOSS) {
            // Ép kiểu từ Enemy* sang Boss* để gọi hàm GetWeaponHitbox
            Boss* boss = dynamic_cast<Boss*>(enemy.get());

            if (boss) {
                SDL_FRect weaponBox = boss->GetWeaponHitbox();

                // Kiểm tra: (Hitbox đang bật) VÀ (Có va chạm với Player)
                if (weaponBox.w > 0.0f && SDL_HasRectIntersectionFloat(&weaponBox, &playerBox)) {

                    // Kiểm tra Player có đang bất tử không
                    if (!player->IsInvulnerable()) {
                        int damage = 0;

                        // Tính damage dựa trên chiêu thức Boss đang dùng
                        if (boss->IsUsingUltimate()) {
                            damage = 80; // Ultimate: Dam cực to
                            effectManager.TriggerScreenShake(15.0f, 0.6f); // Rung mạnh
                        }
                        else if (boss->IsSlamming()) {
                            damage = 50; // Slam: Dam vừa
                            effectManager.TriggerScreenShake(10.0f, 0.4f);
                        }
                        else {
                            damage = boss->GetAttackDamage(); // Đánh thường: Dam mặc định
                            effectManager.TriggerScreenShake(5.0f, 0.2f);
                        }

                        // Trừ máu Player
                        player->TakeDamage(damage);

                        // Hiệu ứng màn hình đỏ lòm khi dính đòn Boss
                        effectManager.CreateFlash(
                            player->GetPosition(),
                            SDL_Color{ 255, 50, 50, 200 }, // Đỏ đậm
                            0.5f,
                            0.2f
                        );

                        std::cout << "Player bi BOSS danh trung! Mat " << damage << " HP.\n";
                    }
                }
            }
        }

        // >>>>> LOGIC VA CHẠM THÂN THỂ (BODY COLLISION) <<<<<
        // (Áp dụng cho quái thường cắn, hoặc Boss lao người vào Player)
        if (enemy->GetEnemyState() == EnemyState::STATE_ATTACK) {
            SDL_FRect enemyBox = enemy->GetBoundingBox();

            if (SDL_HasRectIntersectionFloat(&playerBox, &enemyBox)) {

                // Nếu vừa dính đòn vũ khí Boss bên trên -> Player sẽ có i-frame -> dòng này sẽ bỏ qua
                if (!player->IsInvulnerable()) {
                    int damage = enemy->PerformAttack();

                    if (damage > 0) {
                        player->TakeDamage(damage);

                        // Hiệu ứng khác nhau giữa Boss và Quái thường
                        if (enemy->GetEnemyType() == EnemyType::BOSS) {
                            // Boss húc trúng
                            effectManager.CreateFlash(player->GetPosition(), { 255, 255, 255, 150 }, 0.4f, 0.15f);
                            effectManager.TriggerScreenShake(8.0f, 0.3f);
                        }
                        else {
                            // Quái thường cắn
                            effectManager.CreateFlash(player->GetPosition(), { 255, 0, 0, 60 }, 0.15f, 0.08f);
                            effectManager.TriggerScreenShake(3.0f, 0.2f);
                        }
                    }
                }
            }
        }
    }
}
void Game::startTransition(GameState nextState) {
    currentGameState = GameState::TRANSITIONING;
    targetState = nextState;
    mainMenu->SetTransitioning(true);
    screenTransition->Start();

    std::cout << "Bat dau transition den state: " << (int)nextState << "\n";
}
void Game::resetGame() {
    currentMapName = "assets/tileset/Map_1.tmj";
    enemies.clear();
    items.clear();
    if (map) {
        delete map;
        map = nullptr;
    }
    if (player) {
        delete player;
        player = nullptr;
    }
    map = new Map(renderer);
    if (!map->loadMap(currentMapName)) {
        return;
    }
    auto playerSpawns = map->GetSpawn(0);
    if (!playerSpawns.empty()) {
        player = new Player(renderer, glm::vec2(playerSpawns[0].x, playerSpawns[0].y));
        player->SnapToGround(*map);
        playerStartPos = player->GetPosition();
    }
    if (playerHUD) {
        playerHUD->Reset();
        playerHUD->SetPlayerReference(player);
        playerHUD->Update(0.0f);
    }
    initEnemies();
    initItems();
    if (audio) {
        audio->stopBGM();
        audio->playBGM("assets/audio/breath.mp3", true, 0.4f);
    }
    currentGameState = GameState::PLAYING;
    pendingNextMap = false;
    nextMapName.clear();
    if (mainMenu) mainMenu->Reset();
    if (gameOverMenu) gameOverMenu->Reset();
    if (gameWinMenu) gameWinMenu->Reset();
    if (tutorialMenu) tutorialMenu->Reset();
}

// ===== THÊM SAFE GUARD CHO LoadNextMap() =====
void Game::LoadNextMap() {
    std::cout << "[Game] ===== LOADING NEXT MAP =====\n";
    std::cout << "[Game] Current map: " << currentMapName << "\n";

    // Xác định map kế tiếp
    std::string nextMap;
    if (currentMapName == "assets/tileset/Map_1.tmj") {
        nextMap = "assets/tileset/Map_2.tmj";
    }
    else if (currentMapName == "assets/tileset/Map_2.tmj") {
        nextMap = "assets/tileset/Map_3.tmj";
    }
    else {
        std::cout << "[Game] No more maps! Victory!\n";
        currentGameState = GameState::GAME_OVER;
        return;
    }
    
    std::cout << "[Game] Next map: " << nextMap << "\n";
    currentMapName = nextMap;

    // ✅ Clear dữ liệu cũ TRƯỚC KHI load map mới
    enemies.clear();
    //items.clear();
    std::cout << "[Game] Cleared old data.\n";

    // ✅ Xóa map cũ an toàn
    if (map) {
        delete map;
        map = nullptr;
    }
    
    map = new Map(renderer);
    if (!map) {
        std::cerr << "[Game] ERROR: Failed to create new map!\n";
        isGameRunning = false;
        return;
    }

    // ✅ Load map mới
    if (!map->loadMap(currentMapName)) {
        std::cerr << "[Game] ERROR: Failed to load map: " << currentMapName << std::endl;
        isGameRunning = false;
        return;
    }

    std::cout << "[Game] Map loaded successfully: " << currentMapName << std::endl;

    // ✅ Spawn mới
    try {
        initEnemies();
        initItems();
        std::cout << "[Game] Spawned " << enemies.size() << " enemies and " 
                  << items.size() << " items.\n";
    } catch (const std::exception& e) {
        std::cerr << "[Game] ERROR during spawn: " << e.what() << std::endl;
        isGameRunning = false;
        return;
    }

    // ✅ Cập nhật lại vị trí player
    auto spawn = map->GetSpawn(0);
    if (!spawn.empty()) {
        player->SetPosition(glm::vec2(spawn[0].x, spawn[0].y));
        player->SnapToGround(*map);
        std::cout << "[Game] Player spawned at (" << spawn[0].x << ", " << spawn[0].y << ")\n";
    } else {
        std::cerr << "[Game] WARNING: No player spawn point found!\n";
    }

    // ✅ Reset HUD
    if (playerHUD) {
        playerHUD->SetPlayerReference(player);
    }

    // ✅ Đổi nhạc theo map (tuỳ chọn)
    if (audio) {
        audio->stopBGM();
        audio->playBGM("assets/audio/breath.mp3", true, 0.4f);
    }

    std::cout << "[Game] ===== MAP TRANSITION COMPLETE =====\n";
}

// ===== THÊM HÀM MỚI: XÓA PROJECTILE KHI RA KHỎI MÀN HÌNH CAMERA =====
void Game::checkProjectileBounds() {
    if (!player) return;

    // 1. Lấy offset camera và kích thước màn hình logic
    glm::vec2 cameraOffset = camera.getOffset();
    float screenWidth = GameConstants::LOGICAL_WIDTH;  // Lấy từ GameConstants
    float screenHeight = GameConstants::LOGICAL_HEIGHT; // Lấy từ GameConstants

    // 2. Định nghĩa vùng an toàn (buffer) ngoài màn hình 
    // Mũi tên cần bay ra khỏi vùng này mới bị xóa.
    const float BOUNDS_BUFFER = 50.0f; // 50 pixel buffer

    // 3. Sử dụng erase-remove idiom để xóa các projectile ra khỏi vector
    auto& projectiles = player->GetProjectiles();

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [&](const std::unique_ptr<Projectile>& proj) {
                if (!proj) return true; // Xóa nếu con trỏ rỗng

                glm::vec2 pos = proj->GetPosition();

                // Kiểm tra nếu projectile hoàn toàn ngoài vùng camera + buffer
                bool isOutOfBounds =
                    pos.x < cameraOffset.x - BOUNDS_BUFFER ||                    // Bên trái camera
                    pos.x > cameraOffset.x + screenWidth + BOUNDS_BUFFER ||      // Bên phải camera
                    pos.y < cameraOffset.y - BOUNDS_BUFFER ||                    // Bên trên camera
                    pos.y > cameraOffset.y + screenHeight + BOUNDS_BUFFER;       // Bên dưới camera

                return isOutOfBounds;
            }),
        projectiles.end());
}
