#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
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
#include <framework/gl/program.hpp>


struct MainApp : public App {

private:
    AssetManager assetManager;
    CinematicEngine cinematicEngine{&assetManager};
    GameManager gameManager{assetManager, cinematicEngine};

public:

    MainApp() : App(600, 600) {
        // OpenGL flags
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        if (key == Key::COMMA && action == Action::PRESS) App::imguiEnabled = !App::imguiEnabled;

        switch (cinematicEngine.getState()) {
            case 3: // game state
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
        ImGui::Text("Reserved for later debugging");
        int currentState = cinematicEngine.getState();
        ImGui::SliderInt("State", &currentState, 0, 5);

        if( ImGui::Button("Next State")) {
            cinematicEngine.setState(currentState+1);
        }

        if (ImGui::Button("Previous State")) {
            cinematicEngine.setState(currentState-1);
        }

        ImGui::End();
    }
};

int main() {
    MainApp app;
    app.run();
    return 0;
}