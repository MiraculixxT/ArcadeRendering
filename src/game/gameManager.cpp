//
// Created by julius on 6/26/25.
//

#include "game/gameManager.hpp"

#include "framework/mesh.hpp"
#include "game/block.hpp"

namespace arcader {
std::vector<float> vertices = {
    // positions    // tex coords
    0.0f, 0.0f,     0.0f, 0.0f,
    1.0f, 0.0f,     1.0f, 0.0f,
    1.0f, 1.0f,     1.0f, 1.0f,
    0.0f, 1.0f,     0.0f, 1.0f
};

std::vector<unsigned int> indices = {
    0, 1, 2,
    2, 3, 0
};

/**
   * Constructor for the `GameManager` class.
   * @param assetsManager Reference to the `AssetManager` instance.
   */
GameManager::GameManager(AssetManager *assetsManager, int height, int width) : assets(assetsManager), screenHeight(height), screenWidth(width),
                                                                               tile_shader(assetsManager->getShader(StaticAssets::SHADER_TILE)),
                                                                               entity_shader(assetsManager->getShader(StaticAssets::SHADER_ENTITY)),
                                                                               debug_shader(assetsManager->getShader(StaticAssets::SHADER_DEBUG)) {
};

void GameManager::init(Mesh &mesh) {
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
    mesh.load(vertices, indices);

    // Initilize blocks (temp, maybe doing procedural generation later)
    printf("  - Initializing blocks...\n");
    for (int i = 0; i < 32; ++i) {
        blocks[0][i] = {BlockType::STONE, StaticAssets::BLOCK_STONE};
        blocks[1][i] = {BlockType::STONE, StaticAssets::BLOCK_STONE};
        blocks[2][i] = {BlockType::DIRT, StaticAssets::BLOCK_DIRT};
        blocks[3][i] = {BlockType::GRASS, StaticAssets::BLOCK_GRASS};
    }

    // Initialize player
    printf("  - Initializing entities...\n");
    entities.clear();
    auto player = new EntityPlayer(vec2(0.0f, 6.0f), 32);
    auto pPlayer = std::unique_ptr<Entity>(player);
    entities.push_back(std::move(pPlayer));
}

void GameManager::update(float deltaTime) {
    // Update entities
    for (const auto &entity : entities) {
        entity->update(deltaTime);
    }
}

void GameManager::render(Camera &camera, Program &program, Mesh &mesh) {
    program.use();
    const mat4 projection = ortho(0.0f, static_cast<float>(renderer.windowWidth), 0.0f, static_cast<float>(renderer.windowHeight)); // 2D orthographic projection
    constexpr mat4 view = mat4(1.0f); // no view transformation, or set from camera
void GameManager::render(Camera &camera, Mesh &mesh) {
    tile_shader.use();

    // Get camera matrices
    const mat4& projection = camera.projectionMatrix;
    const mat4& view = camera.viewMatrix;

    // Define where the tilemap should appear in world space
    const vec3 tilemapOrigin = vec3(-5.0f, -5.0f, -1.0f);  // must be moved according to screen position (TODO)
    const float tileSize = blockDimension * 5.0f;

    // Render blocks
    for (int y = 0; y < worldHeight; ++y) {
        for (int x = 0; x < worldWidth; ++x) {
            auto &[type, texture] = blocks[x][y];
            if (type == BlockType::AIR) continue;

            // Position each tile in 3D world space
            vec3 worldPos = tilemapOrigin + vec3(x * tileSize, y * tileSize, 0.0f);
            mat4 model = translate(mat4(1.0f), worldPos);
            mat4 mvp = projection * view * model;

            tile_shader.set("u_MVP", mvp);
            tile_shader.set("u_Texture", 0); // Texture unit 0

            GLuint texID = assets->getTexture(texture).handle;
            glBindTexture(GL_TEXTURE_2D, texID);
            mesh.draw(); // draws the quad
        }
    }

    // Render entities (same idea — use camera matrices)
    entity_shader.use();
    for (const auto &entity : entities) {
        vec3 entityPos3D = tilemapOrigin + vec3(entity->position, 0.0f); // align entities with grid
        mat4 model = translate(mat4(1.0f), entityPos3D);
        model = scale(model, vec3(entity->getDimension(), entity->getDimension(), 1.0f));

        mat4 mvp = projection * view * model;

        entity_shader.set("u_MVP", mvp);
        GLuint texID = assets->getTexture(entity->getTexture()).handle;
        glBindTexture(GL_TEXTURE_2D, texID);
        entity->render(mvp, assets); // may pass model, view, projection separately
    }
}

void GameManager::keyCallback(Key key, Action action, Modifier modifier) {
    auto sKey = debugKeyToString(key);
    auto sAction = debugActionToString(action);
    auto sModifier = debugModToString(modifier);
    printf("Key %s \tAction %s \tMod %s\n", sKey.c_str(), sAction.c_str(), sModifier.c_str());
}
} // arcader
