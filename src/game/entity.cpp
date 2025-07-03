//
// Created by julius on 7/3/25.
//

#include "game/entity.hpp"

namespace arcader {

StaticAssets Entity::getTextureToFromType(const EntityType &type) {
    switch (type) {
        case EntityType::PLAYER: return StaticAssets::ENTITY_PLAYER;
        case EntityType::TREE: return StaticAssets::ENTITY_TREE;

        default: return StaticAssets::MISSING_TEXTURE;
    }
}

void EntityPlayer::update(float deltaTime) {
    // Update position based on velocity and direction
    position += velocity * deltaTime;
    ticksLived++;

    // Update direction based on velocity
    if (velocity.x < 0.0f) direction = false; // Moving left
    else if (velocity.x > 0.0f) direction = true; // Moving right
    // else keep current direction if standing still
}


void EntityPlayer::render(const glm::mat4 &worldToClip, AssetManager &assets) const {
    assets.render(texture, worldToClip, glm::vec3(position, 0.0f), glm::vec3(dimension));
}

bool EntityPlayer::getDirection() const {
    return direction;
}

} // arcader