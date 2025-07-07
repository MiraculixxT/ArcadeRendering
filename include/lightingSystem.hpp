//
// Created by Tim Müller on 28.06.25.
//


#ifndef ARCADE_LIGHTINGSYSTEM_HPP
#define ARCADE_LIGHTINGSYSTEM_HPP

#include <glm/glm.hpp>
#include <framework/gl/program.hpp>
#include <vector>

namespace arcader {

    class LightingSystem {
    public:
        glm::vec3 lightColor = glm::vec3(1.0f);

        struct PointLight {
            glm::vec3 position;
            glm::vec3 color;
            float intensity;
            float radius;
        };

        void init(const glm::vec3& ambientColor, const glm::vec3& lightDir, const glm::vec3& lightColor);
        void update(const glm::vec3& newDir, const glm::vec3& newColor);
        void bindToShader(Program& shader) const;
        glm::vec3 getDirection() const;

        void addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius);
        const std::vector<PointLight>& getPointLights() const;
        void bindPointLightsToShader(Program& shader) const;
        void clearPointLights();
        void setPointLightIntensity(int index, float intensity);

    private:
        glm::vec3 ambientColor = glm::vec3(0.2f);
        glm::vec3 lightDirection = glm::vec3(-0.5f, -1.0f, -0.3f);
        std::vector<PointLight> pointLights;
    };

}

#endif //ARCADE_LIGHTINGSYSTEM_HPP
