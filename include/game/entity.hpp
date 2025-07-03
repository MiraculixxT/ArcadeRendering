//
// Created by julius on 7/3/25.
//

#ifndef ENTITY_HPP
#define ENTITY_HPP
#include "assetManager.hpp"

namespace arcader {

enum class EntityType {
    PLAYER,
    TREE,
};

class Entity {
protected:
    int ticksLived;
    glm::vec2 velocity;
    EntityType type;
    StaticAssets texture;
    int dimension;

public:
    glm::vec2 position;

    Entity(const EntityType type, const int dimension, const glm::vec2& position)
        : ticksLived(0), velocity(glm::vec2(0.0,0.0)), type(type), texture(getTextureToFromType(type)), dimension(dimension), position(position) {}

    virtual ~Entity() = default;

    /**
     * Pure virtual function for updating the entity's state.
     * Must be implemented by derived classes.
     * @param deltaTime Time elapsed since the last update.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * Pure virtual function for rendering the entity.
     * Must be implemented by derived classes.
     * @param worldToClip Transformation matrix for rendering.
     * @param assets Reference to the AssetManager for accessing assets.
     */
    virtual void render(const glm::mat4 &worldToClip, AssetManager &assets) const {}

    virtual EntityType getType() const { return type; }
    virtual StaticAssets getTexture() const { return texture; }
    virtual int getDimension() const { return dimension; }

    static StaticAssets getTextureToFromType(const EntityType& type);
};


/**
 * EntityPlayer class representing a player entity in the game.
 * Inherits from Entity and implements specific behavior for player entities.
 */
class EntityPlayer final : public Entity {
    bool direction; // Direction of movement, true for right, false for left

public:
    /**
     * Constructor for EntityPlayer.
     * @param position Initial position of the player.
     * @param dimension Size of the player entity.
     */
    EntityPlayer(const glm::vec2& position, const int dimension)
        : Entity(EntityType::PLAYER, dimension, position), direction(true) {}

    /**
     * Updates the player's state.
     * @param deltaTime Time elapsed since the last update.
     */
    void update(float deltaTime) override;

    /**
     * Renders the player entity.
     * @param worldToClip Transformation matrix for rendering.
     * @param assets Reference to the AssetManager for accessing assets.
     */
    void render(const glm::mat4& worldToClip, AssetManager& assets) const override;

    /**
     * Gets the current direction of the player.
     * @return Current direction vector.
     */
    bool getDirection() const;
};

} // arcader

#endif //ENTITY_HPP
