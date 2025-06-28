//
// Created by Tim Müller on 28.06.25.
//


#ifndef ARCADE_LIGHTINGSYSTEM_HPP
#define ARCADE_LIGHTINGSYSTEM_HPP

#include <glm/glm.hpp>
#include <framework/gl/program.hpp>

namespace arcader {

    class LightingSystem {
    public:
        glm::vec3 lightColor = glm::vec3(1.0f);

        void init(const glm::vec3& ambientColor, const glm::vec3& lightDir, const glm::vec3& lightColor);
        void update(const glm::vec3& newDir, const glm::vec3& newColor);
        void bindToShader(Program& shader) const;
        glm::vec3 getDirection() const;

    private:
        glm::vec3 ambientColor = glm::vec3(0.2f);
        glm::vec3 lightDirection = glm::vec3(-0.5f, -1.0f, -0.3f);
    };

}

#endif //ARCADE_LIGHTINGSYSTEM_HPP
