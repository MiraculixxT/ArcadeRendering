#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <random>

#include "game/gameManager.hpp"
using namespace glm;

#include <framework/app.hpp>
#include <framework/imguiutil.hpp>

#include <iostream>

#include "cinematicEngine.hpp"
using namespace arcader;

struct MainApp final : App {
private:
    AssetManager assetManager;
    GameManager gameManager{&assetManager, &screenHeight, &screenWidth};
    CinematicEngine cinematicEngine{&assetManager, &gameManager};

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

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //glDisable(GL_DEPTH_TEST); // Debugging

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Debugging

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // better decide between black (uncolored) squares and bg
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
    void keyCallback(const Key key, const Action action, const Modifier modifier) override {
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
                default: break;
            }
        }

        switch (cinematicEngine.getState()) {
            case 2: // game state
                gameManager.keyCallback(key, action, modifier);
                break;
            default: break;
        }
    }

    void render() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Zeit berechnen
        static float lastTime = 0.0f;
        const auto currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update manuell aufrufen
        cinematicEngine.update(deltaTime);

        // Danach rendern
        cinematicEngine.render();
    }

    void buildImGui() override {
        ImGui::StatisticsWindow(delta, resolution);

        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Time: %.3f", glfwGetTime());

        int currentState = cinematicEngine.getState();
        ImGui::SliderInt("State", &currentState, 0, 5);

        if (ImGui::Button("Next State")) {
            changeState(1);
        }

        if (ImGui::Button("Previous State")) {
            changeState(-1);
        }
        ImGui::End();

        if (gameManager.getPlayer()) {
            // Only show when game is initialized
            ImGui::Begin("Game Management", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if (ImGui::Button("Seed Randomize")) {
                std::random_device rd;
                gameManager.seed = static_cast<int>(rd());
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
            const auto player = gameManager.getPlayer();
            ImGui::Text("Key: A:%d | D:%d | W:%d | S:%d | SPRT: %d | JMP: %d", player->isPressingLeft, player->isPressingRight,
                        player->isPressingUp, player->isPressingDown, player->isSprinting, player->isJumping);

            ImGui::BeginChild("Retro Shader Settings", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
            ImGui::Text("--- Retro Shader Settings ---");

            ImGui::SliderFloat("Color Levels", &gameManager.retroShaderData.colorLevels, 1.0f, 10.0f);
            ImGui::SliderFloat("Noise Strength", &gameManager.retroShaderData.noiseStrength, 0.0f, 0.3f);
            ImGui::SliderFloat("Noise Scale", &gameManager.retroShaderData.noiseScale, 1.0f, 20.0f);
            ImGui::SliderFloat("Scanline Strength", &gameManager.retroShaderData.scanlineStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Scanline Frequency", &gameManager.retroShaderData.scanlineFrequency, 0.0f, 1.0f);

            ImGui::EndChild();
            ImGui::End();
        }
    }
};

int main() {
    MainApp app;
    app.run();
    return 0;
}
