//
// Created by Tim Müller on 26.06.25.
//

#include "cinematicEngine.hpp"
#include "lightingSystem.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


namespace arcader {

    CinematicEngine::CinematicEngine(AssetManager *assets, GameManager *gameManager)
            : state(0), timer(0.0f), assets(assets), camera(), game(gameManager) {

        lighting.init(
                glm::vec3(0.02f, 0.02f, 0.05f),             // ambientColor (dunkelblau)
                glm::vec3(-0.2f, -1.0f, -0.1f),             // lightDir (flach und schwach)
                glm::vec3(0.1f, 0.2f, 0.4f)                 // lightColor (kühles Blau)
        );

        //Skybox
        initSkybox();

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
            case 0: {
                lighting.update(glm::vec3(-0.0f, 1.0f, -0.0f), glm::vec3(0.1f, 0.15f, 0.25f)); // static light direction and color

                if (timer < 7.0f) {
                    // all lights off
                    for (int i = 0; i < lighting.getPointLights().size(); ++i) {
                        lighting.setPointLightIntensity(i, 0.0f);
                    }
                } else if (timer < 12.0f) {
                    // light flickering
                    for (int i = 0; i < lighting.getPointLights().size(); ++i) {
                        // random arcade, value between 0 and number of lights
                        float activationTime = 7.0f + i * 2.0f;
                        if (timer < activationTime) {
                            lighting.setPointLightIntensity(i, 0.0f);
                        } else if (timer < activationTime + 2.0f) {
                            float t = timer * 8.0f + i * 3.0f;
                            float flicker = (sin(t) > 0.6f) ? 2.5f :
                                            (sin(t * 0.7f) > 0.3f) ? 1.2f :
                                            (rand() % 100 < 10) ? 0.5f : 0.0f;
                            lighting.setPointLightIntensity(i, flicker);
                        } else {
                            lighting.setPointLightIntensity(i, 2.5f);
                        }
                    }

                    // if all lights are on for a while, switch to next state
                    if (timer > 7.0f + lighting.getPointLights().size() * 2.0f) {
                        setState(1);
                    }
                } else {
                    setState(1);
                }
            } break;
            case 1:
                for (int i = 0; i < lighting.getPointLights().size(); ++i) {
                    lighting.setPointLightIntensity(i, 2.5f);
                }
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

                renderArcade();

                break;
            case 2:

                game->render(camera, mesh);
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
        renderSkybox();

        camera.resize(static_cast<float>(windowWidth) / windowHeight);
        camera.worldPosition = {0.0f, 61.0f, 10.0f};
        camera.target = {0.0f, 60.0f, 0.0f};
        camera.update();

        // place point lights
        if (lighting.getPointLights().empty()) {
            int y = 2;
            for (int i = 0; i < 3; ++i) {
                int x = -4;
                for (int j = 0; j < 3; ++j) {
                    lighting.addPointLight(
                            glm::vec3(x, 63.0f, y),
                            glm::vec3(1.0f, 0.85f, 0.6f),
                            0.0f,
                            5.0f
                    );
                    x += 4;
                }
                y += 4;
            }
        }

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
                                "assets/textures/Arcade_Color.png"
                        }
                );
            }

            Program &arcadeShader = const_cast<Program &>(assets->getShader(ARCADE_MACHINE));
            lighting.bindToShader(arcadeShader);
            lighting.bindPointLightsToShader(arcadeShader);

            assets->render(
                    ARCADE_MACHINE,
                    camera.projectionMatrix * camera.viewMatrix,
                    glm::vec3(0.0f, 60.0f, 0.0f), // mittlere Maschine
                    glm::vec3(0.5f)
            );

            int x = 0;
            for (int i = 0; i < 3; ++i) {

                assets->render(
                        ARCADE_MACHINE,
                        camera.projectionMatrix * camera.viewMatrix,
                        glm::vec3(-2.0f, 60.0f, x), // linke Maschine
                        glm::vec3(0.5f)
                );

                assets->render(
                        ARCADE_MACHINE,
                        camera.projectionMatrix * camera.viewMatrix,
                        glm::vec3(2.0f, 60.0f, x), // rechte Maschine
                        glm::vec3(0.5f)
                );

                assets->render(
                        ARCADE_MACHINE,
                        camera.projectionMatrix * camera.viewMatrix,
                        glm::vec3(-4.0f, 60.0f, x), // linke Maschine
                        glm::vec3(0.5f)
                );

                assets->render(
                        ARCADE_MACHINE,
                        camera.projectionMatrix * camera.viewMatrix,
                        glm::vec3(4.0f, 60.0f, x), // rechte Maschine
                        glm::vec3(0.5f)
                );

                x+=4;
            }

            if (!assets->hasRenderable(ROOM)) {
                assets->loadRenderable(
                        ROOM,
                        "assets/meshes/newroom.obj",
                        "shaders/arcade.vsh",
                        "shaders/arcade.fsh",
                        {
                                "assets/textures/room_atlas.png",
                        }
                );
            }

            Program &roomShader = const_cast<Program &>(assets->getShader(ROOM));
            lighting.bindToShader(roomShader);
            lighting.bindPointLightsToShader(roomShader);
            assets->render(
                    ROOM,
                    camera.projectionMatrix * camera.viewMatrix,
                    glm::vec3(-5.0f, 60.0f, 11.0f), // Room center
                    glm::vec3(0.06f) // Scale
            );

        }
    }


    GLuint CinematicEngine::loadCubemap(const std::vector<std::string>& faces) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            } else {
                std::cerr << "Failed to load cubemap texture at: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    void CinematicEngine::initSkybox() {
        float skyboxVertices[] = {
                // Positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
        };

        std::vector<std::string> faces = {
                "assets/textures/sky/Box_Right.png",
                "assets/textures/sky/Box_Left.png",
                "assets/textures/sky/Box_Top.png",
                "assets/textures/sky/Box_Bottom.png",
                "assets/textures/sky/Box_Front.png",
                "assets/textures/sky/Box_Back.png"
        };

        GLuint skyboxVAO, skyboxVBO;
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        GLuint cubemapTexture;

        cubemapTexture = loadCubemap(faces);

        skyboxShader.load("shaders/skybox.vsh", "shaders/skybox.fsh");
        this->skyboxVAO = skyboxVAO;
        this->cubemapTexture = cubemapTexture;
    }

    void CinematicEngine::renderSkybox() {
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        glm::mat4 view = glm::mat4(glm::mat3(camera.viewMatrix));
        skyboxShader.set("uView", view);
        skyboxShader.set("uProjection", camera.projectionMatrix);

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }
}