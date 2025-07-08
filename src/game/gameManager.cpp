//
// Created by julius on 6/26/25.
//

#include "game/gameManager.hpp"

#include <iostream>

#include "framework/mesh.hpp"
#include "game/block.hpp"

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
    for (int i = 0; i < worldWidth; ++i) {
        blocks[i][0] = {BlockType::STONE, StaticAssets::BLOCK_STONE};
        blocks[i][1] = {BlockType::STONE, StaticAssets::BLOCK_STONE};
        blocks[i][2] = {BlockType::DIRT, StaticAssets::BLOCK_DIRT};
        blocks[i][3] = {BlockType::GRASS, StaticAssets::BLOCK_GRASS};
        blocks[i][30] = {BlockType::WOOD, StaticAssets::BLOCK_WOOD};
        blocks[i][31] = {BlockType::LEAVES, StaticAssets::BLOCK_LEAVES};
    }
    blocks[15][15] = {BlockType::DIRT, StaticAssets::BLOCK_DIRT};

    // Initialize player
    printf("  - Initializing entities...\n");
    entities.clear();
    auto player = new EntityPlayer(vec2(0.0f, 6.0f), 32);
    auto pPlayer = std::unique_ptr<Entity>(player);
    entities.push_back(std::move(pPlayer));

    // Setup camera properly
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

void GameManager::render(Camera &camera) {
    const mat4& projection = camera.projectionMatrix;
    const mat4& view = camera.viewMatrix;

    // Camera
    camera.projectionMatrix = ortho(
    0.0f, static_cast<float>(worldWidth),
    0.0f, static_cast<float>(worldHeight),
    0.1f, 100.0f
    );
    camera.worldPosition = vec3(worldWidth / 2.0f, worldHeight / 2.0f, 10.0f);
    camera.viewMatrix = lookAt(
        camera.worldPosition,
        vec3(worldWidth / 2.0f, worldHeight / 2.0f, 0.0f), // look at center
        vec3(0.0f, 1.0f, 0.0f) // up direction
    );

    
    // --- Render Blocks ---
    tile_shader.use();
    for (int y = 0; y < worldHeight; ++y) {
        for (int x = 0; x < worldWidth; ++x) {
            auto& [type, texture] = blocks[x][y];
            if (type == BlockType::AIR) continue;

            vec3 worldPos = vec3(x + worldWidth / 2, y + worldHeight / 2, 0.0f);

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
    printf("Key %s \tAction %s \tMod %s\n", sKey.c_str(), sAction.c_str(), sModifier.c_str());
}
} // arcader
