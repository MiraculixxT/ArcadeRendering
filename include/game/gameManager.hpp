//
// Created by julius on 6/26/25.
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "assetManager.hpp"
#include "block.hpp"
#include "entity.hpp"
#include "framework/app.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"

namespace arcader {
struct RetroShaderData {
    float colorLevels = 6.0f;
    float noiseStrength = 0.05f;
    float noiseScale = 3.47f;
    float scanlineStrength = 0.255f;
    float scanlineFrequency = 0.25f;
};

class GameManager {
    static constexpr int worldWidth = 32;
    static constexpr int worldHeight = 32;
    /**
     * World block grid with fix x and y size.
     */
    std::vector<std::vector<Block>> blocks;
    static constexpr int blockDimension = 16;

    std::vector<std::unique_ptr<Entity>> entities;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    AssetManager *assets;
    int *screenHeight;
    int *screenWidth;
    Program& tile_shader;
    Program& entity_shader;
    Program& debugShader;
    Program& hudShader;
    Mesh mesh;
    EntityPlayer* player = nullptr;

public:
    GameManager(AssetManager *assetsManager, int *height, int *width);

    // World generation data
    int seed = -1;
    float frequency = 0.03f;
    float terrainBase = 0.0f;
    float terrainPeak = 100.0f;
    float treeFrequency = 0.15f;
    int waterLevel = 7;
    bool showHitboxes = false;
    RetroShaderData retroShaderData;

    /**
     * Initializes the game world by loading mesh data.
     */
    void init();

    /**
     * Generate the block terrain based on a perlin noise algorithm with the help of the fast noise lite API.
     */
    void generateTerrain();

    /**
     * Generate trees on top of grass blocks randomly.
     */
    void generateTrees();

    /**
     * Place a block in the world and updating the surrounding if needed
     * @param pos block position
     * @param type the block type that is intended to be placed
     */
    void placeBlock(uvec2 pos, BlockType type);

    /**
     * Break a block in the world and updating the surrounding if needed.
     * @param pos block position
     */
    void breakBlock(uvec2 pos);

    [[nodiscard]] EntityPlayer* getPlayer() const { return player; };

    /**
     * Updates the game state.
     * @param deltaTime Time elapsed since the last update, used for time-based calculations.
     */
    void update(float deltaTime) const;

    void renderDebug(Camera &camera);

    /**
     * Renders the game world.
     * @param camera Reference to the `Camera` object for view transformations.
     */
    void render(Camera &camera);

    /**
     * Redirection for all key inputs for player interaction.
     * @param key pressed key
     * @param action press or release
     * @param modifier modifier keys (shift, ctrl, alt)
     */
    void keyCallback(Key key, Action action, Modifier modifier);

    static std::string debugKeyToString(const Key key) {
        switch (key) {
            case Key::W: return "W";
            case Key::D: return "D";
            case Key::LEFT: return "LEFT";
            case Key::RIGHT: return "RIGHT";
            case Key::E: return "E";

            default: return "Unknown";
        }
    }


    static std::string debugActionToString(const Action action) {
        switch (action) {
            case Action::RELEASE: return "RELEASE";
            case Action::PRESS: return "PRESS";
            case Action::REPEAT: return "REPEAT";
            default: return "UNDEFINED_ACTION";
        }
    }

    static std::string debugModToString(const Modifier modifier) {
        std::string result;
        result += "-";
        if (modifier >= Modifier::SHIFT) result += "SHIFT|";
        if (modifier >= Modifier::CTRL) result += "CTRL|";
        if (modifier >= Modifier::ALT) result += "ALT|";
        if (modifier >= Modifier::SUPER) result += "SUPER|";
        return result.substr(0, result.size() - 1); // Remove trailing '|'

    }
};
} // arcader

#endif //GAMEMANAGER_H
