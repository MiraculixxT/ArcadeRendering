//
// Created by julius on 6/26/25.
//

#include "game/gameManager.hpp"

#include <iostream>
#include <random>

#include "framework/mesh.hpp"
#include "game/block.hpp"
#include "game/FastNoiseLite.hpp"

namespace arcader {
/**
   * Constructor for the `GameManager` class.
   * @param assetsManager Reference to the `AssetManager` instance.
   */
GameManager::GameManager(AssetManager *assetsManager, int *height, int *width) : assets(assetsManager), screenHeight(height), screenWidth(width),
                                                                                 tile_shader(assetsManager->getShader(StaticAssets::SHADER_TILE)),
                                                                                 entity_shader(assetsManager->getShader(StaticAssets::SHADER_ENTITY)),
                                                                                 debugShader(assetsManager->getShader(StaticAssets::SHADER_DEBUG)) {
    blocks.resize(worldWidth, std::vector<Block>(worldHeight));
};

void GameManager::init() {
    printf("Initializing game...\n");

    // Load textures
    printf("  - Loading textures...\n");
    for (auto type : BlockStates::getBlockTypes()) {
        StaticAssets texture = BlockStates::getTextureToFromType(type);
        assets->loadTexture(texture, "assets/textures/game/" + BlockStates::getTextureName(type) + ".png");
    }
    assets->loadTexture(StaticAssets::ENTITY_PLAYER, "assets/textures/game/player.png");

    // Load mesh
    printf("  - Loading mesh...\n");
    std::vector<Mesh::VertexPTN> vertices = {
        //  position           texCoord      normal
        {{0.0f, 0.0f, 0.0f},  {0.0f, 0.0f},  {0.0f, 0.0f, 1.0f}},
        {{1.0f, 0.0f, 0.0f},  {1.0f, 0.0f},  {0.0f, 0.0f, 1.0f}},
        {{1.0f, 1.0f, 0.0f},  {1.0f, 1.0f},  {0.0f, 0.0f, 1.0f}},
        {{0.0f, 1.0f, 0.0f},  {0.0f, 1.0f},  {0.0f, 0.0f, 1.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };
    mesh = Mesh();
    mesh.load(vertices, indices);

    // Initilize blocks (temp, maybe doing procedural generation later)
    printf("  - Initializing blocks...\n");
    std::random_device rd;
    seed = rd();
    frequency = 0.03f;
    terrainBase = 0.0f;
    terrainPeak = 100.0f;
    treeFrequency = 0.15f;
    waterLevel = 7;
    generateTerrain();
    generateTrees();

    // Initialize player
    printf("  - Initializing entities...\n");
    entities.clear();
    auto pPlayer = std::make_unique<EntityPlayer>(vec2(16, BlockStates::getHighestBlock(true, 16, blocks) + 1));
    player = pPlayer.get();
    entities.push_back(std::move(pPlayer));
}

void GameManager::generateTerrain() {
    // Reset
    for (int x = 0; x < worldWidth; ++x) {
        for (int y = 0; y < worldHeight; ++y) {
            blocks[x][y] = {BlockType::AIR, StaticAssets::BLOCK_AIR};
        }
    }

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    noise.SetSeed(seed);
    noise.SetFrequency(frequency);

    for (int x = 0; x < worldWidth; ++x) {
        // Base terrain height
        const float base = noise.GetNoise(static_cast<float>(x), terrainBase);                  // Base terrain
        const float mountain = noise.GetNoise(static_cast<float>(x) * 0.5f, terrainPeak);  // Large features

        // Shape terrain: combine noise layers
        int height = 8 + static_cast<int>(
            base * 4.0f +
            std::pow(std::max(0.0f, mountain), 3.0f) * 25.0f +
            std::sin(x * 0.3f) * 1.5f
        );
        height = std::min(height, worldHeight - 3); // Leave some space for blocks above

        for (int y = 0; y < worldHeight; ++y) {
            if (y < height - 3)
                placeBlock(x, y, BlockType::STONE);
            else if (y < height - 1)
                placeBlock(x, y, BlockType::DIRT);
            else if (y == height - 1)
                placeBlock(x, y, BlockType::GRASS);
            else if (y <= waterLevel)
                placeBlock(x, y, BlockType::WATER);
            else
                blocks[x][y] = {BlockType::AIR, StaticAssets::BLOCK_AIR}; // dont use place function on air, waste of resources
        }
    }
}

void GameManager::generateTrees() {
    FastNoiseLite treeNoise;
    treeNoise.SetSeed(seed + 42); // Offset from terrain seed
    treeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    treeNoise.SetFrequency(treeFrequency); // Controls tree spacing

    for (int x = 1; x < worldWidth - 1; ++x) {
        for (int y = 0; y < worldHeight - 4; ++y) {
            if (blocks[x][y].type != BlockType::GRASS)
                continue;

            // Check space above for tree
            bool canPlaceTree = true;
            for (int dy = 1; dy <= 3 && canPlaceTree; ++dy) {
                if (blocks[x][y + dy].type != BlockType::AIR)
                    canPlaceTree = false;
            }

            if (!canPlaceTree)
                continue;

            // Use noise to decide whether to place a tree
            float noiseValue = treeNoise.GetNoise(static_cast<float>(x), static_cast<float>(y));
            if (noiseValue < 0.4f) // Density
                continue;

            std::mt19937 gen(seed + x);
            std::uniform_real_distribution dist(2.0f, 5.0f);
            const int treeHeight = dist(gen); // 2 to 5

            // Place wood
            for (int i = 1; i < treeHeight; ++i) {
                placeBlock(x, y + i, BlockType::WOOD);
            }

            // Place leaves
            placeBlock(x, y + 1 + treeHeight, BlockType::LEAVES); // center top
            placeBlock(x - 1, y + treeHeight, BlockType::LEAVES);
            placeBlock(x,     y + treeHeight, BlockType::LEAVES);
            placeBlock(x + 1, y + treeHeight, BlockType::LEAVES);
        }
    }
}

void GameManager::placeBlock(const int x, const int y, const BlockType type) {
    auto currentBlock = blocks[x][y];
    if (type != BlockType::AIR && currentBlock.type == BlockType::WOOD) return;

    Block newBlock = {type, BlockStates::getTextureToFromType(type)};
    blocks[x][y] = newBlock;

    // Check if we are grass and need to decay (block aboth)
    if (type == BlockType::GRASS) {
        const auto topBlock = &blocks[x][y+1];
        if (topBlock->type != BlockType::AIR) blocks[x][y+1] = {BlockType::DIRT, StaticAssets::BLOCK_DIRT};
    }

    // Check if underneath is grass to decay
    const auto subBlock = &blocks[x][y-1];
    if (subBlock->type == BlockType::GRASS) {
        blocks[x][y-1] = {BlockType::DIRT, StaticAssets::BLOCK_DIRT};
    }
}

void GameManager::update(const float deltaTime) const {
    // Update entities
    for (auto &entity : entities) {
        entity->update(deltaTime, blocks);
    }
}

void GameManager::renderDebug(Camera& camera) {
    debugShader.use();

    const mat4& projection = camera.projectionMatrix;
    const mat4& view = camera.viewMatrix;

    // Proper forward vector
    const vec3 forward = -normalize(vec3(camera.cameraMatrix[2]));
    const vec3 debugPos = camera.worldPosition + forward * 5.0f;

    mat4 model = translate(mat4(1.0f), debugPos);

    mat4 localToWorld = model;
    mat4 localToClip = projection * view * model;

    debugShader.set("uLocalToClip", localToClip);
    debugShader.set("uLocalToWorld", localToWorld);
    debugShader.set("u_Color", vec4(1.0f, 0.0f, 0.0f, 1.0f));

    mesh.draw();
}

vec2 offset = vec2(0.0f, 0.0f);

void GameManager::render(Camera &camera) {
    const mat4& projection = camera.projectionMatrix;
    const mat4& view = camera.viewMatrix;

    // Camera
    const float relativeWidth = static_cast<float>(*screenWidth) / static_cast<float>(*screenHeight);
    const float relativeOffset = relativeWidth * worldWidth;
    camera.projectionMatrix = ortho(
    0.0f, relativeOffset,
    0.0f, static_cast<float>(worldHeight),
    0.1f, 100.0f
    );

    vec2 base = vec2(worldWidth / 2.0f - relativeOffset / 2.0f, 0.0f);
    vec3 cameraPos   = vec3(offset + base, 10.0f);  // move in XY, look from Z
    vec3 cameraTarget = vec3(offset + base, 0.0f);   // look straight down at the same XY
    camera.worldPosition = cameraPos;
    camera.viewMatrix = lookAt(
        cameraPos,
        cameraTarget, // look at center
        vec3(0.0f, 1.0f, 0.0f) // up direction
    );

    
    // --- Render Blocks ---
    tile_shader.use();
    for (int y = 0; y < worldHeight; ++y) {
        for (int x = 0; x < worldWidth; ++x) {
            auto& [type, texture] = blocks[x][y];
            //if (type == BlockType::AIR) continue;

            vec3 worldPos = vec3(x, y, 0.0f);

            mat4 model = translate(mat4(1.0f), worldPos);
            mat4 mvp = projection * view * model;

            tile_shader.set("u_MVP", mvp);
            tile_shader.set("u_Texture", 0);

            GLuint texID = assets->getTexture(texture).handle;
            glBindTexture(GL_TEXTURE_2D, texID);
            mesh.draw();
        }
    }

    // --- Render Entities ---
    for (const auto& entity : entities) {
        entity_shader.use();
        entity_shader.set("u_Texture", 0);
        entity_shader.set("u_FlipX", entity->getDirection());

        vec3 worldPos = vec3(entity->position - vec2(0.5, 0.0), 0.01f);
        mat4 model = translate(mat4(1.0f), worldPos);
        mat4 mvp = projection * view * model;

        entity_shader.set("u_MVP", mvp);
        entity_shader.set("u_Time", entity->getTicksLived());
        glBindTexture(GL_TEXTURE_2D, assets->getTexture(entity->getTexture()).handle);
        //entity->render(mvp, assets);
        mesh.draw();

        // ---- Debug ----
        if (!showHitboxes) continue;
        debugShader.use();
        debugShader.set("u_Color", vec4(1.0f, 0.0f, 0.0f, 0.8f));
        vec3 worldPos2 = vec3(entity->position - vec2(entity->getWidth() / 2.0f, 0.0), 0.02f);
        mat4 model2 = translate(mat4(1.0f), worldPos2);
        model2 = scale(model2, vec3(entity->getWidth(), entity->getHeight(), 1.0f));
        mat4 mvp2 = projection * view * model2;
        debugShader.set("u_MVP", mvp2);
        mesh.draw();
    }
}

void GameManager::keyCallback(Key key, Action action, Modifier modifier) {
    auto sKey = debugKeyToString(key);
    auto sAction = debugActionToString(action);
    auto sModifier = debugModToString(modifier);
    //printf("Key %s \tAction %s \tMod %s\n", sKey.c_str(), sAction.c_str(), sModifier.c_str());

    // Only captcha changes
    if (action == Action::REPEAT) return;

    switch (key) {
        case Key::D: player->isPressingRight = action == Action::PRESS; break;
        case Key::A: player->isPressingLeft = action == Action::PRESS; break;
        case Key::LEFT_CONTROL: player->isSprinting = action == Action::PRESS; break;
        case Key::SPACE: player->isJumping = action == Action::PRESS; break;

        case Key::B:
            if (action != Action::PRESS) break;
            showHitboxes = !showHitboxes;
            break;
    }
}
} // arcader
