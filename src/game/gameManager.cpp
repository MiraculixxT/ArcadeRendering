//
// Created by julius on 6/26/25.
//

#include "game/gameManager.hpp"

#include "framework/mesh.hpp"
#include "game/states.hpp"

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

void GameManager::init(Mesh &mesh) const {
    mesh.load(vertices, indices);

}

void GameManager::update(float deltaTime) {
    // Update game logic here
}

void GameManager::render(Camera &camera, Program &program, Mesh &mesh) {
    program.use();
    const mat4 projection = ortho(0.0f, static_cast<float>(renderer.windowWidth), 0.0f, static_cast<float>(renderer.windowHeight)); // 2D orthographic projection
    constexpr mat4 view = mat4(1.0f); // no view transformation, or set from camera
    mat4 VP = projection * view;

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
}
} // arcader
