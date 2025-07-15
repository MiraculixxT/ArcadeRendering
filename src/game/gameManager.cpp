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

GameManager::GameManager(AssetManager *assetsManager, int *height, int *width) : assets(assetsManager), screenHeight(height), screenWidth(width),
                                                                                 tileShader(assetsManager->getShader(StaticAssets::SHADER_TILE)),
                                                                                 entityShader(assetsManager->getShader(StaticAssets::SHADER_ENTITY)),
                                                                                 debugShader(assetsManager->getShader(StaticAssets::SHADER_DEBUG)),
                                                                                 hudShader(assetsManager->getShader(StaticAssets::SHADER_HUD)),
                                                                                 audioPlayer(AudioPlayer{}) {
    audioPlayer.init();
    blocks.resize(worldWidth, std::vector<Block>(worldHeight));
};

void GameManager::init() {
    printf("Initializing game...\n");
    startTime = static_cast<float>(glfwGetTime()); // Store start time to start from 0

    // Load textures
    printf("  - Loading textures...\n");
    for (auto type : BlockStates::getBlockTypes()) {
        StaticAssets texture = BlockStates::getTextureToFromType(type);
        assets->loadTexture(texture, "assets/textures/game/" + BlockStates::getTextureName(type) + ".png");
    }
    assets->loadTexture(StaticAssets::PLAYER_IDLE, "assets/textures/game/player_stand.png");
    assets->loadTexture(StaticAssets::PLAYER_MINE, "assets/textures/game/player_mine.png");
    assets->loadTexture(StaticAssets::PLAYER_WALK1, "assets/textures/game/player_walk1.png");
    assets->loadTexture(StaticAssets::PLAYER_WALK2, "assets/textures/game/player_walk2.png");
    assets->loadTexture(StaticAssets::HUD_SLOT, "assets/textures/game/slot.png");
    assets->loadTexture(StaticAssets::BACKGROUND, "assets/textures/game/background.png");

    // Load mesh
    printf("  - Loading mesh...\n");
    const std::vector<Mesh::VertexPTN> vertices = {
        //  position           texCoord      normal
        {{0.0f, 0.0f, 0.0f},  {0.0f, 0.0f},  {0.0f, 0.0f, 1.0f}},
        {{1.0f, 0.0f, 0.0f},  {1.0f, 0.0f},  {0.0f, 0.0f, 1.0f}},
        {{1.0f, 1.0f, 0.0f},  {1.0f, 1.0f},  {0.0f, 0.0f, 1.0f}},
        {{0.0f, 1.0f, 0.0f},  {0.0f, 1.0f},  {0.0f, 0.0f, 1.0f}}
    };

    const std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };
    mesh = Mesh();
    mesh.load(vertices, indices);

    // Initialize blocks (temp, maybe doing procedural generation later)
    printf("  - Initializing blocks...\n");
    std::random_device rd;
    seed = static_cast<int>(rd());
    frequency = 0.04f;
    terrainBase = 0.0f;
    terrainPeak = 100.0f;
    treeFrequency = 0.15f;
    waterLevel = 7;
    generateTerrain();
    generateTrees();

    // Initialize player
    printf("  - Initializing entities...\n");
    entities.clear();
    auto pPlayer = std::make_unique<EntityPlayer>(vec2(16.5, BlockStates::getHighestBlock(true, 16, blocks) + 1));
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
            const auto pos = uvec2(x, y);
            if (y < height - 3)
                placeBlock(pos, BlockType::STONE);
            else if (y < height - 1)
                placeBlock(pos, BlockType::DIRT);
            else if (y == height - 1)
                placeBlock(pos, BlockType::GRASS);
            else if (y <= waterLevel)
                placeBlock(pos, BlockType::WATER);
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
            const float noiseValue = treeNoise.GetNoise(static_cast<float>(x), static_cast<float>(y));
            if (noiseValue < 0.4f) // Density
                continue;

            std::mt19937 gen(seed + x);
            std::uniform_real_distribution dist(2.0f, 5.0f);
            const int treeHeight = static_cast<int>(dist(gen)); // 2 to 5

            // Place wood
            for (int i = 1; i < treeHeight; ++i) {
                placeBlock(uvec2(x, y + i), BlockType::WOOD);
            }

            // Place leaves
            placeBlock(uvec2(x, y + 1 + treeHeight), BlockType::LEAVES); // center top
            placeBlock(uvec2(x - 1, y + treeHeight), BlockType::LEAVES);
            placeBlock(uvec2(x,     y + treeHeight), BlockType::LEAVES);
            placeBlock(uvec2(x + 1, y + treeHeight), BlockType::LEAVES);
        }
    }
}

void GameManager::placeBlock(const uvec2 pos, const BlockType type) {
    const auto x = pos.x;
    const auto y = pos.y;
    const auto currentBlock = blocks[x][y];
    if (type != BlockType::AIR && currentBlock.type == BlockType::WOOD) return;

    const Block newBlock = {type, BlockStates::getTextureToFromType(type)};
    blocks[x][y] = newBlock;

    // Check if we are grass and need to decay (block above)
    if (type == BlockType::GRASS) {
        const auto topBlock = &blocks[x][y+1];
        if (topBlock->type != BlockType::AIR) blocks[x][y+1] = {BlockType::DIRT, StaticAssets::BLOCK_DIRT};
    }

    // Check if we are water and need to flow
    if (type == BlockType::WATER) {
        // Check if we can flow down
        if (y > 0 && blocks[x][y - 1].type == BlockType::AIR) {
            placeBlock(uvec2(x, y - 1), BlockType::WATER);
        }
        // Check if we can flow left or right
        if (x > 0 && blocks[x - 1][y].type == BlockType::AIR) {
            placeBlock(uvec2(x - 1, y), BlockType::WATER);
        }
        if (x < worldWidth - 1 && blocks[x + 1][y].type == BlockType::AIR) {
            placeBlock(uvec2(x + 1, y), BlockType::WATER);
        }
    }

    // Check if underneath is grass to decay
    if (y <= 0) return;
    const auto subBlock = &blocks[x][y-1];
    if (subBlock->type == BlockType::GRASS) {
        blocks[x][y-1] = {BlockType::DIRT, StaticAssets::BLOCK_DIRT};
    }
}

void GameManager::breakBlock(const uvec2 pos) {
    const auto x = pos.x;
    const auto y = pos.y;
    auto [type, texture] = blocks[x][y];
    if (type == BlockType::AIR || type == BlockType::WATER) return;
    player->selected = type; // Set the selected block type to the one that was broken
    blocks[x][y] = {BlockType::AIR, StaticAssets::BLOCK_AIR};

    // Update water
    if ((y < 31 && blocks[x][y + 1].type == BlockType::WATER) || // aboth
        (x < worldWidth-1 && blocks[x+1][y].type == BlockType::WATER) || // right
        (x > 0 && blocks[x-1][y].type == BlockType::WATER)) { // left
        placeBlock(pos, BlockType::WATER);
    }
}

void GameManager::update(const float deltaTime) {
    // Update entities
    for (const auto &entity : entities) {
        entity->update(deltaTime, blocks, audioPlayer);
    }
}

void GameManager::renderDebug(Camera& camera) {
    debugShader.use();

    const mat4& projection = camera.projectionMatrix;
    const mat4& view = camera.viewMatrix;

    // Proper forward vector
    const vec3 forward = -normalize(vec3(camera.cameraMatrix[2]));
    const vec3 debugPos = camera.worldPosition + forward * 5.0f;

    const mat4 model = translate(mat4(1.0f), debugPos);

    const mat4 localToWorld = model;
    const mat4 localToClip = projection * view * model;

    debugShader.set("uLocalToClip", localToClip);
    debugShader.set("uLocalToWorld", localToWorld);
    debugShader.set("u_Color", vec4(1.0f, 0.0f, 0.0f, 1.0f));

    mesh.draw();
}

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

    const float offsetX = worldWidth / 2.0f - relativeOffset / 2.0f;
    auto base = vec2(offsetX, 0.0f);
    auto cameraPos   = vec3(base, 10.0f);  // move in XY, look from Z
    auto cameraTarget = vec3(base, 0.0f);   // look straight down at the same XY
    camera.worldPosition = cameraPos;
    camera.viewMatrix = lookAt(
        cameraPos,
        cameraTarget, // look at center
        vec3(0.0f, 1.0f, 0.0f) // up direction
    );
    auto time = static_cast<float>(glfwGetTime()) - startTime;

    // --- Render Background ---
    tileShader.use();
    mat4 bgModel = translate(mat4(1.0f), vec3(offsetX, 0.0f, 0.0f));
    bgModel = scale(bgModel, vec3(relativeOffset, worldHeight, 1.0f));
    mat4 bgMVP = projection * view * bgModel;

    tileShader.set("u_MVP", bgMVP);
    tileShader.set("u_Time", time);
    tileShader.set("u_Static", true);
    tileShader.set("u_Texture", 0);
    tileShader.set("u_FlipX", false);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, assets->getTexture(StaticAssets::BACKGROUND).handle);
    mesh.draw();
    
    // --- Render Blocks ---
    tileShader.use();
    tileShader.set("colorLevels", retroShaderData.colorLevels);
    tileShader.set("noiseStrength", retroShaderData.noiseStrength);
    tileShader.set("noiseScale", retroShaderData.noiseScale);
    tileShader.set("scanlineStrength", retroShaderData.scanlineStrength);
    tileShader.set("scanlineFrequency", retroShaderData.scanlineFrequency);
    tileShader.set("u_Texture", 0);
    tileShader.set("u_Static", false);

    for (int y = 0; y < worldHeight; ++y) {
        for (int x = 0; x < worldWidth; ++x) {
            auto& [type, texture] = blocks[x][y];
            //if (type == BlockType::AIR) continue;

            auto worldPos = vec3(x, y, 0.01f);

            mat4 model = translate(mat4(1.0f), worldPos);
            mat4 mvp = projection * view * model;

            tileShader.set("u_MVP", mvp);
            tileShader.set("u_Time", time);

            GLuint texID = assets->getTexture(texture).handle;
            glBindTexture(GL_TEXTURE_2D, texID);
            mesh.draw();
        }
    }

    // --- Render Entities ---
    for (const auto& entity : entities) {
        tileShader.use();
        tileShader.set("u_Texture", 0);
        tileShader.set("u_FlipX", entity->getDirection());

        auto worldPos = vec3(entity->position - vec2(0.75, 0.0), 0.02f);
        mat4 model = translate(mat4(1.0f), worldPos);
        model = scale(model, vec3(1.5f));
        mat4 mvp = projection * view * model;

        tileShader.set("u_MVP", mvp);
        tileShader.set("u_Time", time);
        glBindTexture(GL_TEXTURE_2D, assets->getTexture(entity->getTexture()).handle);
        //entity->render(mvp, assets);
        mesh.draw();

        // ---- Debug ----
        if (!showHitboxes) continue;
        debugShader.use();
        debugShader.set("u_Color", vec4(1.0f, 0.0f, 0.0f, 0.8f));
        auto worldPos2 = vec3(entity->position - vec2(entity->getWidth() / 2.0f, 0.0), 0.03f);
        mat4 model2 = translate(mat4(1.0f), worldPos2);
        model2 = scale(model2, vec3(entity->getWidth(), entity->getHeight(), 1.0f));
        mat4 mvp2 = projection * view * model2;
        debugShader.set("u_MVP", mvp2);
        mesh.draw();
    }

    // --- Render HUD ---
    tileShader.use();
    auto hudPos = vec3(0.5f, 0.5f, 0.1f);
    mat4 model = translate(mat4(1.0f), hudPos);
    model = scale(model, vec3(2.5f));
    mat4 mvp = projection * view * model;

    tileShader.set("noiseStrength", retroShaderData.noiseStrength * 0.25f); // Less noise on HUD
    tileShader.set("u_MVP", mvp);
    tileShader.set("u_Time", time);
    tileShader.set("u_FlipX", false);

    GLuint texID = assets->getTexture(StaticAssets::HUD_SLOT).handle;
    glBindTexture(GL_TEXTURE_2D, texID);
    mesh.draw();

    if (player->selected != BlockType::AIR) {
        hudPos = vec3(1.0f, 1.0f, 0.11f);
        model = translate(mat4(1.0f), hudPos);
        model = scale(model, vec3(1.5f));
        mvp = projection * view * model;
        tileShader.set("noiseStrength", retroShaderData.noiseStrength);
        tileShader.set("u_MVP", mvp);

        texID = assets->getTexture(BlockStates::getTextureToFromType(player->selected)).handle;
        glBindTexture(GL_TEXTURE_2D, texID);
        mesh.draw();
    }

}

void GameManager::keyCallback(const Key key, const Action action, const Modifier modifier) {
    auto sKey = debugKeyToString(key);
    auto sAction = debugActionToString(action);
    auto sModifier = debugModToString(modifier);
    //printf("Key %s \tAction %s \tMod %s\n", sKey.c_str(), sAction.c_str(), sModifier.c_str());

    // Only captcha changes
    if (action == Action::REPEAT) return;

    switch (key) {
        case Key::D: player->isPressingRight = action == Action::PRESS; break;
        case Key::A: player->isPressingLeft = action == Action::PRESS; break;
        case Key::W: player->isPressingUp = action == Action::PRESS; break;
        case Key::S: player->isPressingDown = action == Action::PRESS; break;
        case Key::LEFT_CONTROL: player->isSprinting = action == Action::PRESS; break;
        case Key::SPACE: player->isJumping = action == Action::PRESS; break;

        case Key::Q: {
            // Mining
            if (action != Action::PRESS) return;
            const auto target = player->getTargetPosition();
            if (!BlockStates::isInBounds(target, blocks)) return;

            const auto targetType = blocks[target.x][target.y].type;
            if (!BlockStates::isSolid(targetType)) return; // prevent breaking air or water

            player->updateTexture(StaticAssets::PLAYER_MINE, 0.5f);
            audioPlayer.play("assets/sounds/break.wav", 0.5f);
            breakBlock(target);
            return;
        }

        case Key::E: {
            // Placing
            if (action != Action::PRESS) return;
            const auto target = player->getTargetPosition();
            if (!BlockStates::isInBounds(target, blocks)) return;

            if (player->selected == BlockType::AIR) return;
            const auto targetType = blocks[target.x][target.y].type;
            if (BlockStates::isSolid(targetType)) return; // prevent replacing solid blocks

            player->updateTexture(StaticAssets::PLAYER_MINE, 0.5f);
            audioPlayer.play("assets/sounds/break.wav", 0.5f);
            placeBlock(target, player->selected);
            return;
        }

        case Key::B: {
            if (action != Action::PRESS) return;
            showHitboxes = !showHitboxes;
            return;
        }

        default: break;
    }
}
} // arcader
