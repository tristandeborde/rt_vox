#include "Octree.Class.hpp"

Octree::Octree(int h, int w, int d, std::vector<Cube> &cubes): _height(h), _width(w), _depth(d), cubes(cubes) {
    char *buffer = new char[this->_height * this->_width * this->_depth];
    delete[] buffer;
}

// Keep for later: OpenGL version
/* 
    // Create one OpenGL texture
    glGenTextures(1, &this->_textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_3D, this->_textureID);

    char *buffer = new char[this->_height * this->_width * this->_depth];

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, this->_height, this->_width, this->_depth, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);

    delete[] buffer;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexSubImage3D();
*/

Octree::~Octree() {
    return;
}

void    Octree::rasterizeScene() {
    return;
}