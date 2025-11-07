#include "Boss.h"
#include "Player.h"
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
    // Thông số Boss
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
    std::cout << "=== BOSS XUẤT HIỆN ===\n";
    std::cout << "Rung man hinh!\n";
    // TODO: Trigger camera shake effect
    // TODO: Play boss music
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

    // Chuyển sang Phase 2 (70% HP)
    if (!hasEnteredPhase2 && healthPercent <= 0.7f) {
        EnterPhase2();
    }

    // Chuyển sang Phase 3 (40% HP)
    if (!hasEnteredPhase3 && healthPercent <= 0.4f) {
        EnterPhase3();
    }
}

// ===== VÀO PHASE 2 =====
void Boss::EnterPhase2() {
    hasEnteredPhase2 = true;
    currentPhase = BossPhase::PHASE_2;

    std::cout << "=== BOSS PHASE 2 ===\n";

    // Tăng sức mạnh
    attackDamage = (int)(attackDamage * 1.3f);
    chaseSpeed *= 1.2f;
    attackCooldown *= 0.8f;

    // Triệu hồi minions
    if (canSummon && summonCount < maxSummons) {
        SummonMinions();
    }

    // TODO: Hiệu ứng chuyển phase (flash, particles)
}

// ===== VÀO PHASE 3 (RAGE MODE) =====
void Boss::EnterPhase3() {
    hasEnteredPhase3 = true;
    currentPhase = BossPhase::PHASE_3;

    std::cout << "=== BOSS PHASE 3: RAGE MODE ===\n";

    // Tăng sức mạnh mạnh mẽ
    attackDamage = (int)(attackDamage * 1.5f);
    chaseSpeed *= 1.4f;
    attackCooldown *= 0.6f;
    ultimateCooldown *= 0.7f;

    // Rung màn hình
    std::cout << "Rung man hinh manh!\n";
    // TODO: Strong camera shake

    // Triệu hồi thêm minions
    if (canSummon) {
        SummonMinions();
    }
}

// ===== TRIỆU HỒI MINIONS =====
void Boss::SummonMinions() {
    if (summonCount >= maxSummons || summonTimer > 0) return;

    summonTimer = summonCooldown;
    summonCount += 2; // Triệu hồi 2 minions mỗi lần

    std::cout << "Boss trieu hoi 2 Minions! Tong so: " << summonCount << "\n";

    // TODO: Spawn 2 minions ở 2 bên Boss
    // glm::vec2 leftSpawn = position + glm::vec2(-80.0f, 0.0f);
    // glm::vec2 rightSpawn = position + glm::vec2(80.0f, 0.0f);
}

// ===== LAO VỀ PHÍA PLAYER =====
void Boss::ChargeAttack() {
    if (!targetPlayer || isCharging) return;

    isCharging = true;
    chargeTimer = chargeDuration;

    // Tính hướng lao
    glm::vec2 playerPos = targetPlayer->GetPosition();
    chargeDirection = glm::normalize(playerPos - position);

    std::cout << "Boss lao ve phia Player!\n";
}

// ===== ĐẬP ĐẤT =====
void Boss::GroundSlam() {
    if (!targetPlayer || isSlamming) return;

    isSlamming = true;

    // Kiểm tra Player có trong phạm vi không
    float distToPlayer = GetDistanceToPlayer();
    if (distToPlayer <= slamRadius) {
        targetPlayer->TakeDamage(slamDamage);
        std::cout << "Boss dap dat! Player nhan " << slamDamage << " sat thuong!\n";
    }

    // TODO: Tạo hiệu ứng sóng xung kích

    isSlamming = false;
}

// ===== DÙNG CHIÊU CUỐI =====
void Boss::UseUltimate() {
    if (!targetPlayer || ultimateTimer > 0) return;

    isUsingUltimate = true;
    ultimateTimer = ultimateCooldown;

    std::cout << "=== BOSS DUNG CHIEU CUOI ===\n";

    // AOE damage
    float distToPlayer = GetDistanceToPlayer();
    if (distToPlayer <= ultimateRadius) {
        targetPlayer->TakeDamage(ultimateDamage);
        std::cout << "Player nhan " << ultimateDamage << " sat thuong tu ultimate!\n";
    }

    // TODO: Hiệu ứng nổ lớn, sóng xung kích
    // TODO: Rung màn hình

    isUsingUltimate = false;
}

// ===== THỰC HIỆN TẤN CÔNG =====
void Boss::PerformAttack() {
    if (!targetPlayer) return;

    // Phase 3: Dùng ultimate thường xuyên hơn
    if (currentPhase == BossPhase::PHASE_3 && ultimateTimer <= 0) {
        if ((rand() % 100) < 40) {
            UseUltimate();
            return;
        }
    }

    // Phase 2: Charge attack
    if (currentPhase == BossPhase::PHASE_2 && (rand() % 100) < 30) {
        ChargeAttack();
        return;
    }

    // Tấn công thường
    if (IsPlayerInAttackRange()) {
        // Ground slam hoặc tấn công thường
        if ((rand() % 100) < 50) {
            GroundSlam();
        }
        else {
            targetPlayer->TakeDamage(attackDamage);
            std::cout << "Boss tan cong! Sat thuong: " << attackDamage << "\n";
        }
    }
}

// ===== UPDATE =====
void Boss::Update(float deltaTime, Map& map) {
    // Xử lý intro
    if (!hasIntroPlayed) {
        introTimer -= deltaTime;
        if (introTimer <= 0) {
            hasIntroPlayed = true;
            isInvulnerable = false;
            enemyState = EnemyState::STATE_IDLE;
        }
        return;
    }

    // Kiểm tra chuyển phase
    CheckPhaseTransition();

    // Xử lý charge attack
    if (isCharging) {
        chargeTimer -= deltaTime;

        // Di chuyển theo hướng charge
        velocity = chargeDirection * chargeSpeed;

        if (chargeTimer <= 0) {
            isCharging = false;
            velocity.x = 0.0f;
        }
    }

    // Giảm các timer
    if (ultimateTimer > 0) ultimateTimer -= deltaTime;
    if (summonTimer > 0) summonTimer -= deltaTime;

    // Gọi Enemy::Update
    Enemy::Update(deltaTime, map);
}

// ===== RENDER =====
void Boss::Render(SDL_Renderer* renderer, glm::vec2 cameraOffset) {
    // Gọi Enemy::Render
    Enemy::Render(renderer, cameraOffset);

    // Hiệu ứng phase 3 (aura đỏ)
    if (currentPhase == BossPhase::PHASE_3) {
        // TODO: Vẽ aura rage (đỏ, particles)
    }

    // Hiệu ứng charging
    if (isCharging) {
        // TODO: Vẽ trail khi lao
    }

    // Vẽ health bar lớn hơn cho Boss
    // TODO: Custom Boss health bar ở góc màn hình
}

// ===== NHẬN SÁT THƯƠNG =====
void Boss::TakeDamage(int damage) {
    // Bất tử trong intro
    if (isInvulnerable) {
        std::cout << "Boss dang bat tu!\n";
        return;
    }

    // Phase 3: Giảm sát thương nhận vào
    if (currentPhase == BossPhase::PHASE_3) {
        damage = (int)(damage * 0.8f);
    }

    Enemy::TakeDamage(damage);

    std::cout << "Boss nhan " << damage << " sat thuong! HP: " << health << "/" << maxHealth << "\n";
}