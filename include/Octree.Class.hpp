#ifndef OCTREE_CLASS_HPP
# define OCTREE_CLASS_HPP

# include "rt_vox.hpp"

class Octree
{
public:
    Octree(int width, int height, int depth, std::vector<Cube> &cubes);
    Octree() = delete;
    Octree(Octree &src) = delete;
    Octree &operator=(Octree &src) = delete;
    ~Octree();

    void rasterizeScene();
    int getHeight();
    int getWidth();
    int getDepth();
    

private:
    int _height, _width, _depth;
    std::vector<Cube> &cubes;
    GLuint _textureID;

};


#endif