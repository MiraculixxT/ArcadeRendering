//
// Created by Tim Müller on 26.06.25.
//

#ifndef ARCADE_CINEMATICENGINE_HPP
#define ARCADE_CINEMATICENGINE_HPP

#include <framework/camera.hpp>
#include "assetManager.hpp"
#include <memory>

/**
 * @brief Controls cinematic sequences including timed camera movement and rendering transitions.
 *
 * The CinematicEngine manages different scenes (e.g., intro, transitions, camera movements)
 * and utilizes the AssetManager to retrieve required assets such as models or textures.
 */
namespace arcader {

    class CinematicEngine {
    public:
        explicit CinematicEngine(assetManager *assetManager);

        void update(float deltaTime);

        void render();

        void reset();

        void nextState();

        void setState(int newState);

        int getState() const;

    private:
        void updateScene0(float dt);

        void updateScene1(float dt);

        void updateScene2(float dt);

        void updateScene3(float dt);

        void renderScene0();

        void renderScene1();

        void renderScene2();

        void renderScene3();

        int state = 0;
        float timer = 0.0f;

        assetManager *assets;
        Camera camera;

        // Optional: animation interpolation helpers
    };

}

#endif // ARCADE_CINEMATICENGINE_HPP
