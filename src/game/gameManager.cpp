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
    for (int x = 0; x < worldWidth; ++x) {
        for (int y = 0; y < worldHeight; ++y) {
            blocks[x][y] = {BlockType::AIR, StaticAssets::BLOCK_AIR};
        }
    }

    generateTerrain();

    // Initialize player
    printf("  - Initializing entities...\n");
    entities.clear();
    auto player = new EntityPlayer(vec2(0.0f, 6.0f), 32);
    auto pPlayer = std::unique_ptr<Entity>(player);
    entities.push_back(std::move(pPlayer));

    // Setup camera properly
}

void GameManager::generateTerrain() {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    std::random_device rd;
    const int seed = rd();
    printf("    - Seed: %d\n", seed);
    noise.SetSeed(seed);
    noise.SetFrequency(0.03f);

    for (int x = 0; x < worldWidth; ++x) {
        // Base terrain height
        const float base = noise.GetNoise(static_cast<float>(x), 0.0f);                  // Base terrain
        const float mountain = noise.GetNoise(static_cast<float>(x) * 0.5f, 100.0f);  // Large features
        const float overhang = noise.GetNoise(static_cast<float>(x) * 3.0f, 200.0f);  // Small jagged shapes

        // Shape terrain: combine noise layers
        const int height = 8 + static_cast<int>(
            base * 5.0f +
            std::pow(std::max(0.0f, mountain), 2.0f) * 10.0f +
            std::sin(x * 0.3f) * 1.5f
        );

        // Apply overhang logic
        const bool hasOverhang = overhang > 0.45f;

        for (int y = 0; y < worldHeight; ++y) {
            if (y < height - 3)
                placeBlock(x, y, BlockType::STONE);
            else if (y < height - 1)
                placeBlock(x, y, BlockType::DIRT);
            else if (y == height - 1)
                placeBlock(x, y, BlockType::GRASS);
            else if (hasOverhang && y == height)
                placeBlock(x, y, BlockType::DIRT); // overhang
            else
                blocks[x][y] = {BlockType::AIR, StaticAssets::BLOCK_AIR}; // dont use place function on air, waste of resources
        }
    }
}

void GameManager::placeBlock(const int x, const int y, const BlockType type) {
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

void GameManager::update(float deltaTime) {
    // Update entities
    for (const auto &entity : entities) {
        entity->update(deltaTime);
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
    entity_shader.use();
    entity_shader.set("u_Texture", 0); // bind once

    for (const auto& entity : entities) {
        // vec3 entityPos3D = tilemapOrigin + vec3(entity->position, 0.0f);
        // mat4 model = translate(mat4(1.0f), entityPos3D);
        // model = scale(model, vec3(entity->getDimension(), entity->getDimension(), 1.0f));
        // mat4 mvp = projection * view * model;
        //
        // entity_shader.set("u_MVP", mvp);
        // glBindTexture(GL_TEXTURE_2D, assets->getTexture(entity->getTexture()).handle);
        // entity->render(mvp, assets);
    }
}

void GameManager::keyCallback(Key key, Action action, Modifier modifier) {
    auto sKey = debugKeyToString(key);
    auto sAction = debugActionToString(action);
    auto sModifier = debugModToString(modifier);
    //printf("Key %s \tAction %s \tMod %s\n", sKey.c_str(), sAction.c_str(), sModifier.c_str());

    switch (key) {
        case Key::D: offset.x += 0.1f; break;
            case Key::A: offset.x -= 0.1f; break;
        case Key::W: offset.y += 0.1f; break;
            case Key::S: offset.y -= 0.1f; break;
    }
}
} // arcader
