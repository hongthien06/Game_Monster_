#include "Game.h"
#include "Camera.h"
#include <iostream>
#include <algorithm>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

// Khoi tao game
Game::Game()
    : window(nullptr),
    renderer(nullptr),
    isGameRunning(true),
    idleTex(nullptr),
    walkTex(nullptr),
    runTex(nullptr),
    currentState(PlayerState::STATE_IDLE),
    previousState(PlayerState::STATE_IDLE),
    playerPos(150.0f, GameConstants::FLOOR_Y),
    playerVelocityX(0.0f),
    flipHorizontal(false),
    currentFrame(0),
    animationTimer(0.0f),
    camera( 
        GameConstants::LOGICAL_WIDTH,
        GameConstants::LOGICAL_HEIGHT,
        GameConstants::WORLD_WIDTH,
        GameConstants::WORLD_HEIGHT
    )
{
}

// Don dep
Game::~Game() {
    cleanup();
}

// Khoi tao SDL 
bool Game::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tao cua so window
    window = SDL_CreateWindow("Game Monster",GameConstants::SCREEN_WIDTH,GameConstants::SCREEN_HEIGHT,SDL_WINDOW_RESIZABLE);
    
    
    if (!window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tao renderer de ve nhan vat
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tao nhan vat
    SDL_SetRenderLogicalPresentation(renderer,GameConstants::LOGICAL_WIDTH,GameConstants::LOGICAL_HEIGHT,SDL_LOGICAL_PRESENTATION_LETTERBOX);
    idleTex = IMG_LoadTexture(renderer, "assets/images/idle.png");
    walkTex = IMG_LoadTexture(renderer, "assets/images/walk.png");
    runTex = IMG_LoadTexture(renderer, "assets/images/run.png");
    if (!idleTex || !walkTex || !runTex) {
        std::cerr << "Texture Load Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(walkTex, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(runTex, SDL_SCALEMODE_NEAREST);

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

// Xu ly phim thoat cua so
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isGameRunning = false;
        }
    }
    SDL_PumpEvents();
}

// Xu ly input dau vao va logic chuyen dong
void Game::update(float deltaTime) {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDirection = 0;

    // A -> trai - D -> phai
    if (keys[SDL_SCANCODE_A]) moveDirection = -1;
    if (keys[SDL_SCANCODE_D]) moveDirection = 1;

    // Shift de chay nhanh
    bool isPressingRun = keys[SDL_SCANCODE_LSHIFT];

    // Chuyen doi trang thai giua chay, di bo va dung yen
    if (moveDirection != 0) {
        currentState = isPressingRun ? PlayerState::STATE_RUNNING : PlayerState::STATE_WALKING;
    }
    else {
        currentState = PlayerState::STATE_IDLE;
    }
    float currentMaxSpeed = isPressingRun ? GameConstants::RUN_SPEED : GameConstants::WALK_SPEED;


    // Tang giam van toc nhan vat
    if (moveDirection) {
        playerVelocityX += GameConstants::ACCELERATION * moveDirection * deltaTime;
        flipHorizontal = (moveDirection == -1);
    }
    else {
        if (playerVelocityX > 0)
            playerVelocityX = std::max(0.0f, playerVelocityX - GameConstants::DECELERATION * deltaTime);
        else if (playerVelocityX < 0)
            playerVelocityX = std::min(0.0f, playerVelocityX + GameConstants::DECELERATION * deltaTime);
    }

    // Gioi han van toc 
    playerVelocityX = std::clamp(playerVelocityX, -currentMaxSpeed, currentMaxSpeed);
    playerPos.x += playerVelocityX * deltaTime;
    const float drawSize = 48.0f;
    playerPos.x = std::clamp(playerPos.x, 0.0f, GameConstants::WORLD_WIDTH - drawSize);


    if (currentState != previousState) {
        currentFrame = 0;
        animationTimer = 0.0f;
    }
    previousState = currentState;

    int totalFrames = 0;
    float frameDuration = 0;

    switch (currentState) {
    case PlayerState::STATE_IDLE:
        totalFrames = GameConstants::IDLE_FRAMES;
        frameDuration = GameConstants::IDLE_FRAME_DURATION;
        break;
    case PlayerState::STATE_WALKING:
        totalFrames = GameConstants::WALK_FRAMES;
        frameDuration = GameConstants::WALK_FRAME_DURATION;
        break;
    case PlayerState::STATE_RUNNING:
        totalFrames = GameConstants::RUN_FRAMES;
        frameDuration = GameConstants::RUN_FRAME_DURATION;
        break;
    }

    animationTimer += deltaTime;
    if (animationTimer >= frameDuration && totalFrames > 0) {
        animationTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    camera.update(playerPos, deltaTime);
}

// Ve nhan vat len man hinh 
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Texture* currentTexture = nullptr;
    int totalFrames = 0;
    float frameWidth = 0;
    float frameHeight = 0;

    // Chon hoat anh theo trang thai nhan vat
    switch (currentState) {
    case PlayerState::STATE_IDLE:
        currentTexture = idleTex;
        totalFrames = GameConstants::IDLE_FRAMES;
        frameWidth = GameConstants::IDLE_FRAME_WIDTH;
        frameHeight = GameConstants::IDLE_FRAME_HEIGHT;
        break;
    case PlayerState::STATE_WALKING:
        currentTexture = walkTex;
        totalFrames = GameConstants::WALK_FRAMES;
        frameWidth = GameConstants::WALK_FRAME_WIDTH;
        frameHeight = GameConstants::WALK_FRAME_HEIGHT;
        break;
    case PlayerState::STATE_RUNNING:
        currentTexture = runTex;
        totalFrames = GameConstants::RUN_FRAMES;
        frameWidth = GameConstants::RUN_FRAME_WIDTH;
        frameHeight = GameConstants::RUN_FRAME_HEIGHT;
        break;
    }

    glm::vec2 offset = camera.getOffset();

    if (currentTexture) {
        SDL_FRect src = {
            (float)currentFrame * frameWidth,
            0,
            frameWidth,
            frameHeight
        };

        // Kich thuoc nhan vat tren man hinh
        const float drawSize = 48.0f; 

        // Vi tri nhan vat hien thi
        SDL_FRect dst = {
             playerPos.x - offset.x,
             GameConstants::FLOOR_Y - drawSize,
             drawSize,
             drawSize
        };

        SDL_RenderTextureRotated(renderer,currentTexture,&src,&dst,0.0,nullptr,flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }

    SDL_RenderPresent(renderer);
}


void Game::cleanup() {
    SDL_DestroyTexture(idleTex);
    SDL_DestroyTexture(walkTex);
    SDL_DestroyTexture(runTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
