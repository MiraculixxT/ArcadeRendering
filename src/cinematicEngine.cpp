//
// Created by Tim Müller on 26.06.25.
//

#include "cinematicEngine.hpp"
#include "lightingSystem.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


namespace arcader {

    CinematicEngine::CinematicEngine(AssetManager *assets)
            : state(0), timer(0.0f), assets(assets), camera() {

        lighting.init(
                glm::vec3(0.2f),               // ambientColor
                glm::vec3(-0.5f, -1.0f, -0.3f),// lightDir
                glm::vec3(1.0f)                // lightColor
        );
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

    GameManager* CinematicEngine::getGameManager() {
        return gameManager;
    }

    void CinematicEngine::updateScene(int state, float dt) {
        // Implement scene-specific updates here
        switch (state) {
            case 0:
            {
                if (timer < 5.0f) {
                    // Light is off
                    lighting.update(glm::vec3(-0.0f, 1.0f, -0.0f), glm::vec3(0.0f));
                } else if (timer < 12.0f) {
                    // Flickering light (unstable)
                    float flicker = static_cast<float>((rand() % 100) < 50); // Randomly on/off
                    float intensity = flicker * (0.4f + 0.6f * static_cast<float>(rand()) / RAND_MAX); // random 0.4 to 1.0
                    glm::vec3 lightDir = glm::normalize(glm::vec3(cos(timer * 2.0f), -1.0f, sin(timer * 2.0f)));
                    lighting.update(glm::vec3(-0.0f, 1.0f, -0.0f), glm::vec3(intensity));
                } else {
                    // Transition to next state
                    setState(1);

                }
            }
            break;
            case 1:
                // light flickering stops and camera starts moving
                break;
            case 2:
                // ingame
                break;
            case 3:
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
                renderArcade();
                break;
            case 1:
                // Render static camera with light flickering
                lighting.update(glm::vec3(-0.0f, 1.0f, 0.0f), glm::vec3(1.0f));

                renderArcade();

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

    void CinematicEngine::renderArcade() {
        camera.resize(1.0f); // Prevent division by zero
        camera.worldPosition = {0.0f, 3.0f, 3.0f};
        camera.target = {0.0f, 0.0f, 0.0f};
        camera.update();

        if (assets) {
            using
            enum StaticAssets;

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

            Program &arcadeShader = const_cast<Program &>(assets->getShader(ARCADE_MACHINE));
            lighting.bindToShader(arcadeShader);

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
    }
}