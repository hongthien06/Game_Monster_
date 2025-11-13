#pragma once


namespace GameConstants {
    // Kich thuoc cua cua so window
    constexpr int SCREEN_WIDTH = 1280;
    constexpr int SCREEN_HEIGHT = 800;
    // Kich thuoc logic cua khung hinh
    constexpr int LOGICAL_WIDTH = 400;
    constexpr int LOGICAL_HEIGHT = 300;
    // Toa do mat dat
    constexpr float FLOOR_Y = LOGICAL_HEIGHT - 28.0f;

    // Dung yen
    constexpr int IDLE_FRAMES = 4;//10
    constexpr float IDLE_FRAME_DURATION = 0.15f;
    constexpr int IDLE_FRAME_WIDTH = 128;
    constexpr int IDLE_FRAME_HEIGHT = 128;

    // Di bo
    constexpr int WALK_FRAMES = 8;//10
    constexpr float WALK_FRAME_DURATION = 0.1f;
    constexpr int WALK_FRAME_WIDTH = 128;
    constexpr int WALK_FRAME_HEIGHT = 128;
    constexpr float WALK_SPEED = 80.0f;

    // Chay 
    constexpr int RUN_FRAMES = 8;//10
    constexpr float RUN_FRAME_DURATION = 0.06f;
    constexpr int RUN_FRAME_WIDTH = 128;
    constexpr int RUN_FRAME_HEIGHT = 128;
    constexpr float RUN_SPEED = 160.0f;

    // Nhay
    constexpr int JUMP_FRAMES = 9;//10
    constexpr float GRAVITY = 800.0f;
    constexpr float JUMP_SPEED = 250.0f;
    constexpr float JUMP_FRAME_DURATION = 0.12f;
    constexpr int JUMP_FRAME_WIDTH = 128;
    constexpr int JUMP_FRAME_HEIGHT = 128;

    // Tan Cong
    constexpr int ATTACK_FRAMES = 4;//10
    constexpr float ATTACK_FRAME_DURATION = 0.08f;
    constexpr int ATTACK_FRAME_WIDTH = 128;
    constexpr int ATTACK_FRAME_HEIGHT = 128;

    // Ban
    constexpr int SHOT_FRAMES = 14;
    constexpr float SHOT_FRAME_DURATION = 0.05f;
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

    //Dash
    constexpr int DASH_FRAMES = 8;
    constexpr float DASH_FRAME_DURATION = 0.05f;
    constexpr int DASH_FRAME_WIDTH = 128;
    constexpr int DASH_FRAME_HEIGHT = 128;
    constexpr float DASH_SPEED = 300.0f;
    constexpr float DASH_DURATION = 0.15f;
    constexpr float DASH_COOLDOWN = 1.5f;

    // Gia tri chuyen dong
    constexpr float ACCELERATION = 500.0f;
    constexpr float DECELERATION = 400.0f;

    // Gia tri va cham nhan vat
    constexpr float PLAYER_RENDER_WIDTH = 48.0f;
    constexpr float PLAYER_RENDER_HEIGHT = 48.0f;

    constexpr float PLAYER_PHYSICS_BOX_WIDTH = 11.0f;
    constexpr float PLAYER_PHYSICS_BOX_HEIGHT = 28.0f;

    constexpr float PLAYER_PHYSICS_BOX_OFFSET_X = (PLAYER_RENDER_WIDTH - PLAYER_PHYSICS_BOX_WIDTH) / 2.0f;
    constexpr float PLAYER_PHYSICS_BOX_OFFSET_Y = (PLAYER_RENDER_HEIGHT - PLAYER_PHYSICS_BOX_HEIGHT);

    // Gia tri camera
    constexpr float WORLD_WIDTH = 2000.0f;
    constexpr float WORLD_HEIGHT = 600.0f;

    // Item coin
    constexpr int COIN_FRAMES = 9;
    constexpr float COIN_FRAME_DURATION = 0.1f;
    constexpr int COIN_FRAME_WIDTH = 20;
    constexpr int COIN_FRAME_HEIGHT = 20;

    //item health potion
    constexpr int HEALTH_POTION_FRAMES = 1;
    constexpr float HEALTH_POTION_FRAME_DURATION = 0.1f;
    constexpr int HEALTH_POTION_FRAME_WIDTH = 64;
    constexpr int HEALTH_POTION_FRAME_HEIGHT = 64;
    constexpr int HEALTH_POTION_HEAL_AMOUNT = 30;

    //boss
    constexpr int BOSS_IDLE_FRAMES = 10;
    constexpr float BOSS_IDLE_FRAME_DURATION = 0.12f;
    constexpr int BOSS_IDLE_FRAME_WIDTH = 1600;
    constexpr int BOSS_IDLE_FRAME_HEIGHT = 1000;

    constexpr int BOSS_WALK_FRAMES = 10;
    constexpr float BOSS_WALK_FRAME_DURATION = 0.10f;
    constexpr int BOSS_WALK_FRAME_WIDTH = 1600;
    constexpr int BOSS_WALK_FRAME_HEIGHT = 1000;

    constexpr int BOSS_RUN_FRAMES = 10;
    constexpr float BOSS_RUN_FRAME_DURATION = 0.08f;
    constexpr int BOSS_RUN_FRAME_WIDTH = 1600;
    constexpr int BOSS_RUN_FRAME_HEIGHT = 1000;

    constexpr int BOSS_JUMP_FRAMES = 10;
    constexpr float BOSS_JUMP_FRAME_DURATION = 0.12f;
    constexpr int BOSS_JUMP_FRAME_WIDTH = 1600;
    constexpr int BOSS_JUMP_FRAME_HEIGHT = 1000;

    constexpr int BOSS_ATTACK_FRAMES = 10;
    constexpr float BOSS_ATTACK_FRAME_DURATION = 0.09f;
    constexpr int BOSS_ATTACK_FRAME_WIDTH = 1600;
    constexpr int BOSS_ATTACK_FRAME_HEIGHT = 1000;

    constexpr int BOSS_HURT_FRAMES = 10;
    constexpr float BOSS_HURT_FRAME_DURATION = 0.10f;
    constexpr int BOSS_HURT_FRAME_WIDTH = 1600;
    constexpr int BOSS_HURT_FRAME_HEIGHT = 1000;

    constexpr int BOSS_DEAD_FRAMES = 10;
    constexpr float BOSS_DEAD_FRAME_DURATION = 0.15f;
    constexpr int BOSS_DEAD_FRAME_WIDTH = 1600;
    constexpr int BOSS_DEAD_FRAME_HEIGHT = 1000;

    // ===================== ELITE TROLL TYPE 1 =====================
    constexpr int ELITE_TROLL_1_IDLE_FRAMES = 10;
    constexpr float ELITE_TROLL_1_IDLE_FRAME_DURATION = 0.12f;
    constexpr int ELITE_TROLL_1_IDLE_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_1_IDLE_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_1_WALK_FRAMES = 10;
    constexpr float ELITE_TROLL_1_WALK_FRAME_DURATION = 0.10f;
    constexpr int ELITE_TROLL_1_WALK_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_1_WALK_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_1_RUN_FRAMES = 10;
    constexpr float ELITE_TROLL_1_RUN_FRAME_DURATION = 0.08f;
    constexpr int ELITE_TROLL_1_RUN_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_1_RUN_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_1_JUMP_FRAMES = 10;
    constexpr float ELITE_TROLL_1_JUMP_FRAME_DURATION = 0.12f;
    constexpr int ELITE_TROLL_1_JUMP_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_1_JUMP_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_1_ATTACK_FRAMES = 10;
    constexpr float ELITE_TROLL_1_ATTACK_FRAME_DURATION = 0.09f;
    constexpr int ELITE_TROLL_1_ATTACK_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_1_ATTACK_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_1_HURT_FRAMES = 10;
    constexpr float ELITE_TROLL_1_HURT_FRAME_DURATION = 0.10f;
    constexpr int ELITE_TROLL_1_HURT_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_1_HURT_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_1_DEAD_FRAMES = 10;
    constexpr float ELITE_TROLL_1_DEAD_FRAME_DURATION = 0.15f;
    constexpr int ELITE_TROLL_1_DEAD_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_1_DEAD_FRAME_HEIGHT = 1000;


    // ===================== ELITE TROLL TYPE 2 =====================
    constexpr int ELITE_TROLL_2_IDLE_FRAMES = 10;
    constexpr float ELITE_TROLL_2_IDLE_FRAME_DURATION = 0.12f;
    constexpr int ELITE_TROLL_2_IDLE_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_2_IDLE_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_2_WALK_FRAMES = 10;
    constexpr float ELITE_TROLL_2_WALK_FRAME_DURATION = 0.10f;
    constexpr int ELITE_TROLL_2_WALK_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_2_WALK_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_2_RUN_FRAMES = 10;
    constexpr float ELITE_TROLL_2_RUN_FRAME_DURATION = 0.08f;
    constexpr int ELITE_TROLL_2_RUN_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_2_RUN_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_2_JUMP_FRAMES = 10;
    constexpr float ELITE_TROLL_2_JUMP_FRAME_DURATION = 0.12f;
    constexpr int ELITE_TROLL_2_JUMP_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_2_JUMP_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_2_ATTACK_FRAMES = 10;
    constexpr float ELITE_TROLL_2_ATTACK_FRAME_DURATION = 0.09f;
    constexpr int ELITE_TROLL_2_ATTACK_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_2_ATTACK_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_2_HURT_FRAMES = 10;
    constexpr float ELITE_TROLL_2_HURT_FRAME_DURATION = 0.10f;
    constexpr int ELITE_TROLL_2_HURT_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_2_HURT_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_2_DEAD_FRAMES = 10;
    constexpr float ELITE_TROLL_2_DEAD_FRAME_DURATION = 0.15f;
    constexpr int ELITE_TROLL_2_DEAD_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_2_DEAD_FRAME_HEIGHT = 1000;


    // ===================== ELITE TROLL TYPE 3 =====================
    constexpr int ELITE_TROLL_3_IDLE_FRAMES = 10;
    constexpr float ELITE_TROLL_3_IDLE_FRAME_DURATION = 0.12f;
    constexpr int ELITE_TROLL_3_IDLE_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_3_IDLE_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_3_WALK_FRAMES = 10;
    constexpr float ELITE_TROLL_3_WALK_FRAME_DURATION = 0.10f;
    constexpr int ELITE_TROLL_3_WALK_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_3_WALK_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_3_RUN_FRAMES = 10;
    constexpr float ELITE_TROLL_3_RUN_FRAME_DURATION = 0.08f;
    constexpr int ELITE_TROLL_3_RUN_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_3_RUN_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_3_JUMP_FRAMES = 10;
    constexpr float ELITE_TROLL_3_JUMP_FRAME_DURATION = 0.12f;
    constexpr int ELITE_TROLL_3_JUMP_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_3_JUMP_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_3_ATTACK_FRAMES = 10;
    constexpr float ELITE_TROLL_3_ATTACK_FRAME_DURATION = 0.09f;
    constexpr int ELITE_TROLL_3_ATTACK_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_3_ATTACK_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_3_HURT_FRAMES = 10;
    constexpr float ELITE_TROLL_3_HURT_FRAME_DURATION = 0.10f;
    constexpr int ELITE_TROLL_3_HURT_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_3_HURT_FRAME_HEIGHT = 1000;

    constexpr int ELITE_TROLL_3_DEAD_FRAMES = 10;
    constexpr float ELITE_TROLL_3_DEAD_FRAME_DURATION = 0.15f;
    constexpr int ELITE_TROLL_3_DEAD_FRAME_WIDTH = 1600;
    constexpr int ELITE_TROLL_3_DEAD_FRAME_HEIGHT = 1000;

    // ===================== MINION BERSERK =====================
    constexpr int MINION_BERSERK_IDLE_FRAMES = 5;
    constexpr float MINION_BERSERK_IDLE_FRAME_DURATION = 0.15f;
    constexpr int MINION_BERSERK_IDLE_FRAME_WIDTH = 96;
    constexpr int MINION_BERSERK_IDLE_FRAME_HEIGHT = 96;

    constexpr int MINION_BERSERK_WALK_FRAMES = 7;
    constexpr float MINION_BERSERK_WALK_FRAME_DURATION = 0.10f;
    constexpr int MINION_BERSERK_WALK_FRAME_WIDTH = 96;
    constexpr int MINION_BERSERK_WALK_FRAME_HEIGHT = 96;

    constexpr int MINION_BERSERK_RUN_FRAMES = 6;
    constexpr float MINION_BERSERK_RUN_FRAME_DURATION = 0.07f;
    constexpr int MINION_BERSERK_RUN_FRAME_WIDTH = 96;
    constexpr int MINION_BERSERK_RUN_FRAME_HEIGHT = 96;

    constexpr int MINION_BERSERK_JUMP_FRAMES = 5;
    constexpr float MINION_BERSERK_JUMP_FRAME_DURATION = 0.12f;
    constexpr int MINION_BERSERK_JUMP_FRAME_WIDTH = 96;
    constexpr int MINION_BERSERK_JUMP_FRAME_HEIGHT = 96;

    constexpr int MINION_BERSERK_ATTACK_FRAMES = 9;
    constexpr float MINION_BERSERK_ATTACK_FRAME_DURATION = 0.08f;
    constexpr int MINION_BERSERK_ATTACK_FRAME_WIDTH = 96;
    constexpr int MINION_BERSERK_ATTACK_FRAME_HEIGHT = 96;

    constexpr int MINION_BERSERK_DEAD_FRAMES = 4;
    constexpr float MINION_BERSERK_DEAD_FRAME_DURATION = 0.15f;
    constexpr int MINION_BERSERK_DEAD_FRAME_WIDTH = 96;
    constexpr int MINION_BERSERK_DEAD_FRAME_HEIGHT = 96;

    constexpr int MINION_BERSERK_HURT_FRAMES = 2;
    constexpr float MINION_BERSERK_HURT_FRAME_DURATION = 0.10f;
    constexpr int MINION_BERSERK_HURT_FRAME_WIDTH = 96;
    constexpr int MINION_BERSERK_HURT_FRAME_HEIGHT = 96;


    // ===================== MINION SHAMAN =====================
    constexpr int MINION_SHAMAN_IDLE_FRAMES = 5;
    constexpr float MINION_SHAMAN_IDLE_FRAME_DURATION = 0.15f;
    constexpr int MINION_SHAMAN_IDLE_FRAME_WIDTH = 96;
    constexpr int MINION_SHAMAN_IDLE_FRAME_HEIGHT = 96;

    constexpr int MINION_SHAMAN_WALK_FRAMES = 7;
    constexpr float MINION_SHAMAN_WALK_FRAME_DURATION = 0.10f;
    constexpr int MINION_SHAMAN_WALK_FRAME_WIDTH = 96;
    constexpr int MINION_SHAMAN_WALK_FRAME_HEIGHT = 96;

    constexpr int MINION_SHAMAN_RUN_FRAMES = 6;
    constexpr float MINION_SHAMAN_RUN_FRAME_DURATION = 0.07f;
    constexpr int MINION_SHAMAN_RUN_FRAME_WIDTH = 96;
    constexpr int MINION_SHAMAN_RUN_FRAME_HEIGHT = 96;

    constexpr int MINION_SHAMAN_JUMP_FRAMES = 6;
    constexpr float MINION_SHAMAN_JUMP_FRAME_DURATION = 0.12f;
    constexpr int MINION_SHAMAN_JUMP_FRAME_WIDTH = 96;
    constexpr int MINION_SHAMAN_JUMP_FRAME_HEIGHT = 96;

    constexpr int MINION_SHAMAN_ATTACK_FRAMES = 4;
    constexpr float MINION_SHAMAN_ATTACK_FRAME_DURATION = 0.09f;
    constexpr int MINION_SHAMAN_ATTACK_FRAME_WIDTH = 96;
    constexpr int MINION_SHAMAN_ATTACK_FRAME_HEIGHT = 96;

    constexpr int MINION_SHAMAN_DEAD_FRAMES = 5;
    constexpr float MINION_SHAMAN_DEAD_FRAME_DURATION = 0.15f;
    constexpr int MINION_SHAMAN_DEAD_FRAME_WIDTH = 96;
    constexpr int MINION_SHAMAN_DEAD_FRAME_HEIGHT = 96;

    constexpr int MINION_SHAMAN_HURT_FRAMES = 2;
    constexpr float MINION_SHAMAN_HURT_FRAME_DURATION = 0.10f;
    constexpr int MINION_SHAMAN_HURT_FRAME_WIDTH = 96;
    constexpr int MINION_SHAMAN_HURT_FRAME_HEIGHT = 96;


    // ===================== MINION WARRIOR =====================
    constexpr int MINION_WARRIOR_IDLE_FRAMES = 5;
    constexpr float MINION_WARRIOR_IDLE_FRAME_DURATION = 0.15f;
    constexpr int MINION_WARRIOR_IDLE_FRAME_WIDTH = 96;
    constexpr int MINION_WARRIOR_IDLE_FRAME_HEIGHT = 96;

    constexpr int MINION_WARRIOR_WALK_FRAMES = 7;
    constexpr float MINION_WARRIOR_WALK_FRAME_DURATION = 0.10f;
    constexpr int MINION_WARRIOR_WALK_FRAME_WIDTH = 96;
    constexpr int MINION_WARRIOR_WALK_FRAME_HEIGHT = 96;

    constexpr int MINION_WARRIOR_RUN_FRAMES = 6;
    constexpr float MINION_WARRIOR_RUN_FRAME_DURATION = 0.07f;
    constexpr int MINION_WARRIOR_RUN_FRAME_WIDTH = 96;
    constexpr int MINION_WARRIOR_RUN_FRAME_HEIGHT = 96;

    constexpr int MINION_WARRIOR_JUMP_FRAMES = 8;
    constexpr float MINION_WARRIOR_JUMP_FRAME_DURATION = 0.12f;
    constexpr int MINION_WARRIOR_JUMP_FRAME_WIDTH = 96;
    constexpr int MINION_WARRIOR_JUMP_FRAME_HEIGHT = 96;

    constexpr int MINION_WARRIOR_ATTACK_FRAMES = 11;
    constexpr float MINION_WARRIOR_ATTACK_FRAME_DURATION = 0.08f;
    constexpr int MINION_WARRIOR_ATTACK_FRAME_WIDTH = 96;
    constexpr int MINION_WARRIOR_ATTACK_FRAME_HEIGHT = 96;

    constexpr int MINION_WARRIOR_DEAD_FRAMES = 4;
    constexpr float MINION_WARRIOR_DEAD_FRAME_DURATION = 0.15f;
    constexpr int MINION_WARRIOR_DEAD_FRAME_WIDTH = 96;
    constexpr int MINION_WARRIOR_DEAD_FRAME_HEIGHT = 96;

    constexpr int MINION_WARRIOR_HURT_FRAMES = 2;
    constexpr float MINION_WARRIOR_HURT_FRAME_DURATION = 0.10f;
    constexpr int MINION_WARRIOR_HURT_FRAME_WIDTH = 96;
    constexpr int MINION_WARRIOR_HURT_FRAME_HEIGHT = 96;


}