//
// Created by julius on 6/26/25.
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "assetManager.hpp"
#include "cinematicEngine.hpp"
#include "block.hpp"
#include "entity.hpp"
#include "framework/camera.hpp"
#include "framework/gl/program.hpp"

namespace arcader {
class GameManager {
    static constexpr int worldWidth = 32;
    static constexpr int worldHeight = 32;
    /**
     * World block grid with fix x and y size.
     */
    Block blocks[worldWidth][worldHeight]{};
    static constexpr int blockDimension = 16;

    std::vector<Entity> entities;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    AssetManager &assets;
    CinematicEngine &renderer;

public:
    /**
     * Constructor for the `GameManager` class.
     * @param assetsManager Reference to the `AssetManager` instance.
     * @param cinematicEngine Reference to the `CinematicEngine` instance.
     */
    explicit GameManager(AssetManager &assetsManager, CinematicEngine &cinematicEngine) : assets(assetsManager), renderer(cinematicEngine) {};

    /**
     * Initializes the game world by loading mesh data.
     * @param mesh Reference to the `Mesh` object to be initialized.
     */
    void init(Mesh &mesh);

    /**
     * Updates the game state.
     * @param deltaTime Time elapsed since the last update, used for time-based calculations.
     */
    void update(float deltaTime);

    /**
     * Renders the game world.
     * @param camera Reference to the `Camera` object for view transformations.
     * @param program Reference to the shader `Program` used for rendering.
     * @param mesh Reference to the mesh object
     */
    void render(Camera &camera, Program &program, Mesh &mesh);
};
} // arcader

#endif //GAMEMANAGER_H
