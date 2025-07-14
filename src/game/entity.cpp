//
// Created by julius on 7/3/25.
//

#include "game/entity.hpp"

#include "audioPlayer.hpp"
#include "framework/app.hpp"
#include "game/block.hpp"
#include "glm/detail/_noise.hpp"

namespace arcader {
/*
StaticAssets Entity::getTextureToFromType(const EntityType &type) {
    switch (type) {
        case EntityType::PLAYER: return StaticAssets::ENTITY_PLAYER;
        case EntityType::TREE: return StaticAssets::ENTITY_TREE;

        default: return StaticAssets::MISSING_TEXTURE;
    }
}
*/

Entity::Entity(const EntityType type, const float width, const float height, const glm::vec2 &position, const StaticAssets startSprite):
    ticksLived(0),
    type(type),
    width(width),
    widthHalf(width / 2.0f),
    height(height),
    direction(false),
    currentSprite(startSprite),
    position(position),
    velocity(vec2(0.0, 0.0)) {
}

bool canJump = true;

void EntityPlayer::update(const float deltaTime, const std::vector<std::vector<Block>>& blocks) {
    constexpr float gravity = -8.0f;
    constexpr float maxFallSpeed = -5.0f;

    // React to input
    const int curX = static_cast<int>(std::floor(position.x));
    const int curY = static_cast<int>(std::floor(position.y));
    const bool isInWater = blocks[curX][curY].type == BlockType::WATER;
    if (isJumping) {
        if (isInWater) velocity.y = 1.0f;
        else if (velocity.y == 0.0f) {
            if (canJump) {
                canJump = false;
                velocity.y = 5.0f;
            } else canJump = true;
        }
    }

    const float sprintMult = (isSprinting && !isInWater) ? 2.0f : 1.0f;
    if (isPressingLeft) velocity.x = -1.0f * sprintMult;
    if (isPressingRight) velocity.x = 1.0f * sprintMult;
    if (isPressingLeft && isPressingRight) velocity.x = 0.0f; // Pressing both should cancel any movement


    // Apply gravity
    velocity.y += gravity * deltaTime;
    if (velocity.y < maxFallSpeed) velocity.y = maxFallSpeed;

    // --- Horizontal movement ---
    float newX = position.x + velocity.x * deltaTime;

    // Sweep horizontally (Y-range = full height)
    const int startY = static_cast<int>(std::floor(position.y));
    const int endY = static_cast<int>(std::floor(position.y + height - 0.001f));

    int checkX = static_cast<int>((velocity.x > 0)
        ? std::floor(newX + widthHalf - 0.001f)
        : std::floor(newX - widthHalf));

    bool xBlocked = false;
    for (int y = startY; y <= endY; ++y) {
        if (BlockStates::isColliding({ checkX, y }, blocks)) {
            xBlocked = true;
            break;
        }
    }

    if (xBlocked) {
        velocity.x = 0.0f;
        newX = position.x;
    }


    // --- Vertical movement ---
    float newY = position.y + velocity.y * deltaTime;

    const int startX = static_cast<int>(std::floor(newX - widthHalf));
    const int endX = static_cast<int>(std::floor(newX + widthHalf - 0.001f));

    int checkY = static_cast<int>((velocity.y > 0)
        ? std::floor(newY + height - 0.001f)
        : std::floor(newY));

    bool yBlocked = false;
    for (int x = startX; x <= endX; ++x) {
        if (BlockStates::isColliding({ x, checkY }, blocks)) {
            yBlocked = true;
            break;
        }
    }

    if (yBlocked) {
        velocity.y = 0.0f;
        newY = position.y;
    }


    // Water friction
    if (!xBlocked || !yBlocked) {
        if (isInWater) {
            velocity.y *= 0.8f;
        }
    }

    // Update position
    position.x = newX;
    position.y = newY;

    // Update direction
    if (velocity.x < 0.0f) direction = false;
    else if (velocity.x > 0.0f) direction = true;

    // Update sprite based on velocity
    if (spriteTimer >= 0.0f) {
        spriteTimer -= deltaTime;

        if (spriteTimer <= 0.0f) {
            spriteTimer = 0.0f;
            if (std::abs(velocity.x) > 0.01f) {
                // Walking animation
                if (currentSprite == StaticAssets::PLAYER_WALK1 || currentSprite == StaticAssets::PLAYER_IDLE) {
                    updateTexture(StaticAssets::PLAYER_WALK2, 0.2f);
                } else {
                    updateTexture(StaticAssets::PLAYER_WALK1, 0.2f);
                }
            } else {
                // Idle animation
                updateTexture(StaticAssets::PLAYER_IDLE, 0.0f);
            }
        }
    }

    // Friction
    velocity.x *= 0.8f;
    if (std::abs(velocity.x) < 0.01f) velocity.x = 0.0f;

    ticksLived++;
}

void EntityPlayer::render(const glm::mat4 &worldToClip, AssetManager *assets) const {
}

bool EntityPlayer::getDirection() const {
    return direction;
}

uvec2 EntityPlayer::getTargetPosition() const {
    int placeX = static_cast<int>(std::floor(position.x));
    int placeY = static_cast<int>(std::floor(position.y));

    if (isPressingUp || isPressingDown) { // prioritize vertical direction
        if (isPressingUp) placeY += 1;
        if (isPressingDown) placeY -= 1;

    } else { // Horizontal direction
        placeX += (getDirection() ? 1 : -1);
    }
    return {placeX, placeY};
}
} // arcader