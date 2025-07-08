//
// Created by julius on 6/26/25.
//

#include "game/block.hpp"

#include "assetManager.hpp"

namespace arcader {
std::vector<BlockType> BlockStates::getBlockTypes() {
    return std::vector{
        BlockType::GRASS,
        BlockType::DIRT,
        BlockType::WOOD,
        BlockType::LEAVES,
        BlockType::STONE,
        BlockType::WATER,
        BlockType::AIR,
    };
}

StaticAssets BlockStates::getTextureToFromType(const BlockType &type) {
    switch (type) {
        case BlockType::GRASS: return StaticAssets::BLOCK_GRASS;
        case BlockType::DIRT: return StaticAssets::BLOCK_DIRT;
        case BlockType::WOOD: return StaticAssets::BLOCK_WOOD;
        case BlockType::LEAVES: return StaticAssets::BLOCK_LEAVES;
        case BlockType::STONE: return StaticAssets::BLOCK_STONE;
        case BlockType::WATER: return StaticAssets::BLOCK_WATER;
        case BlockType::AIR: return StaticAssets::BLOCK_AIR;
        default:
            printf("Missing Texture: %d\n", static_cast<int>(type));
            return StaticAssets::MISSING_TEXTURE;
    }
};

std::string BlockStates::getTextureName(const BlockType &type) {
    switch (type) {
        case BlockType::GRASS: return "grass";
        case BlockType::DIRT: return "dirt";
        case BlockType::WOOD: return "wood";
        case BlockType::LEAVES: return "leaves";
        case BlockType::STONE: return "stone";
        case BlockType::WATER: return "water";
        case BlockType::AIR: return "air";
        default:
            printf("Missing Texture Name for type: %d\n", static_cast<int>(type));
            return "missing_texture";
    }
}
}
