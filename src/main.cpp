#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <random>
#include <glm/gtx/transform.hpp>

#include "game/gameManager.hpp"
using namespace glm;

#include <framework/app.hpp>
#include <framework/camera.hpp>
#include <framework/mesh.hpp>
#include <framework/imguiutil.hpp>

#include <iostream>

#include "cinematicEngine.hpp"
using namespace arcader;

struct MainApp : public App {
private:
    AssetManager assetManager;
    GameManager gameManager{&assetManager, &screenHeight, &screenWidth};
    CinematicEngine cinematicEngine{&assetManager, &gameManager};

    // --- post processing ---
    GLuint quadVAO, quadVBO;
    GLuint fbo, colorTexture;
    Program &postShader = assetManager.getShader(StaticAssets::SHADER_DEBUG);

public:
    int screenWidth = 1920;
    int screenHeight = 1080;

    MainApp() : App(1920, 1080) {
        // GLFW flags
        glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        // Fullscreen
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

        // OpenGL flags
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        //glDisable(GL_CULL_FACE); // Debugging

        //glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST); // Debugging

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Debugging

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // better decide between black (uncolored) squares and bg

        //
        // Create frame buffer for post-processing shader
        //
        constexpr float quadVertices[] = {
            // positions    // texCoords
            -1.0f,  1.0f,   0.0f, 1.0f,
            -1.0f, -1.0f,   0.0f, 0.0f,
             1.0f, -1.0f,   1.0f, 0.0f,

            -1.0f,  1.0f,   0.0f, 1.0f,
             1.0f, -1.0f,   1.0f, 0.0f,
             1.0f,  1.0f,   1.0f, 1.0f
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

        // Frame buffer
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // Color attachment
        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void changeState(const int offset) {
        cinematicEngine.setState(cinematicEngine.getState() + offset);
        printf("Switched to state: %d\n", cinematicEngine.getState());
        if (cinematicEngine.getState() == 2) {
            gameManager.init();
        }
    }

    /**
     * Handle keyboard & mouse button inputs
     * @param key pressed key
     * @param action press or release
     * @param modifier modifier keys (shift, ctrl, alt)
     */
    void keyCallback(Key key, Action action, Modifier modifier) override {
        if (action == Action::PRESS) {
            switch (key) {
                case Key::ESC: close();
                    break;
                case Key::COMMA: imguiEnabled = !imguiEnabled;
                    break;
                case Key::RIGHT: changeState(1);
                    break;
                case Key::LEFT: changeState(-1);
                    break;
            }
        }

        switch (cinematicEngine.getState()) {
            case 2: // game state
                gameManager.keyCallback(key, action, modifier);
                break;
            default: nullptr;
        }
    }

    void render() override {
        // Set frame buffer as render target
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calc time
        static float lastTime = 0.0f;
        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // --- Rendering redirection ---
        //cinematicEngine.update(deltaTime);
        //cinematicEngine.render();
        // --- Rendering redirection ---

        // Render frame buffer to screen with post shader
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        postShader.use();
        postShader.set("u_ScreenTexture", 0);
        postShader.set("u_Time", currentTime);
        postShader.set("u_State", cinematicEngine.getState());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTexture);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(quadVAO);
        glEnableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Error catching
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            std::cerr << "OpenGL Error: " << err << std::endl;
    }

    void buildImGui() override {
        ImGui::StatisticsWindow(delta, resolution);

        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        int currentState = cinematicEngine.getState();
        ImGui::SliderInt("State", &currentState, 0, 5);

        if (ImGui::Button("Next State")) {
            changeState(1);
        }

        if (ImGui::Button("Previous State")) {
            changeState(-1);
        }
        ImGui::End();

        if (gameManager.getPlayer()) { // Only show when game is initialized
            ImGui::Begin("Game Management", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if (ImGui::Button("Seed Randomize")) {
                std::random_device rd;
                gameManager.seed = rd();
                gameManager.generateTerrain();
                gameManager.generateTrees();
            }
            if (ImGui::SliderFloat("Frequency", &gameManager.frequency, 0.01f, 0.1f)) {
                gameManager.generateTerrain();
                gameManager.generateTrees();
            }
            if (ImGui::SliderFloat("Mod Base", &gameManager.terrainBase, 0.0f, 100.0f)) {
                gameManager.generateTerrain();
                gameManager.generateTrees();
            }
            if (ImGui::SliderFloat("Mod Peak", &gameManager.terrainPeak, 100.0f, 300.0f)) {
                gameManager.generateTerrain();
                gameManager.generateTrees();
            }
            if (ImGui::SliderFloat("Tree Frequency", &gameManager.treeFrequency, 0.01f, 1.0f)) {
                gameManager.generateTerrain();
                gameManager.generateTrees();
            }
            if (ImGui::SliderInt("Water Level", &gameManager.waterLevel, 0, 31)) {
                gameManager.generateTerrain();
                gameManager.generateTrees();
            }

            const vec2 playerPos = gameManager.getPlayer()->position;
            const vec2 playerVel = gameManager.getPlayer()->velocity;
            ImGui::Text("Pos: (%.2f, %.2f) - Vel: (%.2f, %.2f)", playerPos.x, playerPos.y, playerVel.x, playerVel.y);
            auto player = gameManager.getPlayer();
            ImGui::Text("Key: A:%d | D:%d | SPRNT: %d", player->isPressingLeft, player->isPressingRight, player->isSprinting);
            ImGui::End();
        }
    }
};

int main() {
    MainApp app;
    app.run();
    return 0;
}
