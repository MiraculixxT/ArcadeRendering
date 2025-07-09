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

public:
    int screenWidth = 1280;
    int screenHeight = 720;

    MainApp() : App(1280, 720) {
        // GLFW flags
        glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);

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

    /**
     * Handle keyboard & mouse button inputs
     * @param key pressed key
     * @param action press or release
     * @param modifier modifier keys (shift, ctrl, alt)
     */
    void keyCallback(Key key, Action action, Modifier modifier) override {
        if (key == Key::ESC && action == Action::PRESS) App::close();
        // Toggle GUI with COMMA
        if (key == Key::COMMA && action == Action::PRESS) {
            App::imguiEnabled = !App::imguiEnabled;
        };

        switch (cinematicEngine.getState()) {
            case 2: // game state
                gameManager.keyCallback(key, action, modifier);
                break;
            default: nullptr;
        }
    }

    void render() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Zeit berechnen
        static float lastTime = 0.0f;
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update manuell aufrufen
        cinematicEngine.update(deltaTime);

        // Danach rendern
        cinematicEngine.render();
    }

    void buildImGui() override {
        ImGui::StatisticsWindow(delta, resolution);

        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        int currentState = cinematicEngine.getState();
        ImGui::SliderInt("State", &currentState, 0, 5);

        if( ImGui::Button("Next State")) {
            cinematicEngine.setState(currentState+1);
            printf("Switched to state: %d\n", cinematicEngine.getState());
            if (cinematicEngine.getState() == 2) {
                gameManager.init();
            }
        }

        if (ImGui::Button("Previous State")) {
            cinematicEngine.setState(currentState-1);
        }
        ImGui::End();

        ImGui::Begin("World Generation", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::Button("Seed Randomize")) {
            std::random_device rd;
            gameManager.seed = rd();
            gameManager.generateTerrain();
        }
        if (ImGui::SliderFloat("Frequency", &gameManager.frequency, 0.01f, 0.1f)) {
            gameManager.generateTerrain();
        }
        if (ImGui::SliderFloat("Mod Base", &gameManager.terrainBase, 0.0f, 100.0f)) {
            gameManager.generateTerrain();
        }
        if (ImGui::SliderFloat("Mod Peak", &gameManager.terrainPeak, 100.0f, 300.0f)) {
            gameManager.generateTerrain();
        }
        if (ImGui::SliderFloat("Tree Frequency", &gameManager.treeFrequency, 0.01f, 1.0f)) {
            gameManager.generateTerrain();
            gameManager.generateTrees();
        }
        ImGui::End();
    }
};

int main() {
    MainApp app;
    app.run();
    return 0;
}