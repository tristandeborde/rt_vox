#ifndef CUBE_CLASS_HPP
# define CUBE_CLASS_HPP

# include "Vector3.Class.hpp"

class Cube
{
public:
    Cube(
        const Vec3f &c,
        const float &r,
        const Vec3f &sc,
        const float &refl = 0,
        const float &transp = 0,
        const Vec3f &ec = 0);
    ~Cube();
    Cube(const Cube &) = delete;
    Cube& operator=(const Cube &) = delete;

    Vec3f getCenter();
    Vec3f getSurfaceColor();
    Vec3f getEmissionColor();
    float getHalfLength();
    float getTransparency();
    float getReflection();

    bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1) const;

private:
    Vec3f center;                           /// position of the cube
    float halfLength;                       /// half the length of a side of the cube
    Vec3f surfaceColor, emissionColor;      /// surface color and emission (light)
    float transparency, reflection;         /// surface transparency and reflectivity
    
};
#endif