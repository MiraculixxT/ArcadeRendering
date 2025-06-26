//
// Created by Tim Müller on 26.06.25.
//


#include "cinematicEngine.hpp"

namespace arcader {

CinematicEngine::CinematicEngine(assetManager* assets)
    : state(0), timer(0.0f), assets(assets), camera() {}

void CinematicEngine::update(float deltaTime) {
    timer += deltaTime;
    switch (state) {
        case 0: updateScene0(deltaTime); break;
        case 1: updateScene1(deltaTime); break;
        case 2: updateScene2(deltaTime); break;
        case 3: updateScene3(deltaTime); break;
        default: break;
    }
}

void CinematicEngine::render() {
    switch (state) {
        case 0: renderScene0(); break;
        case 1: renderScene1(); break;
        case 2: renderScene2(); break;
        case 3: renderScene3(); break;
        default: break;
    }
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

void CinematicEngine::updateScene0(float deltaTime) {
    // TODO: Implement logic for scene 0
}

void CinematicEngine::updateScene1(float deltaTime) {
    // TODO: Implement logic for scene 1
}

void CinematicEngine::updateScene2(float deltaTime) {
    // TODO: Implement logic for scene 2
}

void CinematicEngine::updateScene3(float deltaTime) {
    // TODO: Implement logic for scene 3
}

void CinematicEngine::renderScene0() {
    // TODO: Implement rendering for scene 0
}

void CinematicEngine::renderScene1() {
    // TODO: Implement rendering for scene 1
}

void CinematicEngine::renderScene2() {
    // TODO: Implement rendering for scene 2
}

void CinematicEngine::renderScene3() {
    // TODO: Implement rendering for scene 3
}

}
