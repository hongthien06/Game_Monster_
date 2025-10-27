#pragma once


namespace GameConstants {
    // Kich thuoc cua cua so window
    constexpr int SCREEN_WIDTH = 1280;
    constexpr int SCREEN_HEIGHT = 800;
    // Kich thuoc logic cua khung hinh
    constexpr int LOGICAL_WIDTH = 400;
    constexpr int LOGICAL_HEIGHT = 300;
    // Toa do mat dat
    constexpr float FLOOR_Y = LOGICAL_HEIGHT - 32.0f;

    // Dung yen
    constexpr int IDLE_FRAMES = 4;
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

    // Nhay
    constexpr int JUMP_FRAMES = 9;
    constexpr float GRAVITY = 800.0f;
    constexpr float JUMP_SPEED = 250.0f;
    constexpr float PLAYER_HEIGHT = 32.0f;
    constexpr float JUMP_FRAME_DURATION = 0.12f;
    constexpr int JUMP_FRAME_WIDTH = 128;
    constexpr int JUMP_FRAME_HEIGHT = 128;

    // Tan Cong
    constexpr int ATTACK_FRAMES = 4;
    constexpr float ATTACK_FRAME_DURATION = 0.07f;
    constexpr int ATTACK_FRAME_WIDTH = 128;
    constexpr int ATTACK_FRAME_HEIGHT = 128;

    // Ban
    constexpr int SHOT_FRAMES = 14;
    constexpr float SHOT_FRAME_DURATION = 0.08f;
    constexpr int SHOT_FRAME_WIDTH = 128;
    constexpr int SHOT_FRAME_HEIGHT = 128;

    // Chay mau
    constexpr int HURT_FRAMES = 3;
    constexpr int HURT_FRAME_WIDTH = 128;
    constexpr int HURT_FRAME_HEIGHT = 128;

    // Chet
    constexpr int DEAD_FRAMES = 3;
    constexpr float DEAD_FRAME_DURATION = 0.15f;
    constexpr int DEAD_FRAME_WIDTH = 128;
    constexpr int DEAD_FRAME_HEIGHT = 128;

    // Gia tri chuyen dong
    constexpr float WALK_SPEED = 80.0f;
    constexpr float RUN_SPEED = 160.0f;
    constexpr float ACCELERATION = 500.0f;
    constexpr float DECELERATION = 400.0f;

    // Gia tri camera
    constexpr float WORLD_WIDTH = 2000.0f;
    constexpr float WORLD_HEIGHT = 600.0f;
}