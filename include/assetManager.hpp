//
// Created by Tim Müller on 26.06.25.
//

#ifndef ARCADE_ASSETMANAGER_HPP
#define ARCADE_ASSETMANAGER_HPP

#include <string>
#include <unordered_map>
#include <framework/mesh.hpp>
#include <framework/gl/program.hpp>
#include <framework/gl/texture.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace arcader {

    struct RenderableAsset {
        Mesh *mesh;
        Program *shader;
        std::vector<Texture<GL_TEXTURE_2D> *> textures;
        std::vector<Mesh::VertexPTN> vertices;
        std::vector<unsigned int> indices;
        GLuint triangleCount;

        void render(const glm::mat4 &worldToClip,
                    const glm::vec3 &position = glm::vec3(0.0f),
                    const glm::vec3 &scale = glm::vec3(1.0f)) const {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), scale);
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

            shader->use();
            shader->set("uWorldToClip", worldToClip);
            shader->set("uModelMatrix", model);
            shader->set("uNormalMatrix", normalMatrix);

            for (size_t i = 0; i < textures.size(); ++i) {
                textures[i]->bindTextureUnit(static_cast<GLuint>(i));
                shader->bindTextureUnit("tex" + std::to_string(i), static_cast<GLint>(i));
            }

            mesh->draw();
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        }
    };

    enum class StaticAssets {
        MISSING_TEXTURE,

        BLOCK_GRASS,
        BLOCK_DIRT,
        BLOCK_WOOD,
        BLOCK_LEAVES,
        BLOCK_STONE,
        BLOCK_WATER,
        BLOCK_AIR,
        PLAYER_IDLE,
        PLAYER_WALK1,
        PLAYER_WALK2,
        PLAYER_WALK3,
        PLAYER_MINE,
        HUD_SLOT,
        BACKGROUND,

        SHADER_TILE,
        SHADER_ENTITY,
        SHADER_DEBUG,
        SHADER_HUD,

        ARCADE_MACHINE,
        ARCADE_MACHINE_2,
        ARCADE_MACHINE_3,
        ARCADE_MACHINE_4,
        ARCADE_MACHINE_5,
        ROOM
    };

    class AssetManager {

        Program program;

    public:
        void loadMesh(const StaticAssets &name, const std::string &filepath);

        void loadShader(const StaticAssets &name, const std::string &vertexPath, const std::string &fragmentPath);

        AssetManager();

        void loadTexture(const StaticAssets &name, const std::filesystem::path &filepath,
                         GLenum internalFormat = GL_SRGB8_ALPHA8, GLint mipmaps = 0);

        const Texture<GL_TEXTURE_2D> &getTexture(const StaticAssets &name) const;

        const Mesh &getMesh(const StaticAssets &name) const;

        Program &getShader(const StaticAssets &name);

        RenderableAsset getRenderable(const StaticAssets &name) const;

        void registerRenderable(const StaticAssets &name, const StaticAssets &meshName,
                                              const StaticAssets &shaderName,
                                              const std::vector<StaticAssets> &textureNames);

        void loadRenderable(const StaticAssets &name,
                            const std::filesystem::path &meshPath,
                            const std::filesystem::path &vertexShader,
                            const std::filesystem::path &fragmentShader,
                            const std::vector<std::filesystem::path> &texturePaths,
                            GLenum internalFormat = GL_SRGB8_ALPHA8,
                            GLint mipmaps = 0);

        bool hasRenderable(const StaticAssets &name) const;

        void render(const StaticAssets &asset,
                    const glm::mat4 &worldToClip,
                    const glm::vec3 &position = glm::vec3(0.0f),
                    const glm::vec3 &scale = glm::vec3(1.0f)) const {
            if (!hasRenderable(asset)) return;
            renderables.at(asset).render(worldToClip, position, scale);
        }

        void loadRenderableRT(const StaticAssets &name,
                              const std::string &objPath,
                              const std::string &vertexShaderPath,
                              const std::string &fragmentShaderPath,
                              const std::vector<std::filesystem::path> &texturePaths);


    private:
        std::unordered_map<StaticAssets, Mesh> meshes;
        std::unordered_map<StaticAssets, Program> shaders;
        std::unordered_map<StaticAssets, Texture<GL_TEXTURE_2D>> textures;
        std::unordered_map<StaticAssets, RenderableAsset> renderables;
    };


} // arcader


#endif //ARCADE_ASSETMANAGER_HPP