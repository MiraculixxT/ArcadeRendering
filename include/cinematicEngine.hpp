//
// Created by Tim Müller on 26.06.25.
//

#ifndef ARCADE_CINEMATICENGINE_HPP
#define ARCADE_CINEMATICENGINE_HPP

#include <framework/camera.hpp>
#include "assetManager.hpp"
#include <memory>
#include "lightingSystem.hpp"
#include "game/gameManager.hpp"
#include "dustParticles.hpp"
#include "audioPlayer.hpp"

/**
 * @brief Controls cinematic sequences including timed camera movement and rendering transitions.
 *
 * The CinematicEngine manages different scenes (e.g., intro, transitions, camera movements)
 * and utilizes the AssetManager to retrieve required assets such as models or textures.
 */
namespace arcader {
    class GameManager;

    class CinematicEngine {
    public:
        explicit CinematicEngine(AssetManager *assetManager, GameManager *gameManager);

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

        void initShadow();
        void renderShadowPass();

        const int windowWidth = 1280;
        const int windowHeight = 720;
        Mesh mesh;

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
        GameManager *game;
        Camera camera;
        LightingSystem lighting;
        DustParticles dustParticles;

        Program dustShader;
        GLuint dustTexture;

        AudioPlayer audioPlayer;

        GLuint depthMapFBO;
        GLuint depthMap;
        Program depthShader;
        glm::mat4 lightSpaceMatrix;
        const GLuint SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;
    };

}

#endif // ARCADE_CINEMATICENGINE_HPP