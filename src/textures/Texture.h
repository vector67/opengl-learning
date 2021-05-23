//
// Created by Admin on 2021/05/21.
//

#ifndef MAIN_TEXTURE_H
#define MAIN_TEXTURE_H


#include <string>

class Texture {
public:
    Texture(unsigned int width, unsigned int height, std::string type);

    void init(unsigned char* data) const;

    std::string getType();

    unsigned int id;
    std::string type;
private:
    unsigned int width, height;

};


#endif //MAIN_TEXTURE_H
