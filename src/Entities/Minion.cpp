#include "Minion.h"
#include <iostream>

Minions::Minions(SDL_Renderer* renderer, glm::vec2 startPos, MinionType type)
    : Enemy(),
    minionType(type),
    magicTex(nullptr),
    isCastingMagic(false),
    magicCooldown(2.0f),
    magicTimer(0.0f),
    isRaging(false),
    rageThreshold(0.3f)
{
    this->renderer = renderer;
    this->position = startPos;
    // Lưu vị trí ban đầu để ResetToStartPosition có thể trả về đúng vị trí
    this->initialPosition = startPos;
    this->initialPatrolPointA = startPos - glm::vec2(100.0f, 0.0f);
    this->initialPatrolPointB = startPos + glm::vec2(100.0f, 0.0f);
    this->enemyType = EnemyType::MINION;

    std::string folderPath;

    // Thiết lập theo loại
    switch (type) {
    case MinionType::ORC_BERSERK:
        folderPath = "assets/images/Minions/Orc_Berserk/";
        maxHealth = 40;
        health = 40;
        attackDamage = 15;
        attackCooldown = 0.8f;
        runSpeed = 120.0f;
        detectionRange = 180.0f;
        break;

    case MinionType::ORC_SHAMAN:
        folderPath = "assets/images/Minions/Orc_Shaman/";
        maxHealth = 35;
        health = 35;
        attackDamage = 12;
        attackCooldown = 1.5f;
        attackRange = 150.0f;
        runSpeed = 80.0f;
        detectionRange = 200.0f;
        break;

    case MinionType::ORC_WARRIOR:
        folderPath = "assets/images/Minions/Orc_Warrior/";
        maxHealth = 50;
        health = 50;
        attackDamage = 12;
        attackCooldown = 1.2f;
        runSpeed = 90.0f;
        detectionRange = 160.0f;
        break;
    }

    // Load textures
    LoadTexture(renderer, &idleTex, (folderPath + "Idle.png").c_str());
    LoadTexture(renderer, &walkTex, (folderPath + "Walk.png").c_str());
    LoadTexture(renderer, &runTex, (folderPath + "Run.png").c_str());
    LoadTexture(renderer, &jumpTex, (folderPath + "Jump.png").c_str());
    LoadTexture(renderer, &attackTex, (folderPath + "Attack.png").c_str());
    LoadTexture(renderer, &hurtTex, (folderPath + "Hurt.png").c_str());
    LoadTexture(renderer, &deadTex, (folderPath + "Dead.png").c_str());

    if (type == MinionType::ORC_SHAMAN) {
        LoadTexture(renderer, &magicTex, (folderPath + "Magic.png").c_str());
    }

    // Size: 48x48 (bằng player)
    renderWidth = GameConstants::MINION_RENDER_WIDTH;
    renderHeight = GameConstants::MINION_RENDER_HEIGHT;
    hitboxWidth = GameConstants::MINION_HITBOX_WIDTH;
    hitboxHeight = GameConstants::MINION_HITBOX_HEIGHT;
    hitboxOffsetX = GameConstants::MINION_HITBOX_OFFSET_X;
    hitboxOffsetY = GameConstants::MINION_HITBOX_OFFSET_Y;
}

Minions::~Minions() {
    SDL_DestroyTexture(magicTex);
}

// === TRẢ FRAMES TỪ GAMECONSTANTS ===
FrameConfig Minions::GetFrameConfig(EnemyState state) const {
    FrameConfig cfg = { 0, 0.1f, 96, 96 };

    switch (minionType) {
    case MinionType::ORC_BERSERK:
        switch (state) {
        case EnemyState::STATE_IDLE:
            cfg = { GameConstants::MINION_BERSERK_IDLE_FRAMES, GameConstants::MINION_BERSERK_IDLE_FRAME_DURATION,
                   GameConstants::MINION_BERSERK_IDLE_FRAME_WIDTH, GameConstants::MINION_BERSERK_IDLE_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_WALK:
            cfg = { GameConstants::MINION_BERSERK_WALK_FRAMES, GameConstants::MINION_BERSERK_WALK_FRAME_DURATION,
                   GameConstants::MINION_BERSERK_WALK_FRAME_WIDTH, GameConstants::MINION_BERSERK_WALK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_RUN:
            cfg = { GameConstants::MINION_BERSERK_RUN_FRAMES, GameConstants::MINION_BERSERK_RUN_FRAME_DURATION,
                   GameConstants::MINION_BERSERK_RUN_FRAME_WIDTH, GameConstants::MINION_BERSERK_RUN_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_JUMP:
            cfg = { GameConstants::MINION_BERSERK_JUMP_FRAMES, GameConstants::MINION_BERSERK_JUMP_FRAME_DURATION,
                   GameConstants::MINION_BERSERK_JUMP_FRAME_WIDTH, GameConstants::MINION_BERSERK_JUMP_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_ATTACK:
            cfg = { GameConstants::MINION_BERSERK_ATTACK_FRAMES, GameConstants::MINION_BERSERK_ATTACK_FRAME_DURATION,
                   GameConstants::MINION_BERSERK_ATTACK_FRAME_WIDTH, GameConstants::MINION_BERSERK_ATTACK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_HURT:
            cfg = { GameConstants::MINION_BERSERK_HURT_FRAMES, GameConstants::MINION_BERSERK_HURT_FRAME_DURATION,
                   GameConstants::MINION_BERSERK_HURT_FRAME_WIDTH, GameConstants::MINION_BERSERK_HURT_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_DEAD:
            cfg = { GameConstants::MINION_BERSERK_DEAD_FRAMES, GameConstants::MINION_BERSERK_DEAD_FRAME_DURATION,
                   GameConstants::MINION_BERSERK_DEAD_FRAME_WIDTH, GameConstants::MINION_BERSERK_DEAD_FRAME_HEIGHT };
            break;
        }
        break;

    case MinionType::ORC_SHAMAN:
        switch (state) {
        case EnemyState::STATE_IDLE:
            cfg = { GameConstants::MINION_SHAMAN_IDLE_FRAMES, GameConstants::MINION_SHAMAN_IDLE_FRAME_DURATION,
                   GameConstants::MINION_SHAMAN_IDLE_FRAME_WIDTH, GameConstants::MINION_SHAMAN_IDLE_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_WALK:
            cfg = { GameConstants::MINION_SHAMAN_WALK_FRAMES, GameConstants::MINION_SHAMAN_WALK_FRAME_DURATION,
                   GameConstants::MINION_SHAMAN_WALK_FRAME_WIDTH, GameConstants::MINION_SHAMAN_WALK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_RUN:
            cfg = { GameConstants::MINION_SHAMAN_RUN_FRAMES, GameConstants::MINION_SHAMAN_RUN_FRAME_DURATION,
                   GameConstants::MINION_SHAMAN_RUN_FRAME_WIDTH, GameConstants::MINION_SHAMAN_RUN_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_JUMP:
            cfg = { GameConstants::MINION_SHAMAN_JUMP_FRAMES, GameConstants::MINION_SHAMAN_JUMP_FRAME_DURATION,
                   GameConstants::MINION_SHAMAN_JUMP_FRAME_WIDTH, GameConstants::MINION_SHAMAN_JUMP_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_ATTACK:
            cfg = { GameConstants::MINION_SHAMAN_ATTACK_FRAMES, GameConstants::MINION_SHAMAN_ATTACK_FRAME_DURATION,
                   GameConstants::MINION_SHAMAN_ATTACK_FRAME_WIDTH, GameConstants::MINION_SHAMAN_ATTACK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_HURT:
            cfg = { GameConstants::MINION_SHAMAN_HURT_FRAMES, GameConstants::MINION_SHAMAN_HURT_FRAME_DURATION,
                   GameConstants::MINION_SHAMAN_HURT_FRAME_WIDTH, GameConstants::MINION_SHAMAN_HURT_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_DEAD:
            cfg = { GameConstants::MINION_SHAMAN_DEAD_FRAMES, GameConstants::MINION_SHAMAN_DEAD_FRAME_DURATION,
                   GameConstants::MINION_SHAMAN_DEAD_FRAME_WIDTH, GameConstants::MINION_SHAMAN_DEAD_FRAME_HEIGHT };
            break;
        }
        break;

    case MinionType::ORC_WARRIOR:
        switch (state) {
        case EnemyState::STATE_IDLE:
            cfg = { GameConstants::MINION_WARRIOR_IDLE_FRAMES, GameConstants::MINION_WARRIOR_IDLE_FRAME_DURATION,
                   GameConstants::MINION_WARRIOR_IDLE_FRAME_WIDTH, GameConstants::MINION_WARRIOR_IDLE_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_WALK:
            cfg = { GameConstants::MINION_WARRIOR_WALK_FRAMES, GameConstants::MINION_WARRIOR_WALK_FRAME_DURATION,
                   GameConstants::MINION_WARRIOR_WALK_FRAME_WIDTH, GameConstants::MINION_WARRIOR_WALK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_RUN:
            cfg = { GameConstants::MINION_WARRIOR_RUN_FRAMES, GameConstants::MINION_WARRIOR_RUN_FRAME_DURATION,
                   GameConstants::MINION_WARRIOR_RUN_FRAME_WIDTH, GameConstants::MINION_WARRIOR_RUN_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_JUMP:
            cfg = { GameConstants::MINION_WARRIOR_JUMP_FRAMES, GameConstants::MINION_WARRIOR_JUMP_FRAME_DURATION,
                   GameConstants::MINION_WARRIOR_JUMP_FRAME_WIDTH, GameConstants::MINION_WARRIOR_JUMP_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_ATTACK:
            cfg = { GameConstants::MINION_WARRIOR_ATTACK_FRAMES, GameConstants::MINION_WARRIOR_ATTACK_FRAME_DURATION,
                   GameConstants::MINION_WARRIOR_ATTACK_FRAME_WIDTH, GameConstants::MINION_WARRIOR_ATTACK_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_HURT:
            cfg = { GameConstants::MINION_WARRIOR_HURT_FRAMES, GameConstants::MINION_WARRIOR_HURT_FRAME_DURATION,
                   GameConstants::MINION_WARRIOR_HURT_FRAME_WIDTH, GameConstants::MINION_WARRIOR_HURT_FRAME_HEIGHT };
            break;
        case EnemyState::STATE_DEAD:
            cfg = { GameConstants::MINION_WARRIOR_DEAD_FRAMES, GameConstants::MINION_WARRIOR_DEAD_FRAME_DURATION,
                   GameConstants::MINION_WARRIOR_DEAD_FRAME_WIDTH, GameConstants::MINION_WARRIOR_DEAD_FRAME_HEIGHT };
            break;
        }
        break;
    }

    return cfg;
}

void Minions::ActivateRage() {
    if (isRaging || minionType != MinionType::ORC_BERSERK) return;

    isRaging = true;
    attackCooldown *= 0.5f;
    runSpeed *= 1.3f;
    attackDamage = (int)(attackDamage * 1.2f);
}

void Minions::CastMagic() {
    if (magicTimer > 0 || minionType != MinionType::ORC_SHAMAN) return;

    isCastingMagic = true;
    magicTimer = magicCooldown;
}

int Minions::PerformAttack() {
    switch (minionType) {
    case MinionType::ORC_BERSERK:
        if (IsTargetInAttackRange()) {
            return attackDamage;
        }
        break;

    case MinionType::ORC_SHAMAN:
        CastMagic();
        return attackDamage;

    case MinionType::ORC_WARRIOR:
        if (IsTargetInAttackRange()) {
            return attackDamage;
        }
        break;
    }

    return 0;
}

void Minions::Update(float deltaTime, Map& map) {
    Enemy::Update(deltaTime, map);

    if (magicTimer > 0) {
        magicTimer -= deltaTime;
        if (magicTimer <= 0) {
            isCastingMagic = false;
        }
    }

    // Kích hoạt Rage khi máu thấp
    if (minionType == MinionType::ORC_BERSERK && !isRaging) {
        float healthPercent = (float)health / (float)maxHealth;
        if (healthPercent <= rageThreshold) {
            ActivateRage();
        }
    }
}

void Minions::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    Enemy::Render(renderer, cameraOffset);

    // // Aura đỏ khi Rage (Berserk)
    // if (minionType == MinionType::ORC_BERSERK && isRaging) {
    //     SDL_FRect outline = GetBoundingBox();
    //     outline.x -= cameraOffset.x - 2;
    //     outline.y -= cameraOffset.y - 2;
    //     outline.w += 4;
    //     outline.h += 4;

    //     // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
    //     // SDL_RenderRect(renderer, &outline);
    // }
}

void Minions::TakeDamage(int damage) {
    // Giảm damage khi Rage
    if (minionType == MinionType::ORC_BERSERK && isRaging) {
        damage = (int)(damage * 0.9f);
    }

    Enemy::TakeDamage(damage);
}