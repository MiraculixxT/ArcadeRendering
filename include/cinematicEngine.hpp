//
// Created by Tim Müller on 26.06.25.
//

#ifndef ARCADE_CINEMATICENGINE_HPP
#define ARCADE_CINEMATICENGINE_HPP

#include <framework/camera.hpp>
#include "assetManager.hpp"
#include <memory>
#include "lightingSystem.hpp"

/**
 * @brief Controls cinematic sequences including timed camera movement and rendering transitions.
 *
 * The CinematicEngine manages different scenes (e.g., intro, transitions, camera movements)
 * and utilizes the AssetManager to retrieve required assets such as models or textures.
 */
namespace arcader {

    class CinematicEngine {
    public:
        explicit CinematicEngine(AssetManager *assetManager);

        void update(float deltaTime);

        void render();

        void reset();

        void nextState();

        void setState(int newState);

        int getState() const;

        void renderArcade();

        GLuint loadCubemap(const std::vector<std::string>& faces);
        void initSkybox();
        void renderSkybox();

        const int windowWidth = 600;
        const int windowHeight = 600;

    private:
        void updateScene(int state, float dt);

        void renderScene(int state);

        int state = 0;
        float timer = 0.0f;

        // Skybox rendering members
        Program skyboxShader;
        GLuint skyboxVAO;
        GLuint cubemapTexture;

        AssetManager *assets;
        Camera camera;
        LightingSystem lighting;
        Mesh mesh;

        // Optional: animation interpolation helpers
    };

}

#endif // ARCADE_CINEMATICENGINE_HPP