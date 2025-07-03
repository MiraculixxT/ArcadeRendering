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

void GameManager::init(Mesh &mesh) {
    mesh.load(vertices, indices);

    // Initilize blocks (temp, maybe doing procedural generation later)
    for (int i = 0; i < 32; ++i) {
        blocks[0][i] = {BlockType::STONE, StaticAssets::BLOCK_STONE};
        blocks[1][i] = {BlockType::STONE, StaticAssets::BLOCK_STONE};
        blocks[2][i] = {BlockType::DIRT, StaticAssets::BLOCK_DIRT};
        blocks[3][i] = {BlockType::GRASS, StaticAssets::BLOCK_GRASS};
    }

    // Initialize player
    entities.clear();
    auto player = new EntityPlayer(vec2(0.0f, 6.0f), 32);
    entities.push_back(*player);
}

void GameManager::update(float deltaTime) {
    // Update entities
    for (auto &entity : entities) {
        entity.update(deltaTime);
    }

}

void GameManager::render(Camera &camera, Program &program, Mesh &mesh) {
    program.use();
    const mat4 projection = ortho(0.0f, static_cast<float>(renderer.windowWidth), 0.0f, static_cast<float>(renderer.windowHeight)); // 2D orthographic projection
    constexpr mat4 view = mat4(1.0f); // no view transformation, or set from camera

    // Render blocks
    for (int y = 0; y < worldHeight; ++y) {
        for (int x = 0; x < worldWidth; ++x) {
            auto &[type, texture] = blocks[x][y];
            if (type == BlockType::AIR) continue; // Skipping air for now

            vec3 worldPos = vec3(x * blockDimension, y * blockDimension, 0.0f);
            mat4 model = translate(mat4(1.0f), worldPos);
            mat4 mvp = projection * view * model;

            program.set("u_MVP", mvp);
            GLuint texID = assets.getTexture(texture).handle;
            glBindTexture(GL_TEXTURE_2D, texID);
            mesh.draw(); // draws the quad
        }
    }

    // Render entities
    for (const auto &entity : entities) {
        mat4 model = translate(mat4(1.0f), vec3(entity.position, 0.0f));
        model = scale(model, vec3(entity.getDimension(), entity.getDimension(), 1.0f));
        mat4 mvp = projection * view * model;

        program.set("u_MVP", mvp);
        GLuint texID = assets.getTexture(entity.getTexture()).handle;
        glBindTexture(GL_TEXTURE_2D, texID);
        entity.render(mvp, assets); // Render the entity
    }
}
} // arcader
