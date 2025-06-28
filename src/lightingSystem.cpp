//
// Created by Tim Müller on 28.06.25.
//

// Implementation of LightingSystem class
#include "lightingSystem.hpp"

using namespace arcader;

void LightingSystem::init(const glm::vec3& ambientColor, const glm::vec3& lightDir, const glm::vec3& lightColor) {
    this->ambientColor = ambientColor;
    this->lightDirection = glm::normalize(lightDir);
    this->lightColor = lightColor;
}

void LightingSystem::update(const glm::vec3& newDir, const glm::vec3& newColor) {
    this->lightDirection = glm::normalize(newDir);
    this->lightColor = newColor;
}

void LightingSystem::bindToShader(Program& shader) const {
    shader.set("uAmbientColor", ambientColor);
    shader.set("uLightDirection", lightDirection);
    shader.set("uLightColor", lightColor);

}

glm::vec3 LightingSystem::getDirection() const {
    return lightDirection;
}