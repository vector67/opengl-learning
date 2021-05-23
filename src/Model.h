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
    Model();

    void Draw(Shader &shader);

private:
    // model data
    std::vector<Mesh> meshes;
};


#endif //MAIN_MODEL_H
