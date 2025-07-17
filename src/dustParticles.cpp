//
// Created by Tim Müller on 12.07.25.
//

#include "dustParticles.hpp"
#include <random>

void DustParticles::init(size_t count, float spread) {
    numParticles = count;

    particles.clear();

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-spread, spread);
    std::uniform_real_distribution<float> distY(50.0f, 70.0f);
    for (size_t i = 0; i < count; ++i) {
        glm::vec3 pos(dist(rng), distY(rng), dist(rng)); // random position in spread area
        glm::vec3 vel(
            0.01f * dist(rng),                     // slight drift horizontal
            -0.01f * dist(rng),                    // slight drift vertical
            0.01f * dist(rng)                      // slight drift depth
        );
        particles.push_back({ pos, vel });
    }

    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::vec3), particles.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); // Positionen
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(0, 1);

    glBindVertexArray(0);
}

void DustParticles::render() {
    glBindVertexArray(vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particles.size());
    glBindVertexArray(0);
}

void DustParticles::update(float dt) {
    // Update particle positions based on their velocities
    for (auto& p : particles) {
        p.position += p.velocity * dt;
        if (p.position.y < 50.0f) {
            p.position.y = 70.0f;
        }
    }

    // Update VBO with new positions
    std::vector<glm::vec3> gpuPositions;
    gpuPositions.reserve(particles.size());
    for (const auto& p : particles) {
        gpuPositions.push_back(p.position);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gpuPositions.size() * sizeof(glm::vec3), gpuPositions.data());
}
