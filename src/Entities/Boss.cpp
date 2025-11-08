#include "Boss.h"
#include <iostream>
#include <cmath>

// ===== CONSTRUCTOR =====
Boss::Boss(SDL_Renderer* renderer, glm::vec2 startPos)
    : Enemy(renderer, startPos,
        "assets/images/Bosses/Idle_000.png",
        "assets/images/Bosses/Walk_000.png",
        "assets/images/Bosses/Run_000.png",
        "assets/images/Bosses/Jump_000.png",
        "assets/images/Bosses/Attack_000.png",
        "assets/images/Bosses/Hurt_000.png",
        "assets/images/Bosses/Dead_000.png",
        EnemyType::BOSS),
    currentPhase(BossPhase::PHASE_1),
    hasEnteredPhase2(false),
    hasEnteredPhase3(false),
    isUsingUltimate(false),
    ultimateCooldown(15.0f),
    ultimateTimer(0.0f),
    ultimateDamage(80),
    ultimateRadius(150.0f),
    canSummon(true),
    summonCount(0),
    maxSummons(6),
    summonCooldown(20.0f),
    summonTimer(0.0f),
    isCharging(false),
    chargeSpeed(300.0f),
    chargeDirection(0.0f, 0.0f),
    chargeDuration(1.5f),
    chargeTimer(0.0f),
    isSlamming(false),
    slamRadius(120.0f),
    slamDamage(50),
    hasIntroPlayed(false),
    isInvulnerable(true),
    introTimer(3.0f)
{
    maxHealth = 800;
    health = 800;
    attackDamage = 40;
    attackCooldown = 2.0f;
    attackRange = 80.0f;
    chaseSpeed = 80.0f;
    detectionRange = 400.0f;
}

// ===== DESTRUCTOR =====
Boss::~Boss() {
}

// ===== PHÁT INTRO =====
void Boss::PlayIntro() {
    std::cout << "=== BOSS XUAT HIEN ===\n";
    std::cout << "Rung man hinh!\n";
}

// ===== KÍCH HOẠT INTRO =====
void Boss::TriggerIntro() {
    if (!hasIntroPlayed) {
        PlayIntro();
        enemyState = EnemyState::STATE_IDLE;
    }
}

// ===== KIỂM TRA CHUYỂN PHASE =====
void Boss::CheckPhaseTransition() {
    float healthPercent = (float)health / (float)maxHealth;

    if (!hasEnteredPhase2 && healthPercent <= 0.7f) {
        EnterPhase2();
    }

    if (!hasEnteredPhase3 && healthPercent <= 0.4f) {
        EnterPhase3();
    }
}

// ===== VÀO PHASE 2 =====
void Boss::EnterPhase2() {
    hasEnteredPhase2 = true;
    currentPhase = BossPhase::PHASE_2;

    std::cout << "=== BOSS PHASE 2 ===\n";

    attackDamage = (int)(attackDamage * 1.3f);
    chaseSpeed *= 1.2f;
    attackCooldown *= 0.8f;

    if (canSummon && summonCount < maxSummons) {
        SummonMinions();
    }
}

// ===== VÀO PHASE 3 (RAGE) =====
void Boss::EnterPhase3() {
    hasEnteredPhase3 = true;
    currentPhase = BossPhase::PHASE_3;

    std::cout << "=== BOSS PHASE 3: RAGE MODE ===\n";

    attackDamage = (int)(attackDamage * 1.5f);
    chaseSpeed *= 1.4f;
    attackCooldown *= 0.6f;
    ultimateCooldown *= 0.7f;

    std::cout << "Rung man hinh manh!\n";

    if (canSummon) {
        SummonMinions();
    }
}

// ===== TRIỆU HỒI MINIONS =====
void Boss::SummonMinions() {
    if (summonCount >= maxSummons || summonTimer > 0) return;

    summonTimer = summonCooldown;
    summonCount += 2;

    std::cout << "Boss trieu hoi 2 Minions!\n";
}

// ===== LAO VỀ PHÍA TARGET =====
void Boss::ChargeAttack() {
    if (isCharging) return;

    isCharging = true;
    chargeTimer = chargeDuration;

    chargeDirection = glm::normalize(targetPosition - position);

    std::cout << "Boss lao!\n";
}

// ===== ĐẬP ĐẤT =====
void Boss::GroundSlam() {
    if (isSlamming) return;

    isSlamming = true;

    float distToTarget = GetDistanceToTarget();
    if (distToTarget <= slamRadius) {
        std::cout << "Boss dap dat! Damage: " << slamDamage << "\n";
    }

    isSlamming = false;
}

// ===== DÙNG ULTIMATE =====
void Boss::UseUltimate() {
    if (ultimateTimer > 0) return;

    isUsingUltimate = true;
    ultimateTimer = ultimateCooldown;

    std::cout << "=== BOSS ULTIMATE ===\n";

    float distToTarget = GetDistanceToTarget();
    if (distToTarget <= ultimateRadius) {
        std::cout << "Target nhan " << ultimateDamage << " damage!\n";
    }

    isUsingUltimate = false;
}

// ===== THỰC HIỆN TẤN CÔNG =====
int Boss::PerformAttack() {
    // Phase 3: Ultimate
    if (currentPhase == BossPhase::PHASE_3 && ultimateTimer <= 0) {
        if ((rand() % 100) < 40) {
            UseUltimate();
            return ultimateDamage;
        }
    }

    // Phase 2: Charge
    if (currentPhase == BossPhase::PHASE_2 && (rand() % 100) < 30) {
        ChargeAttack();
        return attackDamage;
    }

    // Tấn công thường
    if (IsTargetInAttackRange()) {
        if ((rand() % 100) < 50) {
            GroundSlam();
            return slamDamage;
        }
        else {
            std::cout << "Boss tan cong! Damage: " << attackDamage << "\n";
            return attackDamage;
        }
    }

    return 0;
}

// ===== UPDATE =====
void Boss::Update(float deltaTime, Map& map) {
    if (!hasIntroPlayed) {
        introTimer -= deltaTime;
        if (introTimer <= 0) {
            hasIntroPlayed = true;
            isInvulnerable = false;
            enemyState = EnemyState::STATE_IDLE;
        }
        return;
    }

    CheckPhaseTransition();

    if (isCharging) {
        chargeTimer -= deltaTime;
        velocity = chargeDirection * chargeSpeed;

        if (chargeTimer <= 0) {
            isCharging = false;
            velocity.x = 0.0f;
        }
    }

    if (ultimateTimer > 0) ultimateTimer -= deltaTime;
    if (summonTimer > 0) summonTimer -= deltaTime;

    Enemy::Update(deltaTime, map);
}

// ===== RENDER =====
void Boss::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    Enemy::Render(renderer, cameraOffset);

    // TODO: Hiệu ứng Phase 3, Charging, Ultimate
}

// ===== TAKE DAMAGE =====
void Boss::TakeDamage(int damage) {
    if (isInvulnerable) {
        std::cout << "Boss bat tu!\n";
        return;
    }

    if (currentPhase == BossPhase::PHASE_3) {
        damage = (int)(damage * 0.8f);
    }

    Enemy::TakeDamage(damage);

    std::cout << "Boss nhan " << damage << " damage! HP: " << health << "/" << maxHealth << "\n";
}