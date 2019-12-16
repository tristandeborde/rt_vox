#include "Cube.Class.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Cube::Cube(
        const float &width,
        const glm::mat4 &transMat,
        const glm::vec3 &sc,
        const float &refl = 0,
        const float &transp = 0,
        const glm::vec3 &ec = glm::vec3(0)): 
    _transMat(transMat), _surfaceColor(sc), _reflection(refl), _transparency(transp), _emissionColor(ec)
{
    glm::vec3 center(transMat[3].x, transMat[3].y, transMat[3].z);
    this->_min = center - (width/2);
    this->_max = center + (width/2);
    return;
}

Cube::~Cube() {
    return;
}

bool Cube::intersect(const glm::vec3 &ray_orig, const glm::vec3 &ray_dir, float &intersection_dist) const {
    // Source: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
    float tMin = 0.0f;
    float tMax = 100000.0f;

    // 4th vector in a transformation matrix is the translation vector
    glm::vec3 OBBposition_worldspace = this->getCenter();

    glm::vec3 delta = OBBposition_worldspace - ray_orig;

    // Cube is an OBB, so we need its X axis (not the world's X axis)
    glm::vec3 xaxis(this->_transMat[0].x, this->_transMat[0].y, this->_transMat[0].z); 

    float e = glm::dot(xaxis, delta); // project delta onto OBB's xaxis
    float f = glm::dot(xaxis, ray_dir); // project raydir onto OBB's xaxis

    float t1;
    float t2;

    if ( fabs(f) > 0.001f ) { // Standard case
        t1 = (e + this->_min.x)/f; // Intersection with the "left" plane
        t2 = (e + this->_max.x)/f; // Intersection with the "right" plane
        // t1 and t2 now contain distances between ray origin and ray-plane intersections

        // We want t1 to represent the nearest intersection, 
        // so if it's not the case, invert t1 and t2
        if (t1>t2) {
            float w=t1;t1=t2;t2=w; // swap t1 and t2
        }

        // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
        if ( t2 < tMax )
            tMax = t2;
        // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
        if ( t1 > tMin )
            tMin = t1;

        // And here's the trick :
        // If "far" is closer than "near", then there is NO intersection.
        // See the images in the tutorials for the visual explanation.
        if (tMax < tMin )
            return false;
    } 
    else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
        if(-e+this->_min.x > 0.0f || -e+this->_max.x < 0.0f)
            return false;
    }

	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
    glm::vec3 yaxis(this->_transMat[1].x, this->_transMat[1].y, this->_transMat[1].z);
    e = glm::dot(yaxis, delta);
    f = glm::dot(ray_dir, yaxis);

    t1 = (e+this->_min.y)/f;
    t2 = (e+this->_max.y)/f;

    if (t1>t2){float w=t1;t1=t2;t2=w;}

    if ( t2 < tMax )
        tMax = t2;
    if ( t1 > tMin )
        tMin = t1;
    if (tMin > tMax)
        return false;
  

	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
    glm::vec3 zaxis(this->_transMat[2].x, this->_transMat[2].y, this->_transMat[2].z);
    e = glm::dot(zaxis, delta);
    f = glm::dot(ray_dir, zaxis);
    
    t1 = (e+this->_min.z)/f;
    t2 = (e+this->_max.z)/f;
    
    if (t1>t2){float w=t1;t1=t2;t2=w;}

    if ( t2 < tMax )
        tMax = t2;
    if ( t1 > tMin )
        tMin = t1;
    if (tMin > tMax)
        return false;
    
    intersection_dist = tMin;
    return true;
}

glm::vec3 Cube::computeNormal(const glm::vec3 &hit_point) const {
    static const glm::vec3 normals[] = { // A cube has 3 possible orientations
        glm::vec3(1,0,0),
        glm::vec3(0,1,0),
        glm::vec3(0,0,1)
    };
    const glm::vec3 interRelative = hit_point - this->getCenter();
    const float yxCoef = interRelative.y / interRelative.x;
    const float xzCoef = interRelative.x / interRelative.z;
    const float yzCoef = interRelative.y / interRelative.z;

    int coef = (std::abs(yxCoef) <= 1 ? (std::abs(xzCoef) <= 1 ? 2 : 0) :
                      (std::abs(yzCoef) <= 1 ? 2 : 1));
    return glm::normalize(normals[coef] * interRelative); // Find the normal's sign
}

// ********************************
// GETTERS

glm::vec3 Cube::getMin() const {
    return this->_min;
}

glm::vec3 Cube::getMax() const {
    return this->_max;
}

glm::mat4 Cube::gettransMat() const {
    return this->_transMat;
}

glm::vec3 Cube::getCenter() const {
    return glm::vec3(this->_transMat[3].x, this->_transMat[3].y, this->_transMat[3].z);
}

glm::vec3 Cube::getSurfaceColor() const {
    return this->_surfaceColor;
}

glm::vec3 Cube::getEmissionColor() const {
    return this->_emissionColor;
}

float Cube::getTransparency() const {
    return this->_transparency;
}

float Cube::getReflection() const {
    return this->_reflection;
}

