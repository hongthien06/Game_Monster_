#include "Elite.h"
#include <iostream>

Elites::Elites(SDL_Renderer* renderer, glm::vec2 startPos, TrollType type)
    : Enemy(),
    trollType(type),
    isPerformingSpecial(false),
    specialCooldown(5.0f),
    specialTimer(0.0f),
    specialDamage(0),
    isHeavyAttack(false),
    heavyAttackChargeTime(1.0f),
    heavyChargeTimer(0.0f),
    hasRoared(false),
    roarRadius(200.0f),
    buffDuration(10.0f),
    buffTimer(0.0f)
{
    this->renderer = renderer;
    this->position = startPos;
    // Lưu vị trí ban đầu để ResetToStartPosition có thể trả về đúng vị trí
    this->initialPosition = startPos;
    this->initialPatrolPointA = startPos - glm::vec2(100.0f, 0.0f);
    this->initialPatrolPointB = startPos + glm::vec2(100.0f, 0.0f);
    this->enemyType = EnemyType::ELITE;

    std::string folderPath;

    switch (type) {
    case TrollType::TROLL_1:
        folderPath = "assets/images/Elites/1_TROLL/";
        maxHealth = 120;
        health = 120;
        attackDamage = 20;
        specialDamage = 35;
        attackCooldown = 1.5f;
        runSpeed = 100.0f;
        aggroRange = 250.0f;
        attackRange = 50.0f;
        break;

    case TrollType::TROLL_2:
        folderPath = "assets/images/Elites/2_TROLL/";
        maxHealth = 180;
        health = 180;
        attackDamage = 28;
        specialDamage = 45;
        attackCooldown = 1.3f;
        runSpeed = 110.0f;
        aggroRange = 280.0f;
        attackRange = 55.0f;
        heavyAttackChargeTime = 0.8f;
        break;

    case TrollType::TROLL_3:
        folderPath = "assets/images/Elites/3_TROLL/";
        maxHealth = 250;
        health = 250;
        attackDamage = 35;
        specialDamage = 60;
        attackCooldown = 1.0f;
        runSpeed = 120.0f;
        aggroRange = 300.0f;
        attackRange = 60.0f;
        specialCooldown = 4.0f;
        break;
    }

    // Load textures
    LoadTexture(renderer, &idleTex, (folderPath + "Idle.png").c_str());
    LoadTexture(renderer, &walkTex, (folderPath + "Walk.png").c_str());
    LoadTexture(renderer, &runTex, (folderPath + "Run.png").c_str());
    LoadTexture(renderer, &jumpTex, (folderPath + "Jump.png").c_str());
    LoadTexture(renderer, &attackTex, (folderPath + "Attack.png").c_str());
    LoadTexture(renderer, &hurtTex, (folderPath + "Hurt.png").c_str());
    LoadTexture(renderer, &deadTex, (folderPath + "Die.png").c_str());

    // Size: 64x64 (l?n h?n Minion)
    renderWidth = GameConstants::ELITE_RENDER_WIDTH;
    renderHeight = GameConstants::ELITE_RENDER_HEIGHT;
    hitboxWidth = GameConstants::ELITE_HITBOX_WIDTH;
    hitboxHeight = GameConstants::ELITE_HITBOX_HEIGHT;
    hitboxOffsetX = GameConstants::ELITE_HITBOX_OFFSET_X;
    hitboxOffsetY = GameConstants::ELITE_HITBOX_OFFSET_Y;
}

Elites::~Elites() {
}

// === TR? FRAMES T? GAMECONSTANTS ===
FrameConfig Elites::GetFrameConfig(EnemyState state) const {
    FrameConfig cfg = { 0, 0.1f, 1600, 1000 };

    switch (trollType) {
    case TrollType::TROLL_1:
        switch (state) {
        case EnemyState::STATE_IDLE:
            cfg = { GameConstants::ELITE_TROLL_1_IDLE_FRAMES, GameConstants::ELITE_TROLL_1_IDLE_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_1_IDLE_FRAME_WIDTH, GameConstants::ELITE_TROLL_1_IDLE_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_WALK:
            cfg = { GameConstants::ELITE_TROLL_1_WALK_FRAMES, GameConstants::ELITE_TROLL_1_WALK_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_1_WALK_FRAME_WIDTH, GameConstants::ELITE_TROLL_1_WALK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_RUN:
            cfg = { GameConstants::ELITE_TROLL_1_RUN_FRAMES, GameConstants::ELITE_TROLL_1_RUN_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_1_RUN_FRAME_WIDTH, GameConstants::ELITE_TROLL_1_RUN_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_JUMP:
            cfg = { GameConstants::ELITE_TROLL_1_JUMP_FRAMES, GameConstants::ELITE_TROLL_1_JUMP_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_1_JUMP_FRAME_WIDTH, GameConstants::ELITE_TROLL_1_JUMP_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_ATTACK:
            cfg = { GameConstants::ELITE_TROLL_1_ATTACK_FRAMES, GameConstants::ELITE_TROLL_1_ATTACK_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_1_ATTACK_FRAME_WIDTH, GameConstants::ELITE_TROLL_1_ATTACK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_HURT:
            cfg = { GameConstants::ELITE_TROLL_1_HURT_FRAMES, GameConstants::ELITE_TROLL_1_HURT_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_1_HURT_FRAME_WIDTH, GameConstants::ELITE_TROLL_1_HURT_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_DEAD:
            cfg = { GameConstants::ELITE_TROLL_1_DEAD_FRAMES, GameConstants::ELITE_TROLL_1_DEAD_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_1_DEAD_FRAME_WIDTH, GameConstants::ELITE_TROLL_1_DEAD_FRAME_HEIGHT };
            break;
        }
        break;

    case TrollType::TROLL_2:
        switch (state) {
        case EnemyState::STATE_IDLE:
            cfg = { GameConstants::ELITE_TROLL_2_IDLE_FRAMES, GameConstants::ELITE_TROLL_2_IDLE_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_2_IDLE_FRAME_WIDTH, GameConstants::ELITE_TROLL_2_IDLE_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_WALK:
            cfg = { GameConstants::ELITE_TROLL_2_WALK_FRAMES, GameConstants::ELITE_TROLL_2_WALK_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_2_WALK_FRAME_WIDTH, GameConstants::ELITE_TROLL_2_WALK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_RUN:
            cfg = { GameConstants::ELITE_TROLL_2_RUN_FRAMES, GameConstants::ELITE_TROLL_2_RUN_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_2_RUN_FRAME_WIDTH, GameConstants::ELITE_TROLL_2_RUN_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_JUMP:
            cfg = { GameConstants::ELITE_TROLL_2_JUMP_FRAMES, GameConstants::ELITE_TROLL_2_JUMP_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_2_JUMP_FRAME_WIDTH, GameConstants::ELITE_TROLL_2_JUMP_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_ATTACK:
            cfg = { GameConstants::ELITE_TROLL_2_ATTACK_FRAMES, GameConstants::ELITE_TROLL_2_ATTACK_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_2_ATTACK_FRAME_WIDTH, GameConstants::ELITE_TROLL_2_ATTACK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_HURT:
            cfg = { GameConstants::ELITE_TROLL_2_HURT_FRAMES, GameConstants::ELITE_TROLL_2_HURT_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_2_HURT_FRAME_WIDTH, GameConstants::ELITE_TROLL_2_HURT_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_DEAD:
            cfg = { GameConstants::ELITE_TROLL_2_DEAD_FRAMES, GameConstants::ELITE_TROLL_2_DEAD_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_2_DEAD_FRAME_WIDTH, GameConstants::ELITE_TROLL_2_DEAD_FRAME_HEIGHT };
            break;
        }
        break;

    case TrollType::TROLL_3:
        switch (state) {
        case EnemyState::STATE_IDLE:
            cfg = { GameConstants::ELITE_TROLL_3_IDLE_FRAMES, GameConstants::ELITE_TROLL_3_IDLE_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_3_IDLE_FRAME_WIDTH, GameConstants::ELITE_TROLL_3_IDLE_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_WALK:
            cfg = { GameConstants::ELITE_TROLL_3_WALK_FRAMES, GameConstants::ELITE_TROLL_3_WALK_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_3_WALK_FRAME_WIDTH, GameConstants::ELITE_TROLL_3_WALK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_RUN:
            cfg = { GameConstants::ELITE_TROLL_3_RUN_FRAMES, GameConstants::ELITE_TROLL_3_RUN_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_3_RUN_FRAME_WIDTH, GameConstants::ELITE_TROLL_3_RUN_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_JUMP:
            cfg = { GameConstants::ELITE_TROLL_3_JUMP_FRAMES, GameConstants::ELITE_TROLL_3_JUMP_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_3_JUMP_FRAME_WIDTH, GameConstants::ELITE_TROLL_3_JUMP_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_ATTACK:
            cfg = { GameConstants::ELITE_TROLL_3_ATTACK_FRAMES, GameConstants::ELITE_TROLL_3_ATTACK_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_3_ATTACK_FRAME_WIDTH, GameConstants::ELITE_TROLL_3_ATTACK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_HURT:
            cfg = { GameConstants::ELITE_TROLL_3_HURT_FRAMES, GameConstants::ELITE_TROLL_3_HURT_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_3_HURT_FRAME_WIDTH, GameConstants::ELITE_TROLL_3_HURT_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_DEAD:
            cfg = { GameConstants::ELITE_TROLL_3_DEAD_FRAMES, GameConstants::ELITE_TROLL_3_DEAD_FRAME_DURATION,
                   GameConstants::ELITE_TROLL_3_DEAD_FRAME_WIDTH, GameConstants::ELITE_TROLL_3_DEAD_FRAME_HEIGHT };
            break;
        }
        break;
    }

    return cfg;
}

void Elites::Roar() {
    if (hasRoared) return;

    hasRoared = true;
    buffTimer = buffDuration;
    attackDamage = (int)(attackDamage * 1.3f);
}

void Elites::HeavyAttack() {
    if (!IsTargetInAttackRange()) return;

    isHeavyAttack = true;
    heavyChargeTimer = heavyAttackChargeTime;
}

void Elites::PerformSpecialAttack() {
    if (specialTimer > 0) return;

    isPerformingSpecial = true;
    specialTimer = specialCooldown;

    float distToTarget = GetDistanceToTarget();
    if (distToTarget <= attackRange * 1.5f) {
        std::cout << "[Elite Troll] CHIEU DAC BIET! Damage: " << specialDamage << "\n";
    }
}

int Elites::PerformAttack() {
    if (specialTimer <= 0 && (rand() % 100) < 30) {
        PerformSpecialAttack();
        return specialDamage;
    }

    if ((trollType == TrollType::TROLL_2 || trollType == TrollType::TROLL_3)
        && (rand() % 100) < 40) {
        HeavyAttack();
        return attackDamage * 2;
    }

    if (IsTargetInAttackRange()) {
        return attackDamage;
    }

    return 0;
}

void Elites::Update(float deltaTime, Map& map) {
    Enemy::Update(deltaTime, map);

    if (specialTimer > 0) {
        specialTimer -= deltaTime;
        if (specialTimer <= 0) {
            isPerformingSpecial = false;
        }
    }

    if (isHeavyAttack) {
        heavyChargeTimer -= deltaTime;
        if (heavyChargeTimer <= 0) {
            isHeavyAttack = false;
        }
    }

    if (buffTimer > 0) {
        buffTimer -= deltaTime;
    }

    if (!hasRoared && enemyState == EnemyState::STATE_RUN) {
        float healthPercent = (float)health / (float)maxHealth;
        if (healthPercent > 0.7f) {
            Roar();
        }
    }
}

void Elites::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    Enemy::Render(renderer, cameraOffset);
}

void Elites::TakeDamage(int damage) {
    if (buffTimer > 0) {
        damage = (int)(damage * 0.7f);
    }

    Enemy::TakeDamage(damage);
}