#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include "Camera.h"   
#include <glm/glm.hpp>

namespace GameConstants {
    // Kich thuoc cua cua so window
    constexpr int SCREEN_WIDTH = 1280;
    constexpr int SCREEN_HEIGHT = 800;
    // Kich thuoc logic cua khung hinh
    constexpr int LOGICAL_WIDTH = 400;
    constexpr int LOGICAL_HEIGHT = 300;
    // Toa do mat dat
    constexpr float FLOOR_Y = LOGICAL_HEIGHT - 48.0f;

    // Dung yen
    constexpr int IDLE_FRAMES = 6;
    constexpr float IDLE_FRAME_DURATION = 0.15f;
    constexpr int IDLE_FRAME_WIDTH = 128;
    constexpr int IDLE_FRAME_HEIGHT = 128;

    // Di bo
    constexpr int WALK_FRAMES = 8;
    constexpr float WALK_FRAME_DURATION = 0.1f;
    constexpr int WALK_FRAME_WIDTH = 128;
    constexpr int WALK_FRAME_HEIGHT = 128;

    // Chay 
    constexpr int RUN_FRAMES = 8;
    constexpr float RUN_FRAME_DURATION = 0.08f;
    constexpr int RUN_FRAME_WIDTH = 128;
    constexpr int RUN_FRAME_HEIGHT = 128;

    // Gia tri chuyen dong
    constexpr float WALK_SPEED = 80.0f;
    constexpr float RUN_SPEED = 160.0f;
    constexpr float ACCELERATION = 500.0f;
    constexpr float DECELERATION = 400.0f;

    // Gia tri camera
    constexpr float WORLD_WIDTH = 2000.0f;   
    constexpr float WORLD_HEIGHT = 600.0f;
}

// Enum luu giu trang thai nhan vat
enum class PlayerState {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING
};

class Game {
public:
    Game(); 
    ~Game();
    void run(); // Ham chay game

private:
    bool init();          // Khoi tao
    void handleEvents();  // Xu ly input
    void update(float deltaTime); // Cap nhat logic game
    void render();        // Ve len man hinh
    void cleanup();       // Don dep tai nguyen

    SDL_Window* window; 
    SDL_Renderer* renderer;
    bool isGameRunning;

    // Texture nhan vat
    SDL_Texture* idleTex;
    SDL_Texture* walkTex;
    SDL_Texture* runTex;

    // Player
    PlayerState currentState;
    PlayerState previousState;

    glm::vec2 playerPos;
    float playerVelocityX;
    bool flipHorizontal;

    // Animation
    int currentFrame;
    float animationTimer;

    Camera camera;
};
