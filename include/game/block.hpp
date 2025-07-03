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

class BlockStates {
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
};
} // arcader

#endif //STATES_H
