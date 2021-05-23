//
// Created by Admin on 4/15/21.
//

#include <vector>
#include "Model.h"
std::vector<Texture> textures_loaded;

void Model::Draw(Shader &shader) {
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

Model::Model() = default;
