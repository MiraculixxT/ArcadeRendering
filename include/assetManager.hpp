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

namespace arcader {

struct RenderableAsset {
    Mesh* mesh;
    Program* shader;
    std::vector<Texture<GL_TEXTURE_2D>*> textures;

    void render() const {
        shader->use();

        for (size_t i = 0; i < textures.size(); ++i) {
            textures[i]->bindTextureUnit(static_cast<GLuint>(i));
            shader->bindTextureUnit("tex" + std::to_string(i), static_cast<GLint>(i));
        }

        mesh->draw();
    }
};

enum class StaticAssets {
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_WOOD,
    BLOCK_LEAVES,
    BLOCK_STONE,
    BLOCK_WATER
};

class assetManager {

    Mesh mesh;
    Program program;

public:
    void loadMesh(const std::string& name, const std::string& filepath);
    void loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

    void loadTexture(const std::string& name, const std::filesystem::path& filepath, GLenum internalFormat = GL_SRGB8_ALPHA8, GLint mipmaps = 0);
    const Texture<GL_TEXTURE_2D>& getTexture(const std::string& name) const;

    const Mesh& getMesh(const std::string& name) const;
    const Program& getShader(const std::string& name) const;

    RenderableAsset getRenderable(const std::string& name) const;
    void registerRenderable(const std::string& name, const std::string& mesh, const std::string& shader, const std::vector<std::string>& textureNames);

    void loadRenderable(const std::string& name,
                        const std::filesystem::path& meshPath,
                        const std::filesystem::path& vertexShader,
                        const std::filesystem::path& fragmentShader,
                        const std::vector<std::filesystem::path>& texturePaths,
                        GLenum internalFormat = GL_SRGB8_ALPHA8,
                        GLint mipmaps = 0);

private:
    std::unordered_map<std::string, Mesh> meshes;
    std::unordered_map<std::string, Program> shaders;
    std::unordered_map<std::string, Texture<GL_TEXTURE_2D>> textures;
    std::unordered_map<std::string, RenderableAsset> renderables;
};

} // arcader


#endif //ARCADE_ASSETMANAGER_HPP
