//
// Created by julius on 6/26/25.
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "assetManager.hpp"
#include "cinematicEngine.hpp"
#include "block.hpp"
#include "entity.hpp"
#include "framework/app.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"

namespace arcader {
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
    Mesh mesh;
    EntityPlayer* player;

public:
    GameManager(AssetManager *assetsManager, int *height, int *width);

    // World generation data
    int seed;
    float frequency;
    float terrainBase;
    float terrainPeak;
    float treeFrequency;
    int waterLevel = 7;
    bool showHitboxes = false;

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
     * @param x coord x
     * @param y coord y
     * @param type the block type that is intended to be placed
     */
    void placeBlock(int x, int y, BlockType type);

    EntityPlayer* getPlayer() const { return player; };

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

    static std::string debugKeyToString(Key key) {
        switch (key) {
            case Key::W: return "W";
            case Key::D: return "D";
            case Key::LEFT: return "LEFT";
            case Key::RIGHT: return "RIGHT";
            case Key::E: return "E";

            default: return "Unknown";
        }
    }


    static std::string debugActionToString(Action action) {
        switch (action) {
            case Action::RELEASE: return "RELEASE";
            case Action::PRESS: return "PRESS";
            case Action::REPEAT: return "REPEAT";
            default: return "UNDEFINED_ACTION";
        }
    }

    static std::string debugModToString(Modifier modifier) {
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
