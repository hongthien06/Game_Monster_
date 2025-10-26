#include "Character.h"

Character::Character()
    : currentTexture(nullptr),
    position(0.0f, 0.0f),
    velocity(0.0f, 0.0f),
    isOnGround(true),
    flipHorizontal(false),
    state(CharacterState::IDLE),
    currentFrame(0),
    animationTimer(0.0f) {
}

Character::Character(glm::vec2 startPos)
    : currentTexture(nullptr),
    position(startPos),
    velocity(0.0f, 0.0f),
    isOnGround(true),
    flipHorizontal(false),
    state(CharacterState::IDLE),
    currentFrame(0),
    animationTimer(0.0f) {
}

Character::~Character() {}

void Character::SetPosition(const glm::vec2& pos) {
    position = pos;
}

glm::vec2 Character::GetPosition() const {
    return position;
}

void Character::SetVelocity(const glm::vec2& vel) {
    velocity = vel;
}

glm::vec2 Character::GetVelocity() const {
    return velocity;
}

void Character::SetState(CharacterState newState) {
    state = newState;
}

CharacterState Character::GetState() const {
    return state;
}
