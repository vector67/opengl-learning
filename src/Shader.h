//
// Created by Admin on 4/13/21.
//

#ifndef MAIN_SHADER_H
#define MAIN_SHADER_H

#include "../include/glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/detail/type_mat4x4.hpp>

struct PointLight {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void use() const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    void setVec3(const std::string &name, float val1, float val2, float val3) const;
    void setVec3(const std::string &name, glm::vec3 vec);

    void setVec4(const std::string &name, float val1, float val2, float val3, float val4) const;

    void setMat4(const std::string &name, glm::mat4 mat) const;

    void setPointLight(const int num, const PointLight &light);
    void setDirLight(const DirLight &light);
};


#endif //MAIN_SHADER_H
