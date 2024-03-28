#pragma once

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"


class Model
{
public:
    Model(std::string path);
    void Draw(Shader& shader);
    void DrawInstanced(Shader& shader, int instanceCount);
    glm::vec3 GetAverageNormal();
    void AddTexture(Texture& texture, std::string type, int meshIndex);
    int GetMeshCount() const { return meshes.size(); }
    const Mesh& GetMesh(int i) const { return meshes[i]; }
    std::string getPath() const { return path; };

private:
    // model data
    std::vector<TextureStruct> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    std::string path;
    bool gammaCorrection;
    glm::mat4 modelMatrix;

private:
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureStruct> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);
};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
