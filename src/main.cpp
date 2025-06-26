#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <framework/app.hpp>
#include <framework/camera.hpp>
#include <framework/mesh.hpp>
#include <framework/imguiutil.hpp>
#include <framework/gl/program.hpp>


struct MainApp : public App {
    Program program;
    Mesh mesh;
    Camera camera;

    /**
     * State defines in what render loop we are at the moment.
     *
     * 0 - black screen (start)<br>
     * 1 - static camera, light flickers on<br>
     * 2 - moving camera to main machine<br>
     * 3 - static camera, rendering 2D game scene
     */
    int state = 0;
    
    MainApp() : App(600, 600) {
        // Loading models/meshes
        // mesh.load("meshes/<model_name>.obj")

        // Loading shaders (fragment & vertex)
        // program.load("shaders/<vertex_shader>.vsh", "shaders/<fragment_shader>.fsh")

        // Initialize camera position
        camera.worldPosition = vec3(0.0f, 0.0f, 0.0f);

        // OpenGL flags
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
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
    }

    /**
     * Handle window resizing
     * @param resolution new window resolution
     */
    void resizeCallback(const vec2& resolution) override {
        camera.resize(resolution.x / resolution.y);
    }

    void render() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera (movement or resolution change)
        camera.updateIfChanged();

        // Program setup with uniforms
        // program.use()
        // program.set(key, value)
        // program.draw()
    }

    void buildImGui() override {
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Reserved for later debugging");
        ImGui::SliderInt("State", &state, 0, 5);
        ImGui::End();
    }
};

int main() {
    MainApp app;
    app.run();
    return 0;
}