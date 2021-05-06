//
// Created by Admin on 4/15/21.
//

#ifndef MAIN_MESH_H
#define MAIN_MESH_H


#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <assimp/types.h>
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    aiString path;
};

class Mesh {
public:
    // mesh data
    Vertex *vertices;
    unsigned int numVertices;
    unsigned int *indices;
    unsigned int numIndices;
    Texture *textures;
    unsigned int numTextures;

    Mesh(Vertex *vertices, unsigned int numVertices,
         unsigned int *indices, unsigned int numIndices,
         Texture *textures, unsigned int numTextures);

    void Draw(Shader &shader);

private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};


#endif //MAIN_MESH_H
