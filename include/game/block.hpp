//
// Created by julius on 6/26/25.
//

#ifndef STATES_H
#define STATES_H
#include <vector>

#include "assetManager.hpp"

namespace arcader {
/**
 * Block type for each block in the world.
 * Important for rendering and distinction of blocks.
 */
enum class BlockType {
    GRASS,
    DIRT,
    WOOD,
    LEAVES,
    STONE,
    WATER,

    AIR
};

/**
 * Holder struct for all information a block can have.
 * This is mainly for the block type, but maybe blocks will have some states, like water later.
 */
struct Block {
    BlockType type;
    StaticAssets texture;
};

/**
 * Holder struct for block updates that are scheduled to be applied.
 */
struct BlockUpdate {
    BlockType type;
    glm::uvec2 position;
};

class BlockStates {
public:
    /**
     * Retrieves all available block types in the game.
     * @return A vector containing all block types.
     */
    static std::vector<BlockType> getBlockTypes();

    /**
     * Maps a block type to its corresponding texture.
     * @param type The block type to map.
     * @return The texture associated with the given block type.
     */
    static StaticAssets getTextureToFromType(const BlockType &type);

    /**
     * Gets the enums name of textures for debug purposes.
     */
    static std::string getTextureName(const BlockType &type);

    /**
     * Check if the block type is solid or if entities can pass through it.
     */
    static bool isSolid(const BlockType &type);

    /**
     * Checks if a position collides with any solid blocks in the world.
     */
    static bool isColliding(const glm::vec2& pos, const std::vector<std::vector<Block>>& blocks);

    /**
     * Get the first block from top to bottom that is not air.
     * @param ignoreLeaves Useful for placing the player under trees, not on top
     * @param x X coordinate in world
     * @param blocks Blocks in world
     */
    static int getHighestBlock(bool ignoreLeaves, int x, const std::vector<std::vector<Block>>& blocks);

    static bool isInBounds(glm::uvec2 pos, const std::vector<std::vector<Block>>& blocks);
};
} // arcader

#endif //STATES_H
