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

void LightingSystem::addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius) {
    pointLights.push_back({ position, color, intensity, radius });
}

const std::vector<LightingSystem::PointLight>& LightingSystem::getPointLights() const {
    return pointLights;
}

void LightingSystem::bindPointLightsToShader(Program& shader) const {
    shader.set("uNumPointLights", static_cast<int>(pointLights.size()));
    for (size_t i = 0; i < pointLights.size(); ++i) {
        const auto& light = pointLights[i];
        std::string index = std::to_string(i);
        shader.set("uPointLights[" + index + "].position", light.position);
        shader.set("uPointLights[" + index + "].color", light.color);
        shader.set("uPointLights[" + index + "].intensity", light.intensity);
        shader.set("uPointLights[" + index + "].radius", light.radius);
    }
}

void LightingSystem::clearPointLights() {
    pointLights.clear();
}

void LightingSystem::setPointLightIntensity(int index, float intensity) {
    if (index >= 0 && index < pointLights.size()) {
        pointLights[index].intensity = intensity;
    }
}

glm::vec3 LightingSystem::getDirection() const {
    return lightDirection;
}