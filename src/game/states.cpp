//
// Created by julius on 6/26/25.
//

#include "game/states.hpp"

#include "assetManager.hpp"

namespace arcader {
std::vector<BlockType> StateHandler::getBlockTypes() {
    return std::vector{
        BlockType::GRASS,
        BlockType::DIRT,
        BlockType::WOOD,
        BlockType::LEAVES,
        BlockType::STONE,
        BlockType::WATER
    };
}

StaticAssets StateHandler::getTextureToFromType(const BlockType &type) {
    switch (type) {
        case BlockType::GRASS: return StaticAssets::BLOCK_GRASS;
        case BlockType::DIRT: return StaticAssets::BLOCK_DIRT;
        case BlockType::WOOD: return StaticAssets::BLOCK_WOOD;
        case BlockType::LEAVES: return StaticAssets::BLOCK_LEAVES;
        case BlockType::STONE: return StaticAssets::BLOCK_STONE;
        case BlockType::WATER: return StaticAssets::BLOCK_WATER;
        default: return StaticAssets::MISSING_TEXTURE;
    }
};
}
