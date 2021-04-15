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

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void use() const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    void setColour(const std::string &name, float red, float green, float blue, float alpha) const;

    void setMat4(const std::string &name, glm::mat4 mat) const;
};


#endif //MAIN_SHADER_H
