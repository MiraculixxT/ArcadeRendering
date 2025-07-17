//
// Created by Tim Müller on 26.06.25.
//

#include "cinematicEngine.hpp"
#include "lightingSystem.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <random>
#include <ctime>


namespace arcader {

    CinematicEngine::CinematicEngine(AssetManager *assets, GameManager *gameManager)
            : state(0), timer(0.0f), assets(assets), camera(), game(gameManager) {

        lighting.init(
                glm::vec3(0.02f, 0.03f, 0.05f),             // ambientColor
                glm::vec3(0.0f, -1.0f, -1.0f),               // lightDir (toward camera)
                glm::vec3(0.1f, 0.2f, 0.4f)                 // lightColor
        );

        //Skybox
        initSkybox();

        // Shadow
        initShadow();

        // init audio player
        audioPlayer.init();

        // init camera
        camera.resize(static_cast<float>(windowWidth) / windowHeight);
        camera.worldPosition = {0.0f, 62.0f, 10.0f};
        camera.target = {0.0f, 60.0f, 0.0f};
        camera.update();

        // Particles
        dustShader.load("shaders/dust.vsh", "shaders/dust.fsh");
        // Dummy white texture for dust particles
        GLuint whiteTex;
        glGenTextures(1, &whiteTex);
        glBindTexture(GL_TEXTURE_2D, whiteTex);
        unsigned char whitePixel[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        this->dustTexture = whiteTex;
        dustParticles.init(1000, 30.0f);

    }

    void CinematicEngine::update(float deltaTime) {
        timer += deltaTime;
        updateScene(state, deltaTime);

        // Play background sound if not already playing
        if(!audioPlayer.isPlaying("assets/sounds/emptyroom.wav")) {
            printf("Playing room sound\n");
            audioPlayer.play("assets/sounds/emptyroom.wav", 0.2f);
        }
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
                dustParticles.update(dt);
                lighting.update(glm::vec3(0.0f, -1.0f, -1.0f), glm::vec3(0.1f, 0.15f, 0.25f));

                // Simulate slow camera movement with staged light flicker after 15 seconds
                if (timer < 15.0f) {
                    float t = timer;
                    float z = 10.0f - t * 0.2f; // slow movement forward
                    float y = 62.5f + sin(t * 3.0f) * 0.05f; // vertical bobbing
                    glm::vec3 camPos = glm::vec3(0.0f, y, z);
                    glm::vec3 lookAt = glm::vec3(0.0f, 60.0f, 0.0f);

                    static float prevT = -1.0f;
                    if (prevT >= 0.0f) {
                        float prevSin = sin(prevT * 3.0f);
                        float currSin = sin(t * 3.0f);

                        // play footstep sound when stepping up
                        if (prevSin < currSin && sin(t * 3.0f + 0.016f) < currSin) {
                            audioPlayer.play("assets/sounds/footstep.wav", 1.5f);
                        }
                    }
                    prevT = t;

                    if (timer >= 11.0f && timer < 15.0f) {
                        float t = timer - 11.0f;
                        // smooth in-out over full 4 seconds
                        float s = sin(t * glm::pi<float>() / 4.0f); // smooth in-out over full 4 seconds
                        lookAt.x = sin(s * glm::pi<float>()) * 3.0f;
                    }

                    camera.worldPosition = camPos;
                    camera.target = lookAt;
                    camera.update();

                    for (int i = 0; i < lighting.getPointLights().size(); ++i) {
                        lighting.setPointLightIntensity(i, 0.0f);
                    }
                } else if (timer < 20.0f) {

                    glm::vec3 lookAt = glm::vec3(0.0f, 60.0f, 0.0f);

                    if (timer >= 15.0f && timer < 20.0f) {
                        float t = (timer - 15.0f) / 5.0f; // from 0 to 1 over 5 seconds
                        lookAt.x = sin(t * glm::pi<float>()) * -3.0f; // smooth sinusoidal motion to the left and back
                    }

                    camera.target = lookAt;
                    camera.update();

                    if(!audioPlayer.isPlaying("assets/sounds/lightflicker.wav")) {
                        printf("Playing light flicker sound\n");
                        audioPlayer.play("assets/sounds/lightflicker.wav", 0.5f);
                    }

                    // Flickering lights
                    for (int i = 0; i < 3; ++i) {
                        float activationTime = 15.0f + i * 2.0f;
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

                    if (timer > 30.0f + lighting.getPointLights().size() * 2.0f) {
                        setState(1);
                    }
                } else {
                    setState(1);
                }

            } break;
            case 1: {
                dustParticles.update(dt);

                // Camera movement: similar to state 0, but move in faster and bob
                if(camera.worldPosition.z > 1.5f) {
                    float t = timer;
                    float z = 7.0f - t * 0.7f; // move faster than in state 0
                    float y = 62.5f + sin(t * 3.0f) * 0.05f;
                    glm::vec3 camPos = glm::vec3(0.0f, y, z);

                    float baseLookY = 60.0f + (2.0f * t / 8.0f); // prevent looking too low
                    glm::vec3 lookAt = glm::vec3(0.0f, baseLookY, 0.0f);

                    static float prevT = -1.0f;
                    if (prevT >= 0.0f) {
                        float prevSin = sin(prevT * 3.0f);
                        float currSin = sin(t * 3.0f);
                        // play footstep sound when stepping up
                        if (prevSin < currSin && sin(t * 3.0f + 0.016f) < currSin) {
                            audioPlayer.play("assets/sounds/footstep.wav", 1.5f);
                        }
                    }
                    prevT = t;

                    camera.worldPosition = camPos;
                    camera.target = lookAt;
                    camera.update();
                }

                // All point lights are on
                for (int i = 0; i < 3; ++i) {
                    lighting.setPointLightIntensity(i, 2.5f);
                }

                // Insert Coin
                if (timer >=9.0f && timer < 10.0f) {
                    audioPlayer.play("assets/sounds/coin.wav", 0.5f);
                }

                if (timer > 10.0f) {
                    setState(2);
                    if(!audioPlayer.isPlaying("assets/sounds/activate.wav")) {
                        printf("Playing arcade sound\n");
                        audioPlayer.play("assets/sounds/activate.wav", 0.5f);
                    }
                    game->init();
                }

            } break;
            case 2:
                game->update(dt);
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
                game->render(camera);
                //game->renderDebug(camera);
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

        renderShadowPass();

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
            using enum StaticAssets;

            std::vector<StaticAssets> machines = {
                ARCADE_MACHINE,
                ARCADE_MACHINE_2,
                ARCADE_MACHINE_3,
                ARCADE_MACHINE_4,
                ARCADE_MACHINE_5
            };

            // Load arcade machines if not already loaded and bind shaders and lighting

            int z = 1;
            for(const auto &machine : machines) {
                if (!assets->hasRenderable(machine)) {
                    std::string texturePath = "assets/textures/Arcade_Color" + std::to_string(z) + ".png";
                    assets->loadRenderable(
                        machine,
                        "assets/meshes/arcade.obj",
                        "shaders/arcade.vsh",
                        "shaders/arcade.fsh",
                        {
                            texturePath
                        }
                    );
                    z += 1;
                }
                Program &arcadeShader = const_cast<Program &>(assets->getShader(machine));
                lighting.bindToShader(arcadeShader);
                lighting.bindPointLightsToShader(arcadeShader);
                arcadeShader.use();
                arcadeShader.set("uShadowMap", 1);
                arcadeShader.set("uLightSpaceMatrix", lightSpaceMatrix);
            }

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);

            assets->render(
                ARCADE_MACHINE,
                camera.projectionMatrix * camera.viewMatrix,
                glm::vec3(0.0f, 60.0f, 0.0f), // mittlere Maschine
                glm::vec3(0.5f)
            );

            // Arcade machine arrangement shuffle-once per row
            static std::vector<std::vector<StaticAssets>> shuffledRows;
            static bool initialized = false;
            if (!initialized) {
                for (int i = 0; i < 3; ++i) {
                    std::vector<StaticAssets> row = {
                        ARCADE_MACHINE_2,
                        ARCADE_MACHINE_3,
                        ARCADE_MACHINE_4,
                        ARCADE_MACHINE_5
                    };
                    std::shuffle(row.begin(), row.end(), std::default_random_engine(static_cast<unsigned>(time(0) + i)));
                    shuffledRows.push_back(row);
                }
                initialized = true;
            }

            int x = 0;
            for (int i = 0; i < 3; ++i) {
                const auto &rowMachines = shuffledRows[i];
                assets->render(rowMachines[0], camera.projectionMatrix * camera.viewMatrix, glm::vec3(-2.0f, 60.0f, x), glm::vec3(0.5f));
                assets->render(rowMachines[1], camera.projectionMatrix * camera.viewMatrix, glm::vec3(2.0f, 60.0f, x), glm::vec3(0.5f));
                assets->render(rowMachines[2], camera.projectionMatrix * camera.viewMatrix, glm::vec3(-4.0f, 60.0f, x), glm::vec3(0.5f));
                assets->render(rowMachines[3], camera.projectionMatrix * camera.viewMatrix, glm::vec3(4.0f, 60.0f, x), glm::vec3(0.5f));
                x += 4;
            }


            // Render the room
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

            roomShader.use();
            roomShader.set("uShadowMap", 1);
            roomShader.set("uLightSpaceMatrix", lightSpaceMatrix);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);

            assets->render(
                ROOM,
                camera.projectionMatrix * camera.viewMatrix,
                glm::vec3(-5.0f, 60.0f, 11.0f), // Room center
                glm::vec3(0.06f) // Scale
            );

            // Render dust particles
            dustShader.use();
            dustShader.set("uView", camera.viewMatrix);
            dustShader.set("uProj", camera.projectionMatrix);
            dustShader.set("uCameraRight", glm::vec3(camera.viewMatrix[0]));
            dustShader.set("uCameraUp", glm::vec3(camera.viewMatrix[1]));
            dustShader.set("uSize", 0.25f);

            // Enable blending for dust particles
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, dustTexture);
            dustShader.set("uTex", 0);
            dustShader.set("uSize", 0.05f);
            dustShader.set("uAlpha", 0.1f);
            dustParticles.render();

            glEnable(GL_DEPTH_TEST);
        }
    }

    void CinematicEngine::renderShadowPass() {
        // Save current viewport
        GLint prevViewport[4];
        glGetIntegerv(GL_VIEWPORT, prevViewport);
        glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f));

        glm::mat4 lightProjection = glm::ortho(-50.f, 50.f, -50.f, 50.f, 1.0f, 150.f);
        glm::mat4 lightView = glm::lookAt(-lightDir * 50.0f, glm::vec3(0.0f), glm::vec3(0, 1, 0));
        lightSpaceMatrix = lightProjection * lightView;

        // Set viewport to shadow map size
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        depthShader.use();
        depthShader.set("uLightSpaceMatrix", lightSpaceMatrix);

        using enum StaticAssets;

        // Render all arcade machines in shadow pass
        int x = 4;
        for (int i = 1; i < 3; ++i) {

            assets->render(
                    ARCADE_MACHINE,
                    lightSpaceMatrix,
                    glm::vec3(-2.0f, 60.0f, x),
                    glm::vec3(0.5f)
            );

            assets->render(
                    ARCADE_MACHINE,
                    lightSpaceMatrix,
                    glm::vec3(2.0f, 60.0f, x),
                    glm::vec3(0.5f)
            );

            assets->render(
                    ARCADE_MACHINE,
                    lightSpaceMatrix,
                    glm::vec3(-4.0f, 60.0f, x),
                    glm::vec3(0.5f)
            );

            assets->render(
                    ARCADE_MACHINE,
                    lightSpaceMatrix,
                    glm::vec3(4.0f, 60.0f, x),
                    glm::vec3(0.5f)
            );

            x+=4;
        }

        /*if (assets->hasRenderable(ROOM)) {
            assets->render(
                    ROOM,
                    lightSpaceMatrix,
                    glm::vec3(-5.0f, 60.0f, 11.0f), // Room center
                    glm::vec3(0.06f) // Scale
            );
        }
        */

        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Restore previous viewport
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
    }

    void CinematicEngine::initShadow() {
        GLint prevViewport[4];
        glGetIntegerv(GL_VIEWPORT, prevViewport);

        // Shadow map setup
        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

        depthShader.load("shaders/depth.vsh", "shaders/depth.fsh");
    }

    GLuint CinematicEngine::loadCubemap(const std::vector<std::string>& faces) {
        // Load cubemap textures
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        // Load each face of the cubemap
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
        // Set texture parameters
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

        // Generate and bind the skybox VAO and VBO
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