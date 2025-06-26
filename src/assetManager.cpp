//
// Created by Tim Müller on 26.06.25.
//

#include "assetManager.hpp"

namespace arcader {

void assetManager::loadTexture(const std::string& name, const std::filesystem::path& filepath, GLenum internalFormat, GLint mipmaps) {
    Texture<GL_TEXTURE_2D> tex;
    tex.load(internalFormat, filepath, mipmaps);
    textures[name] = std::move(tex);
}

const Texture<GL_TEXTURE_2D>& assetManager::getTexture(const std::string& name) const {
    auto it = textures.find(name);
    if (it == textures.end()) throw std::runtime_error("Texture not found: " + name);
    return it->second;
}

void assetManager::loadMesh(const std::string& name, const std::string& filepath) {
    Mesh mesh;
    mesh.load(filepath);
    meshes[name] = std::move(mesh);
}

void assetManager::loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
    Program program;
    program.load(vertexPath, fragmentPath);
    shaders[name] = std::move(program);
}

const Mesh& assetManager::getMesh(const std::string& name) const {
    auto it = meshes.find(name);
    if (it == meshes.end()) throw std::runtime_error("Mesh not found: " + name);
    return it->second;
}

const Program& assetManager::getShader(const std::string& name) const {
    auto it = shaders.find(name);
    if (it == shaders.end()) throw std::runtime_error("Shader not found: " + name);
    return it->second;
}


void arcader::assetManager::loadRenderable(const std::string& name,
                                           const std::filesystem::path& meshPath,
                                           const std::filesystem::path& vertexShader,
                                           const std::filesystem::path& fragmentShader,
                                           const std::vector<std::filesystem::path>& texturePaths,
                                           GLenum internalFormat,
                                           GLint mipmaps) {
    const std::string meshName = name + "_mesh";
    const std::string shaderName = name + "_shader";

    loadMesh(meshName, meshPath);
    loadShader(shaderName, vertexShader, fragmentShader);

    std::vector<std::string> textureNames;
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        std::string texName = name + "_tex" + std::to_string(i);
        loadTexture(texName, texturePaths[i], internalFormat, mipmaps);
        textureNames.push_back(texName);
    }

    registerRenderable(name, meshName, shaderName, textureNames);
}

void assetManager::registerRenderable(const std::string& name,
                                      const std::string& meshName,
                                      const std::string& shaderName,
                                      const std::vector<std::string>& textureNames) {
    RenderableAsset asset;
    asset.mesh = &meshes.at(meshName);
    asset.shader = &shaders.at(shaderName);

    for (const auto& texName : textureNames) {
        asset.textures.push_back(&textures.at(texName));
    }

    renderables[name] = std::move(asset);
}

} // arcader
