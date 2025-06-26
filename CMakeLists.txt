cmake_minimum_required(VERSION 3.26..4.0)

project(arcade VERSION 1.0 LANGUAGES CXX)

file(GLOB INCLUDE_FILES include/*.h)

set(SRC
    src/main.cpp
        src/game/gameManager.cpp
        src/assetManager.cpp
        src/cinematicEngine.cpp
        ${INCLUDE_FILES}
)

# Fetch framework
include(FetchContent)
FetchContent_Declare(
    framework
    #GIT_REPOSITORY https://github.com/julcst/gltemplate/
    URL https://github.com/julcst/gltemplate/archive/v1.6.tar.gz
)
FetchContent_MakeAvailable(framework)

# Add executable
add_executable(${PROJECT_NAME} ${SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE framework)
