//
// Created by Tim Müller on 26.06.25.
//

#include "assetManager.hpp"

namespace arcader {

    void AssetManager::loadTexture(const StaticAssets& name, const std::filesystem::path& filepath, GLenum internalFormat, GLint mipmaps) {
        Texture<GL_TEXTURE_2D> tex;
        tex.load(internalFormat, filepath, mipmaps);
        textures[name] = std::move(tex);
    }

    const Texture<GL_TEXTURE_2D>& AssetManager::getTexture(const StaticAssets& name) const {
        auto it = textures.find(name);
        if (it == textures.end()) throw std::runtime_error("Texture not found: " + std::to_string(static_cast<int>(name)));
        return it->second;
    }

    void AssetManager::loadMesh(const StaticAssets& name, const std::string& filepath) {
        Mesh mesh;
        mesh.load(filepath);
        meshes[name] = std::move(mesh);
    }

    void AssetManager::loadShader(const StaticAssets& name, const std::string& vertexPath, const std::string& fragmentPath) {
        Program program;
        program.load(vertexPath, fragmentPath);
        shaders[name] = std::move(program);
    }

    const Mesh& AssetManager::getMesh(const StaticAssets& name) const {
        auto it = meshes.find(name);
        if (it == meshes.end()) throw std::runtime_error("Mesh not found: " + std::to_string(static_cast<int>(name)));
        return it->second;
    }

    const Program& AssetManager::getShader(const StaticAssets& name) const {
        auto it = shaders.find(name);
        if (it == shaders.end()) throw std::runtime_error("Shader not found: " + std::to_string(static_cast<int>(name)));
        return it->second;
    }

    RenderableAsset AssetManager::getRenderable(const StaticAssets& name) const {
        auto it = renderables.find(name);
        if (it == renderables.end()) {
            throw std::runtime_error("Renderable not found: " + std::to_string(static_cast<int>(name)));
        }
        return it->second;
    }


    void arcader::AssetManager::loadRenderable(const StaticAssets& name,
                                               const std::filesystem::path& meshPath,
                                               const std::filesystem::path& vertexShader,
                                               const std::filesystem::path& fragmentShader,
                                               const std::vector<std::filesystem::path>& texturePaths,
                                               GLenum internalFormat,
                                               GLint mipmaps) {
        StaticAssets meshName = name;
        StaticAssets shaderName = name;

        loadMesh(meshName, meshPath);
        loadShader(shaderName, vertexShader, fragmentShader);

        std::vector<StaticAssets> textureNames;
        for (size_t i = 0; i < texturePaths.size(); ++i) {
            StaticAssets texName = static_cast<StaticAssets>(static_cast<int>(name) + static_cast<int>(i) + 1); // Example mapping
            loadTexture(texName, texturePaths[i], internalFormat, mipmaps);
            textureNames.push_back(texName);
        }

        registerRenderable(name, meshName, shaderName, textureNames);
    }

    void AssetManager::registerRenderable(const StaticAssets& name,
                                          const StaticAssets& meshName,
                                          const StaticAssets& shaderName,
                                          const std::vector<StaticAssets>& textureNames) {
        RenderableAsset asset;
        asset.mesh = &meshes.at(meshName);
        asset.shader = &shaders.at(shaderName);

        for (const auto& texName : textureNames) {
            asset.textures.push_back(&textures.at(texName));
        }

        renderables[name] = std::move(asset);
    }

    bool AssetManager::hasRenderable(const StaticAssets &name) const {
        return renderables.find(name) != renderables.end();
    }

} // arcader