#pragma once

#include "Shader.h"
#include "Mesh.h"
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
    Model(std::string path);
    void Draw(Shader& shader);

private:
    // model data
    std::vector<TextureStruct> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

private:
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureStruct> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);
};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
