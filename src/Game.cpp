#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Game.h"
#include "Camera.h"
#include "Map.h"
#include "Player.h"
#include <iostream>
#include <algorithm>

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
        GameConstants::WORLD_HEIGHT)
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

    map = new Map(renderer);
    map->LoadTiles();
    SDL_FPoint spawnPoint = map->GetSpawnPoint();
    map->CleanSpawnTile();
    player = new Player(renderer);
    player->SetPosition({ spawnPoint.x, spawnPoint.y });
    std::cout << "Vi tri nhan vat: (" << spawnPoint.x << ", " << spawnPoint.y << ")" << std::endl;

    return true;
}

void Game::run() {
    if (!init()) return;

    uint64_t prevTime = SDL_GetTicks();

    while (isGameRunning) {
        uint64_t nowTime = SDL_GetTicks();
        float deltaTime = (nowTime - prevTime) / 1000.0f;
        prevTime = nowTime;

        if (deltaTime > (1.0f / 60.0f)) deltaTime = (1.0f / 60.0f);

        handleEvents();
        update(deltaTime);
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isGameRunning = false;
        }
    }
    SDL_PumpEvents();
}

void Game::update(float deltaTime) {
    SDL_PumpEvents();
    if (player) player->Update(deltaTime);

    camera.update(player->GetPosition(), deltaTime);
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 200, 255);
    SDL_RenderClear(renderer);

    glm::vec2 offset = camera.getOffset();
    if (map) map->DrawMap(offset);
    if (player) player->Render(renderer, offset);

    SDL_RenderPresent(renderer);
}

void Game::cleanup() {
    delete player;
    delete map;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
