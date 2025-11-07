#include "Elite.h"
#include "Player.h"
#include <iostream>

// ===== CONSTRUCTOR =====
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
    // Load textures d?a theo lo?i Troll
    std::string folderPath;

    switch (type) {
    case TrollType::TROLL_1:
        folderPath = "assets/images/Elites/1_TROLL/";
        maxHealth = 120;
        health = 120;
        attackDamage = 20;
        specialDamage = 35;
        attackCooldown = 1.5f;
        chaseSpeed = 100.0f;
        detectionRange = 220.0f;
        break;

    case TrollType::TROLL_2:
        folderPath = "assets/images/Elites/2_TROLL/";
        maxHealth = 180;
        health = 180;
        attackDamage = 28;
        specialDamage = 45;
        attackCooldown = 1.3f;
        chaseSpeed = 110.0f;
        detectionRange = 250.0f;
        heavyAttackChargeTime = 0.8f;
        break;

    case TrollType::TROLL_3:
        folderPath = "assets/images/Elites/3_TROLL/";
        maxHealth = 250;
        health = 250;
        attackDamage = 35;
        specialDamage = 60;
        attackCooldown = 1.0f;
        chaseSpeed = 120.0f;
        detectionRange = 280.0f;
        specialCooldown = 4.0f;
        break;
    }

    // Load các texture
    LoadTexture(renderer, &idleTex, (folderPath + "Troll_01_1_IDLE_000.png").c_str());
    LoadTexture(renderer, &walkTex, (folderPath + "Troll_01_1_WALK_000.png").c_str());
    LoadTexture(renderer, &runTex, (folderPath + "Troll_01_1_RUN_000.png").c_str());
    LoadTexture(renderer, &jumpTex, (folderPath + "Troll_01_1_JUMP_000.png").c_str());
    LoadTexture(renderer, &attackTex, (folderPath + "Troll_01_1_ATTACK_000.png").c_str());
    LoadTexture(renderer, &hurtTex, (folderPath + "Troll_01_1_HURT_000.png").c_str());
    LoadTexture(renderer, &deadTex, (folderPath + "Troll_01_1_DIE_000.png").c_str());
}

// ===== DESTRUCTOR =====
Elites::~Elites() {
}

// ===== G?M LÊN (BUFF ??NG ??I) =====
void Elites::Roar() {
    if (hasRoared) return;

    hasRoared = true;
    buffTimer = buffDuration;

    // T?ng sát th??ng c?a b?n thân
    attackDamage = (int)(attackDamage * 1.3f);

    std::cout << "Elite Troll gam len! Buff toan bo dong doi!\n";

    // TODO: Tìm các Enemy khác trong ph?m vi và buff chúng
}

// ===== HEAVY ATTACK =====
void Elites::HeavyAttack() {
    if (!targetPlayer || !IsPlayerInAttackRange()) return;

    isHeavyAttack = true;
    heavyChargeTimer = heavyAttackChargeTime;

    std::cout << "Elite Troll dang nap heavy attack!\n";
}

// ===== CHIÊU ??C BI?T =====
void Elites::PerformSpecialAttack() {
    if (!targetPlayer || specialTimer > 0) return;

    isPerformingSpecial = true;
    specialTimer = specialCooldown;

    // T?n công ph?m vi r?ng
    float distToPlayer = GetDistanceToPlayer();
    if (distToPlayer <= attackRange * 1.5f) {
        targetPlayer->TakeDamage(specialDamage);
        std::cout << "Elite Troll dung chieu dac biet! Sat thuong: " << specialDamage << "\n";
    }

    // TODO: Thêm hi?u ?ng AOE
}

// ===== TH?C HI?N T?N CÔNG =====
void Elites::PerformAttack() {
    if (!targetPlayer) return;

    // 30% c? h?i dùng chiêu ??c bi?t
    if (specialTimer <= 0 && (rand() % 100) < 30) {
        PerformSpecialAttack();
        return;
    }

    // T?n công th??ng
    if (IsPlayerInAttackRange()) {
        // Troll 2 và 3 có c? h?i heavy attack
        if ((trollType == TrollType::TROLL_2 || trollType == TrollType::TROLL_3)
            && (rand() % 100) < 40) {
            HeavyAttack();
        }
        else {
            targetPlayer->TakeDamage(attackDamage);
            std::cout << "Elite Troll tan cong! Sat thuong: " << attackDamage << "\n";
        }
    }
}

// ===== UPDATE =====
void Elites::Update(float deltaTime, Map& map) {
    // G?i Enemy::Update
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
            // Th?c hi?n heavy attack
            if (targetPlayer && IsPlayerInAttackRange()) {
                int heavyDamage = attackDamage * 2;
                targetPlayer->TakeDamage(heavyDamage);
                std::cout << "Elite Troll heavy attack! Sat thuong: " << heavyDamage << "\n";
            }
            isHeavyAttack = false;
        }
    }

    // Gi?m buff timer
    if (buffTimer > 0) {
        buffTimer -= deltaTime;
    }

    // Kích ho?t Roar khi vào combat l?n ??u (ch? 1 l?n)
    if (!hasRoared && enemyState == EnemyState::STATE_CHASE) {
        float healthPercent = (float)health / (float)maxHealth;
        if (healthPercent > 0.7f) {
            Roar();
        }
    }
}

// ===== RENDER =====
void Elites::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // G?i Enemy::Render
    Enemy::Render(renderer, cameraOffset);

    // Hi?u ?ng charge (heavy attack)
    if (isHeavyAttack) {
        // TODO: V? hi?u ?ng charging (vòng tròn sáng lên)
    }

    // Hi?u ?ng buff active
    if (buffTimer > 0) {
        // TODO: V? aura buff (vi?n vàng/??)
    }
}

// ===== NH?N SÁT TH??NG =====
void Elites::TakeDamage(int damage) {
    // Gi?m sát th??ng n?u ?ang buff
    if (buffTimer > 0) {
        damage = (int)(damage * 0.7f);
    }

    Enemy::TakeDamage(damage);
}