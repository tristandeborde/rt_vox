#include "Cube.Class.hpp"

Cube::Cube(
        const Vec3f &c,
        const float &r,
        const Vec3f &sc,
        const float &refl = 0,
        const float &transp = 0,
        const Vec3f &ec = 0): 
    center(c), halfLength(r), surfaceColor(sc), emissionColor(ec), 
    transparency(transp), reflection(refl)
{
    return;
}

Cube::~Cube() {
    return;
}

bool Cube::intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1) const {
    // TODO
    return true;
}

Vec3f Cube::getCenter() {
    return this->center;
}

Vec3f Cube::getSurfaceColor() {
    return this->surfaceColor;
}

Vec3f Cube::getEmissionColor() {
    return this->emissionColor;
}

float Cube::getHalfLength() {
    return this->halfLength;
}

float Cube::getTransparency() {
    return this->transparency;
}

float Cube::getReflection() {
    return this->reflection;
}

