#pragma once
#include "Character.h"

class Player : public Character {
public:
    Player(SDL_Renderer* renderer)
        : Character(renderer, glm::vec2(50.0f, GameConstants::FLOOR_Y),
            "assets/images/Player/Archer/Idle.png",
            "assets/images/Player/Archer/Walk.png",
            "assets/images/Player/Archer/Run.png",
            "assets/images/Player/Archer/Jump.png")
    {
    }
};
