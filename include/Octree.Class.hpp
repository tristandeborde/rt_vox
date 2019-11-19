#ifndef OCTREE_CLASS_HPP
# define OCTREE_CLASS_HPP

# include "rt_vox.hpp"

class Octree
{
public:
    Octree(int width, int height, int depth, vector<Cube> &cubes);
    Octree() = delete;
    Octree(Octree &src) = delete;
    Octree &operator=(Octree &src) = delete;
    ~Octree();

    char* rasterizeScene();
    int getHeight();
    int getWidth();
    int getDepth();

private:
    int _height, _width, _depth;
    vector<Cube> &cubes;
    GLuint _textureID;

};


#endif