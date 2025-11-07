#include "Minion.h"
#include "Player.h"
#include <iostream>

// ===== CONSTRUCTOR =====
Minions::Minions(SDL_Renderer* renderer, glm::vec2 startPos, MinionType type)
    : Enemy(renderer, startPos,
        nullptr, nullptr, nullptr, nullptr, // Sẽ set sau
        nullptr, nullptr, nullptr,
        EnemyType::MINION),
    minionType(type),
    magicTex1(nullptr),
    magicTex2(nullptr),
    isCastingMagic(false),
    magicCooldown(2.0f),
    magicTimer(0.0f),
    isRaging(false),
    rageThreshold(0.3f)
{
    // Load textures dựa theo loại Minion
    const char* folderPath = nullptr;

    switch (type) {
    case MinionType::ORC_BERSERK:
        folderPath = "assets/images/Minions/Orc_Berserk/";
        maxHealth = 40;
        health = 40;
        attackDamage = 15;
        attackCooldown = 0.8f;  // Tấn công nhanh
        chaseSpeed = 120.0f;
        detectionRange = 180.0f;
        break;

    case MinionType::ORC_SHAMAN:
        folderPath = "assets/images/Minions/Orc_Shaman/";
        maxHealth = 35;
        health = 35;
        attackDamage = 12;
        attackCooldown = 1.5f;
        attackRange = 150.0f;   // Tấn công từ xa
        chaseSpeed = 80.0f;
        detectionRange = 200.0f;
        break;

    case MinionType::ORC_WARRIOR:
        folderPath = "assets/images/Minions/Orc_Warrior/";
        maxHealth = 50;
        health = 50;
        attackDamage = 12;
        attackCooldown = 1.2f;
        chaseSpeed = 90.0f;
        detectionRange = 160.0f;
        break;
    }

    // Load các texture chung
    std::string folder(folderPath);
    LoadTexture(renderer, &idleTex, (folder + "Idle.png").c_str());
    LoadTexture(renderer, &walkTex, (folder + "Walk.png").c_str());
    LoadTexture(renderer, &runTex, (folder + "Run.png").c_str());
    LoadTexture(renderer, &jumpTex, (folder + "Jump.png").c_str());
    LoadTexture(renderer, &attackTex, (folder + "Attack_1.png").c_str());
    LoadTexture(renderer, &hurtTex, (folder + "Hurt.png").c_str());
    LoadTexture(renderer, &deadTex, (folder + "Dead.png").c_str());

    // Load texture đặc biệt cho Shaman
    if (type == MinionType::ORC_SHAMAN) {
        LoadTexture(renderer, &magicTex1, (folder + "Magic_1.png").c_str());
        LoadTexture(renderer, &magicTex2, (folder + "Magic_2.png").c_str());
    }
}

// ===== DESTRUCTOR =====
Minions::~Minions() {
    SDL_DestroyTexture(magicTex1);
    SDL_DestroyTexture(magicTex2);
}

// ===== KÍCH HOẠT RAGE (BERSERK) =====
void Minions::ActivateRage() {
    if (isRaging) return;

    isRaging = true;
    attackCooldown *= 0.5f;     // Tăng tốc độ tấn công
    chaseSpeed *= 1.3f;         // Tăng tốc độ di chuyển
    attackDamage = (int)(attackDamage * 1.2f); // Tăng sát thương

    std::cout << "Orc Berserk da cuong no!\n";
}

// ===== THI PHÁP THUẬT (SHAMAN) =====
void Minions::CastMagic() {
    if (!targetPlayer || magicTimer > 0) return;

    isCastingMagic = true;
    magicTimer = magicCooldown;

    // TODO: Tạo projectile magic bay về phía Player
    std::cout << "Orc Shaman thi phap thuat!\n";
}

// ===== THỰC HIỆN TẤN CÔNG =====
void Minions::PerformAttack() {
    if (!targetPlayer) return;

    switch (minionType) {
    case MinionType::ORC_BERSERK:
    case MinionType::ORC_WARRIOR:
        // Tấn công cận chiến - kiểm tra khoảng cách
        if (IsPlayerInAttackRange()) {
            targetPlayer->TakeDamage(attackDamage);
            std::cout << "Minion tan cong Player!\n";
        }
        break;

    case MinionType::ORC_SHAMAN:
        // Tấn công từ xa - thi pháp thuật
        CastMagic();
        break;
    }
}

// ===== UPDATE =====
void Minions::Update(float deltaTime, Map& map) {
    // Gọi Enemy::Update để xử lý AI cơ bản
    Enemy::Update(deltaTime, map);

    // Xử lý magic timer (Shaman)
    if (magicTimer > 0) {
        magicTimer -= deltaTime;
        if (magicTimer <= 0) {
            isCastingMagic = false;
        }
    }

    // Kích hoạt Rage khi máu thấp (Berserk)
    if (minionType == MinionType::ORC_BERSERK && !isRaging) {
        float healthPercent = (float)health / (float)maxHealth;
        if (healthPercent <= rageThreshold) {
            ActivateRage();
        }
    }
}

// ===== RENDER =====
void Minions::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // Shaman đang casting magic - vẽ magic texture
    if (minionType == MinionType::ORC_SHAMAN && isCastingMagic && magicTex1) {
        // TODO: Render magic effect
    }

    // Gọi Enemy::Render để vẽ sprite chính
    Enemy::Render(renderer, cameraOffset);

    // Hiệu ứng Rage (Berserk) - vẽ viền đỏ
    if (minionType == MinionType::ORC_BERSERK && isRaging) {
        // TODO: Vẽ hiệu ứng rage (viền đỏ, aura)
    }
}

// ===== NHẬN SÁT THƯƠNG =====
void Minions::TakeDamage(int damage) {
    // Gọi Enemy::TakeDamage
    Enemy::TakeDamage(damage);

    // Logic đặc biệt khi bị đánh (có thể thêm sau)
}