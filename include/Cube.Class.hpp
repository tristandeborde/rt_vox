#ifndef CUBE_CLASS_HPP
# define CUBE_CLASS_HPP

# include <glm/vec3.hpp>
# include <glm/matrix.hpp>

class Cube
{
public:
    Cube(
        const float &width,
        const glm::mat4 &transMat,
        const glm::vec3 &sc,
        const float &refl,
        const float &transp,
        const glm::vec3 &ec); 
    ~Cube();
    Cube(const Cube &) = default;
    Cube& operator=(const Cube &) = delete;

    glm::vec3 getMin() const;
    glm::vec3 getMax() const;
    glm::vec3 getCenter() const;
    glm::mat4 gettransMat() const;

    glm::vec3 getSurfaceColor() const;
    glm::vec3 getEmissionColor() const;
    float getTransparency() const;
    float getReflection() const;

    bool intersect(const glm::vec3 &rayorig, const glm::vec3 &raydir, float &intersection_dist) const;
    glm::vec3 computeNormal(const glm::vec3 &hit_point) const;

private:
    glm::vec3       _min;        // minimum X,Y,Z coords of cube when not transformed.
    glm::vec3       _max;        // maximum X,Y,Z coords of cube when not transformed.
    glm::mat4       _transMat;   // transformation matrix applied to cube (the center of the cube corresponds to the 4th column)

    glm::vec3 _surfaceColor;
    float _reflection;             // surface transparency and reflectivity
    float _transparency;
    glm::vec3 _emissionColor;    // surface color and emission (light)
    
};
#endif