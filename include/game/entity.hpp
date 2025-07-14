//
// Created by julius on 7/3/25.
//

#ifndef ENTITY_HPP
#define ENTITY_HPP
#include "assetManager.hpp"
#include "block.hpp"

namespace arcader {

enum class EntityType {
    PLAYER,
    TREE,
};

class Entity {
protected:
    int ticksLived;
    EntityType type;
    float width;
    float widthHalf;
    float height;
    bool direction; // Direction of movement, true for right, false for left

    // sprite management
    StaticAssets currentSprite;
    float spriteTimer = 0.0f;

public:
    glm::vec2 position;
    glm::vec2 velocity;

    Entity(EntityType type, float width, float height, const glm::vec2& position, StaticAssets startSprite);

    virtual ~Entity() = default;

    /**
     * Pure virtual function for updating the entity's state.
     * Must be implemented by derived classes.
     * @param deltaTime Time elapsed since the last update.
     * @param blocks all blocks
     */
    virtual void update(float deltaTime, const std::vector<std::vector<Block>>& blocks) = 0;

    /**
     * Pure virtual function for rendering the entity.
     * Must be implemented by derived classes.
     * @param worldToClip Transformation matrix for rendering.
     * @param assets Reference to the AssetManager for accessing assets.
     */
    virtual void render(const glm::mat4 &worldToClip, AssetManager *assets) const {}

    [[nodiscard]] virtual EntityType getType() const { return type; }
    [[nodiscard]] virtual float getWidth() const { return width; }
    [[nodiscard]] virtual float getHeight() const { return height; }
    [[nodiscard]] virtual int getTicksLived() const { return ticksLived; }
    [[nodiscard]] virtual bool getDirection() const { return direction; }
    [[nodiscard]] virtual StaticAssets getTexture() const { return currentSprite; }

    void updateTexture(const StaticAssets newSprite, const float time) {
        spriteTimer = time;
        currentSprite = newSprite;
    }
};


/**
 * EntityPlayer class representing a player entity in the game.
 * Inherits from Entity and implements specific behavior for player entities.
 */
class EntityPlayer final : public Entity {
public:
    bool isPressingRight = false;
    bool isPressingLeft = false;
    bool isPressingUp = false;
    bool isPressingDown = false;
    bool isSprinting = false;
    bool isJumping = false;
    BlockType selected = BlockType::AIR;

    /**
     * Constructor for EntityPlayer.
     * @param position Initial position of the player.
     */
    explicit EntityPlayer(const glm::vec2& position)
        : Entity(EntityType::PLAYER, 0.4f, 0.9f, position, StaticAssets::PLAYER_IDLE) {}

    /**
     * Updates the player's state.
     * @param deltaTime Time elapsed since the last update.
     * @param blocks
     */
    void update(float deltaTime, const std::vector<std::vector<Block>>& blocks) override;

    /**
     * Renders the player entity.
     * @param worldToClip Transformation matrix for rendering.
     * @param assets Reference to the AssetManager for accessing assets.
     */
    void render(const glm::mat4 & worldToClip, AssetManager *assets) const override;

    /**
     * Gets the current direction of the player.
     * @return Current direction vector.
     */
    [[nodiscard]] bool getDirection() const override;

    [[nodiscard]] glm::uvec2 getTargetPosition() const;
};

} // arcader

#endif //ENTITY_HPP
