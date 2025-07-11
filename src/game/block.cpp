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

bool BlockStates::isSolid(const BlockType &type) {
    return type != BlockType::AIR && type != BlockType::WATER;
}

bool BlockStates::isColliding(const glm::vec2 &pos, const std::vector<std::vector<Block> > &blocks) {
    int x = static_cast<int>(pos.x);
    int y = static_cast<int>(pos.y);

    if (x < 0 || x >= blocks.size() || y < 0 || y >= blocks[0].size()) {
        return true; // Outside bounds = solid
    }

    return isSolid(blocks[x][y].type);
}

int BlockStates::getHighestBlock(const bool ignoreLeaves, const int x, const std::vector<std::vector<Block> > &blocks) {
    for (int y = 31; y < blocks[x].size(); --y) {
        auto type = blocks[x][y].type;
        if (isSolid(type)) {
            if (ignoreLeaves && type == BlockType::LEAVES) {
                // check if under leaves is a non-solid
                auto subBlock = blocks[x][y - 1].type;
                if (isSolid(subBlock)) return y; // No free space underneath leaves (double leaves appear very rare, so not worth the check cost)
            }
            return y;
        }
    }
    return 0; // No solid block found
}
}
