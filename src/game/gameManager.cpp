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

void GameManager::render(Camera &camera, Program &program) {

}
} // arcader
