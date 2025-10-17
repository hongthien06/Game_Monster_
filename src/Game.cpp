#include "Game.h"
#include <iostream>
#include <algorithm>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

Game::Game()
    : window(nullptr),
    renderer(nullptr),
    isGameRunning(true),
    idleTex(nullptr),
    walkTex(nullptr),
    runTex(nullptr),
    currentState(PlayerState::STATE_IDLE),
    previousState(PlayerState::STATE_IDLE),
    playerX(150.0f),
    playerVelocityX(0.0f),
    flipHorizontal(false),
    currentFrame(0),
    animationTimer(0.0f)
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

    window = SDL_CreateWindow("Game Monster",GameConstants::SCREEN_WIDTH,GameConstants::SCREEN_HEIGHT,SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

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
        if (deltaTime > (1.0f / 30.0f)) deltaTime = (1.0f / 30.0f);
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
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int moveDirection = 0;

    if (keys[SDL_SCANCODE_A]) moveDirection = -1;
    if (keys[SDL_SCANCODE_D]) moveDirection = 1;

    bool isPressingRun = keys[SDL_SCANCODE_LSHIFT];

    if (moveDirection != 0) {
        currentState = isPressingRun ? PlayerState::STATE_RUNNING : PlayerState::STATE_WALKING;
    }
    else {
        currentState = PlayerState::STATE_IDLE;
    }

    float currentMaxSpeed = isPressingRun ? GameConstants::RUN_SPEED : GameConstants::WALK_SPEED;

    if (moveDirection) {
        playerVelocityX += GameConstants::ACCELERATION * moveDirection * deltaTime;
        flipHorizontal = (moveDirection == -1);
    }
    else {
        if (playerVelocityX > 0) playerVelocityX = std::max(0.0f, playerVelocityX - GameConstants::DECELERATION * deltaTime);
        if (playerVelocityX < 0) playerVelocityX = std::min(0.0f, playerVelocityX + GameConstants::DECELERATION * deltaTime);
    }

    playerVelocityX = std::max(-currentMaxSpeed, std::min(playerVelocityX, currentMaxSpeed));
    playerX += playerVelocityX * deltaTime;

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
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Texture* currentTexture = nullptr;
    int totalFrames = 0;
    float frameWidth = 0;
    float frameHeight = 0;

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

    if (currentTexture) {
        SDL_FRect src = {
            (float)currentFrame * frameWidth,
            0,
            frameWidth,
            frameHeight
        };
        const float drawSize = 64.0f; 
        SDL_FRect dst = {
            playerX,
            GameConstants::FLOOR_Y - drawSize,
            drawSize,
            drawSize
        };

        SDL_RenderTextureRotated(
            renderer,
            currentTexture,
            &src,
            &dst,
            0.0,
            nullptr,
            flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE
        );
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
