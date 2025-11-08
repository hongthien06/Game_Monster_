#include "Elite.h"
#include <iostream>

// ===== CONSTRUCTOR - PHÂN BI?T 3 LO?I TROLL =====
Elites::Elites(SDL_Renderer* renderer, glm::vec2 startPos, TrollType type)
    : Enemy(renderer, startPos,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr,
        EnemyType::ELITE),
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
    // ===== THI?T L?P THÔNG S? CHO T?NG LO?I TROLL =====
    std::string folderPath;
    std::string prefix;

    switch (type) {
    case TrollType::TROLL_1:
        // Troll 1 - C? b?n
        folderPath = "assets/images/Elites/1_TROLL/";
        prefix = "Troll_01_1_";
        maxHealth = 120;
        health = 120;
        attackDamage = 20;
        specialDamage = 35;
        attackCooldown = 1.5f;
        chaseSpeed = 100.0f;
        detectionRange = 220.0f;
        std::cout << "[Troll 1] Khoi tao - HP: 120, Damage: 20\n";
        break;

    case TrollType::TROLL_2:
        // Troll 2 - Trung bình
        folderPath = "assets/images/Elites/2_TROLL/";
        prefix = "Troll_02_1_";
        maxHealth = 180;
        health = 180;
        attackDamage = 28;
        specialDamage = 45;
        attackCooldown = 1.3f;
        chaseSpeed = 110.0f;
        detectionRange = 250.0f;
        heavyAttackChargeTime = 0.8f;
        std::cout << "[Troll 2] Khoi tao - HP: 180, Damage: 28\n";
        break;

    case TrollType::TROLL_3:
        // Troll 3 - M?nh nh?t
        folderPath = "assets/images/Elites/3_TROLL/";
        prefix = "Troll_03_1_";
        maxHealth = 250;
        health = 250;
        attackDamage = 35;
        specialDamage = 60;
        attackCooldown = 1.0f;
        chaseSpeed = 120.0f;
        detectionRange = 280.0f;
        specialCooldown = 4.0f;
        std::cout << "[Troll 3] Khoi tao - HP: 250, Damage: 35\n";
        break;

    default:
        std::cerr << "[Elite] Loai Troll khong hop le!\n";
        return;
    }

    // ===== LOAD TEXTURE ?ÚNG CHO T?NG LO?I =====
    std::cout << "Loading textures tu: " << folderPath << prefix << "...\n";

    LoadTexture(renderer, &idleTex, (folderPath + prefix + "IDLE_000.png").c_str());
    LoadTexture(renderer, &walkTex, (folderPath + prefix + "WALK_000.png").c_str());
    LoadTexture(renderer, &runTex, (folderPath + prefix + "RUN_000.png").c_str());
    LoadTexture(renderer, &jumpTex, (folderPath + prefix + "JUMP_000.png").c_str());
    LoadTexture(renderer, &attackTex, (folderPath + prefix + "ATTACK_000.png").c_str());
    LoadTexture(renderer, &hurtTex, (folderPath + prefix + "HURT_000.png").c_str());
    LoadTexture(renderer, &deadTex, (folderPath + prefix + "DIE_000.png").c_str());
}

// ===== DESTRUCTOR =====
Elites::~Elites() {
}

// ===== G?M LÊN (BUFF) =====
void Elites::Roar() {
    if (hasRoared) return;

    hasRoared = true;
    buffTimer = buffDuration;
    attackDamage = (int)(attackDamage * 1.3f);

    std::cout << "[Elite Troll] Gam len! Buff toan doi!\n";
}

// ===== HEAVY ATTACK =====
void Elites::HeavyAttack() {
    if (!IsTargetInAttackRange()) return;

    isHeavyAttack = true;
    heavyChargeTimer = heavyAttackChargeTime;

    std::cout << "[Elite Troll] Dang nap Heavy Attack!\n";
}

// ===== CHIÊU ??C BI?T =====
void Elites::PerformSpecialAttack() {
    if (specialTimer > 0) return;

    isPerformingSpecial = true;
    specialTimer = specialCooldown;

    float distToTarget = GetDistanceToTarget();
    if (distToTarget <= attackRange * 1.5f) {
        std::cout << "[Elite Troll] CHIEU DAC BIET! Damage: " << specialDamage << "\n";
    }
}

// ===== TH?C HI?N T?N CÔNG =====
int Elites::PerformAttack() {
    // 30% c? h?i dùng chiêu ??c bi?t
    if (specialTimer <= 0 && (rand() % 100) < 30) {
        PerformSpecialAttack();
        return specialDamage;
    }

    // Heavy attack (ch? Troll 2 và 3)
    if ((trollType == TrollType::TROLL_2 || trollType == TrollType::TROLL_3)
        && (rand() % 100) < 40) {
        HeavyAttack();
        return attackDamage * 2;
    }

    // T?n công th??ng
    if (IsTargetInAttackRange()) {
        std::cout << "[Elite Troll] Tan cong! Damage: " << attackDamage << "\n";
        return attackDamage;
    }

    return 0;
}

// ===== UPDATE =====
void Elites::Update(float deltaTime, Map& map) {
    Enemy::Update(deltaTime, map);

    // Gi?m special timer
    if (specialTimer > 0) {
        specialTimer -= deltaTime;
        if (specialTimer <= 0) {
            isPerformingSpecial = false;
        }
    }

    // X? lý heavy attack charge
    if (isHeavyAttack) {
        heavyChargeTimer -= deltaTime;
        if (heavyChargeTimer <= 0) {
            isHeavyAttack = false;
        }
    }

    // Gi?m buff timer
    if (buffTimer > 0) {
        buffTimer -= deltaTime;
    }

    // Kích ho?t Roar khi vào combat l?n ??u
    if (!hasRoared && enemyState == EnemyState::STATE_CHASE) {
        float healthPercent = (float)health / (float)maxHealth;
        if (healthPercent > 0.7f) {
            Roar();
        }
    }
}

// ===== RENDER =====
void Elites::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    Enemy::Render(renderer, cameraOffset);

    // TODO: Hi?u ?ng charging, buff
}

// ===== TAKE DAMAGE =====
void Elites::TakeDamage(int damage) {
    // Gi?m damage khi ?ang buff
    if (buffTimer > 0) {
        damage = (int)(damage * 0.7f);
        std::cout << "[Elite Troll] Giam sat thuong nho buff! Damage: " << damage << "\n";
    }

    Enemy::TakeDamage(damage);
}