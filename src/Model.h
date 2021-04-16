//
// Created by Admin on 4/15/21.
//

#ifndef MAIN_MODEL_H
#define MAIN_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Mesh.h"

class Model {
public:
    Model(char *path) {
        loadModel(path);
    }

    void Draw(Shader &shader);

private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(const std::string &path);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName);

    unsigned int TextureFromFile(const char *path, std::string directory);
};


#endif //MAIN_MODEL_H
