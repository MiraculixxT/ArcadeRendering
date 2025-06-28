//
// Created by Tim Müller on 26.06.25.
//


#include "cinematicEngine.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


namespace arcader {

    CinematicEngine::CinematicEngine(AssetManager *assets)
            : state(0), timer(0.0f), assets(assets), camera() {
    }

    void CinematicEngine::update(float deltaTime) {
        timer += deltaTime;
        updateScene(state, deltaTime);
    }

    void CinematicEngine::render() {
        renderScene(state);
    }

    void CinematicEngine::reset() {
        state = 0;
        timer = 0.0f;
    }

    void CinematicEngine::nextState() {
        state++;
        timer = 0.0f;
    }

    void CinematicEngine::setState(int newState) {
        state = newState;
        timer = 0.0f;
    }

    int CinematicEngine::getState() const {
        return state;
    }

    void CinematicEngine::updateScene(int state, float dt) {
        // Implement scene-specific updates here
        switch (state) {
            case 0:
                // Simple initialization to test rendering
                std::cout << "Scene 0: Starting cinematic engine..." << std::endl;
                break;
            case 1:
                // Update for static camera with light flickering
                break;
            case 2:
                // Update for moving camera to main machine
                break;
            case 3:
                // Update for static camera with 2D game scene
                break;
            default:
                // Handle other states or default updates
                break;
        }
    }

    void CinematicEngine::renderScene(int state) {
        // Implement scene-specific rendering here
        switch (state) {
            case 0:
                camera.resize(1.0f); // Prevent division by zero
                camera.worldPosition = {0.0f, 3.0f, 3.0f};
                camera.target = {0.0f, 0.0f, 0.0f};
                camera.update();

                if (assets) {
                    using enum StaticAssets;

                    if (!assets->hasRenderable(ARCADE_MACHINE)) {
                        assets->loadRenderable(
                            ARCADE_MACHINE,
                            "assets/meshes/arcade.obj",
                            "shaders/arcade.vsh",
                            "shaders/arcade.fsh",
                            {
                                "assets/textures/Arcade_Color.png",
                            }
                        );
                    }

                    assets->render(
                            ARCADE_MACHINE,
                            camera.projectionMatrix * camera.viewMatrix,
                            glm::vec3(-1.5f, -1.0f, 0.0f), // linke Maschine
                            glm::vec3(0.5f)
                    );

                    assets->render(
                            ARCADE_MACHINE,
                            camera.projectionMatrix * camera.viewMatrix,
                            glm::vec3(0.0f, -1.0f, 0.0f), // mittlere Maschine
                            glm::vec3(0.5f)
                    );

                    assets->render(
                            ARCADE_MACHINE,
                            camera.projectionMatrix * camera.viewMatrix,
                            glm::vec3(1.5f, -1.0f, 0.0f), // rechte Maschine
                            glm::vec3(0.5f)
                    );
                }
                break;
            case 1:
                // Render static camera with light flickering
                break;
            case 2:
                // Render moving camera to main machine
                break;
            case 3:
                // Render static camera with 2D game scene
                break;
            default:
                // Handle other states or default rendering
                break;
        }
    }
}