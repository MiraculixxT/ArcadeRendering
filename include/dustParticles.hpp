//
// Created by Tim Müller on 12.07.25.
//

#ifndef ARCADE_DUSTPARTICLES_HPP
#define ARCADE_DUSTPARTICLES_HPP

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

class DustParticles {
public:
    void init(size_t count, float spread = 30.0f);
    void render();
    void update(float dt);

    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
    };

private:
    std::vector<Particle> particles;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint quadVBO = 0;
    size_t numParticles = 0;
};

#endif //ARCADE_DUSTPARTICLES_HPP
