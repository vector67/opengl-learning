#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        std::string srcFolder = "src/shaders/";
        vShaderFile.open(srcFolder.append(vertexPath));
        srcFolder = "src/shaders/";
        fShaderFile.open(srcFolder.append(fragmentPath));
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch(std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const {
    glUseProgram(ID);
}



void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, float val1, float val2, float val3) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), val1, val2, val3);
}

void Shader::setVec3(const std::string &name, glm::vec3 vec) {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::setVec4(const std::string &name, float val1, float val2, float val3, float val4) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), val1, val2, val3, val4);
}

void Shader::setMat4(const std::string &name, glm::mat4 mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setPointLight(const int num, const PointLight &light) {
    char buf[30];

    snprintf(buf, 30, "pointLights[%d].position", num);
    setVec3(buf, light.position);

    snprintf(buf, 30, "pointLights[%d].ambient", num);
    setVec3(buf, light.ambient);

    snprintf(buf, 30, "pointLights[%d].diffuse", num);
    setVec3(buf, light.diffuse);

    snprintf(buf, 30, "pointLights[%d].specular", num);
    setVec3(buf, light.diffuse);

    snprintf(buf, 30, "pointLights[%d].constant", num);
    setFloat(buf, light.constant);

    snprintf(buf, 30, "pointLights[%d].linear", num);
    setFloat(buf, light.linear);

    snprintf(buf, 30, "pointLights[%d].quadratic", num);
    setFloat(buf, light.quadratic);
}

void Shader::setDirLight(const DirLight &light) {
    setVec3("dirLight.direction", light.direction);
    setVec3("dirLight.ambient", light.ambient);
    setVec3("dirLight.diffuse", light.diffuse);
    setVec3("dirLight.specular", light.specular);
}
