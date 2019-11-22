#ifndef RAYTRACER_CLASS_HPP
# define RAYTRACER_CLASS_HPP

# include <string>
# include <vector>
# include "Cube.Class.hpp"

class Raytracer
{
public:
    Raytracer(std::vector<Cube >&c);
    ~Raytracer();
    Raytracer() = delete;
    Raytracer(Raytracer &src) = delete;
    Raytracer &operator=(Raytracer &src) = delete;

    void render(const std::string &fn);

private:
    std::vector<Cube>  &_cubes;
    unsigned int        _width, _height;
    int                 _maxRayDepth;
    float               _bias;

    glm::vec3   trace(const glm::vec3 &rayorig, const glm::vec3 &raydir, const int &depth);
    const Cube  *searchCube(const glm::vec3 &rayorig, const glm::vec3 &raydir, float &tnear);
    glm::vec3   computeDiffuse(const Cube *cube, const glm::vec3 &hit_point, const glm::vec3 &hit_normal);
    glm::vec3   computeReflRefr(const Cube *cube, const glm::vec3 &hit_point, const glm::vec3 &hit_normal, const glm::vec3 &raydir, const int &depth, const bool &inside);
    float       mix(const float &a, const float &b, const float &mix);
    void        saveImage(const std::string &fn, const glm::vec3 *image);

};


#endif