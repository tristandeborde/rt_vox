// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cassert>
#include "Raytracer.Class.hpp"


Raytracer::Raytracer(std::vector<Cube> &c): _cubes(c) {
    this->_width = 1280;
    this->_height = 1024;
    this->_maxRayDepth = 5;
    this->_bias = 1e-4;
}

Raytracer::~Raytracer() {
    return;
}

void Raytracer::render(const std::string &fn) {
    glm::vec3 *image = new glm::vec3[this->_width * this->_height];
    glm::vec3 *pixel = image;

    float invWidth = 1 / float(this->_width), invHeight = 1 / float(this->_height);
    float fov = 30, aspectratio = this->_width / float(this->_height);
    float angle = tan(M_PI * 0.5 * fov / 180.);
    
    // Trace rays
    for (unsigned y = 0; y < this->_height; ++y) {
        for (unsigned x = 0; x < this->_width; ++x, ++pixel) {
            float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
            float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            glm::vec3 raydir(xx, yy, -1);
            raydir = glm::normalize(raydir);
            *pixel = trace(glm::vec3(0), raydir, 0);
        }
    }
    this->saveImage(fn, image);
}

void Raytracer::saveImage(const std::string &fn, const glm::vec3 *image) {
    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs("./" + fn + ".ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << this->_width << " " << this->_height << "\n255\n";
    for (unsigned i = 0; i < this->_width * this->_height; ++i) {
        ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
               (unsigned char)(std::min(float(1), image[i].y) * 255) <<
               (unsigned char)(std::min(float(1), image[i].z) * 255);
    }
    ofs.close();
    delete [] image;
}

glm::vec3 Raytracer::trace(const glm::vec3 &rayorig, const glm::vec3 &raydir, const int &depth)
{
    float tnear = INFINITY;

    // TODO: replace with Octree Search
    const Cube* cube = this->searchCube(rayorig, raydir, tnear);

    // if there's no intersection return black or background color
    if (!cube)
        return glm::vec3(2);
    
    glm::vec3 surfaceColor(0); // color of the ray/surface of the object intersected by the ray
    glm::vec3 hit_point = rayorig + raydir * tnear; // point of intersection
    glm::vec3 hit_normal = cube->computeNormal(hit_point); // normal at the intersection point

    // Check if inside the cube by comparing view direction and normal.
    bool inside = false;
    if (glm::dot(raydir, hit_normal) > 0)
    {
        hit_normal = -hit_normal;
        inside = true;
    }

    if ((cube->getTransparency() > 0 || cube->getReflection() > 0) && depth < this->_maxRayDepth)
        surfaceColor = this->computeReflRefr(cube, hit_point, hit_normal, raydir, depth, inside);
    else {
        // it's a diffuse object, no need to raytrace any further
        surfaceColor = computeDiffuse(cube, hit_point, hit_normal);
    }
    return surfaceColor + cube->getEmissionColor();
}

const Cube  *Raytracer::searchCube(const glm::vec3 &rayorig, const glm::vec3 &raydir, float &tnear) {
    for (unsigned i = 0; i < this->_cubes.size(); ++i) {
        float intersection_dist;
        if (this->_cubes[i].intersect(rayorig, raydir, intersection_dist)) {
            if (intersection_dist < tnear) {
                tnear = intersection_dist;
                return &this->_cubes[i];
            }
        }
    }
    return NULL;
}

glm::vec3 Raytracer::computeReflRefr(const Cube *cube, const glm::vec3 &hit_point, const glm::vec3 &hit_normal, const glm::vec3 &raydir, const int &depth, const bool &inside){
    float facingratio = - glm::dot(raydir, hit_normal);
    // change the mix value to tweak the effect
    float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);

    // compute reflection direction (not need to normalize because all vectors
    // are already normalized)
    glm::vec3 refldir = raydir - hit_normal * 2.f * glm::dot(raydir, hit_normal);
    refldir = glm::normalize(refldir);
    glm::vec3 reflection = trace(hit_point + hit_normal * this->_bias, refldir, depth + 1);
    glm::vec3 refraction(0);

    // if the cube is also transparent compute refraction ray (transmission)
    if (cube->getTransparency()) {
        float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
        float cosi = - glm::dot(hit_normal, raydir);
        float k = 1 - eta * eta * (1 - cosi * cosi);
        glm::vec3 refrdir = raydir * eta + hit_normal * (eta *  cosi - sqrt(k));
        refrdir = glm::normalize(refrdir);
        refraction = trace(hit_point - hit_normal * this->_bias, refrdir, depth + 1);
    }
    // the result is a mix of reflection and refraction (if the cube is transparent)
    return (
        reflection * fresneleffect +
        refraction * (1 - fresneleffect) * cube->getTransparency()) * cube->getSurfaceColor();
}

glm::vec3 Raytracer::computeDiffuse(const Cube *cube, const glm::vec3 &hit_point, const glm::vec3 &hit_normal) {
    glm::vec3 surfaceColor(0);
    
    for (unsigned i = 0; i < this->_cubes.size(); ++i) {
        if (this->_cubes[i].getEmissionColor().x > 0) {
            // this is a light
            glm::vec3 transmission(1);
            glm::vec3 lightDirection = this->_cubes[i].getCenter() - hit_point;
            lightDirection = glm::normalize(lightDirection);
            for (unsigned j = 0; j < this->_cubes.size(); ++j) {
                if (i != j) {
                    float intersection_dist;
                    if (this->_cubes[j].intersect(hit_point + hit_normal * this->_bias, lightDirection, intersection_dist)) {
                        transmission = glm::vec3(0);
                        break;
                    }
                }
            }
            surfaceColor += cube->getSurfaceColor() * transmission *
            std::max(float(0), glm::dot(hit_normal, lightDirection)) * this->_cubes[i].getEmissionColor();
        }
    }
    return surfaceColor;
}

float Raytracer::mix(const float &a, const float &b, const float &mix)
{
    return b * mix + a * (1 - mix);
}
