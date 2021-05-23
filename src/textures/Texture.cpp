//
// Created by Admin on 2021/05/21.
//

#include <glad/glad.h>

#include <utility>
#include "Texture.h"

Texture::Texture(unsigned int width, unsigned int height, std::string type) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    this->width = width;
    this->height = height;
    this->type = std::move(type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::init(unsigned char *data) const {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

std::string Texture::getType() {
    return type;
}
