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
#include <vector>
#include <iostream>
#include <cassert>
#include "Cube.Class.hpp"

#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif

#define MAX_RAY_DEPTH 5

float mix(const float &a, const float &b, const float &mix)
{
    return b * mix + a * (1 - mix);
}

//[comment]
// This is the main trace function. It takes a ray as argument (defined by its origin
// and direction). We test if this ray intersects any of the geometry in the scene.
// If the ray intersects an object, we compute the intersection point, the normal
// at the intersection point, and shade this point using this information.
// Shading depends on the surface property (is it transparent, reflective, diffuse).
// The function returns a color for the ray. If the ray intersects an object that
// is the color of the object at the intersection point, otherwise it returns
// the background color.
//[/comment]
glm::vec3 trace(
    const glm::vec3 &rayorig,
    const glm::vec3 &raydir,
    const std::vector<Cube> &cubes,
    const int &depth)
{
    //if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
    float tnear = INFINITY;
    const Cube* cube = NULL;

    // TODO: Replace vector<Cube> search by Octree search
    for (unsigned i = 0; i < cubes.size(); ++i) {
        float intersection_dist;
        if (cubes[i].intersect(rayorig, raydir, intersection_dist)) {
            if (intersection_dist < tnear) {
                tnear = intersection_dist;
                cube = &cubes[i];
            }
        }
    }
    // if there's no intersection return black or background color
    if (!cube)
        return glm::vec3(2);
    glm::vec3 surfaceColor(0); // color of the ray/surfaceof the object intersected by the ray
    glm::vec3 hit_point = rayorig + raydir * tnear; // point of intersection
    glm::vec3 hit_normal = cube->computeNormal(hit_point); // normal at the intersection point

    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the cube so set
    // the inside bool to true. Finally reverse the sign of IdotN which we want
    // positive.
    float bias = 1e-4; // add some bias to the point from which we will be tracing
    bool inside = false;
    if (glm::dot(raydir, hit_normal) > 0)
    {
        hit_normal = -hit_normal;
        inside = true;
    }
    if ((cube->getTransparency() > 0 || cube->getReflection() > 0) && depth < MAX_RAY_DEPTH) {
        float facingratio = - glm::dot(raydir, hit_normal);
        // change the mix value to tweak the effect
        float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);
        // compute reflection direction (not need to normalize because all vectors
        // are already normalized)
        glm::vec3 refldir = raydir - hit_normal * 2.f * glm::dot(raydir, hit_normal);
        refldir = glm::normalize(refldir);
        glm::vec3 reflection = trace(hit_point + hit_normal * bias, refldir, cubes, depth + 1);
        glm::vec3 refraction(0);
        // if the cube is also transparent compute refraction ray (transmission)
        if (cube->getTransparency()) {
            float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
            float cosi = - glm::dot(hit_normal, raydir);
            float k = 1 - eta * eta * (1 - cosi * cosi);
            glm::vec3 refrdir = raydir * eta + hit_normal * (eta *  cosi - sqrt(k));
            refrdir = glm::normalize(refrdir);
            refraction = trace(hit_point - hit_normal * bias, refrdir, cubes, depth + 1);
        }
        // the result is a mix of reflection and refraction (if the cube is transparent)
        surfaceColor = (
            reflection * fresneleffect +
            refraction * (1 - fresneleffect) * cube->getTransparency()) * cube->getSurfaceColor();
    }
    else {
        // it's a diffuse object, no need to raytrace any further
        for (unsigned i = 0; i < cubes.size(); ++i) {
            if (cubes[i].getEmissionColor().x > 0) {
                // this is a light
                glm::vec3 transmission(1);
                glm::vec3 lightDirection = cubes[i].getCenter() - hit_point;
                lightDirection = glm::normalize(lightDirection);
                for (unsigned j = 0; j < cubes.size(); ++j) {
                    if (i != j) {
                        float intersection_dist;
                        if (cubes[j].intersect(hit_point + hit_normal * bias, lightDirection, intersection_dist)) {
                            transmission = glm::vec3(0);
                            break;
                        }
                    }
                }
                surfaceColor += cube->getSurfaceColor() * transmission *
                std::max(float(0), glm::dot(hit_normal, lightDirection)) * cubes[i].getEmissionColor();
            }
        }
    }
    return surfaceColor + cube->getEmissionColor();
}

void render(const std::vector<Cube> &cubes, std::string &fn)
{
    unsigned width = 1280, height = 1024;
    glm::vec3 *image = new glm::vec3[width * height], *pixel = image;
    float invWidth = 1 / float(width), invHeight = 1 / float(height);
    float fov = 30, aspectratio = width / float(height);
    float angle = tan(M_PI * 0.5 * fov / 180.);
    // Trace rays
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x, ++pixel) {
            float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
            float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            glm::vec3 raydir(xx, yy, -1);
            raydir = glm::normalize(raydir);
            *pixel = trace(glm::vec3(0), raydir, cubes, 0);
        }
    }
    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs("./" + fn + ".ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (unsigned i = 0; i < width * height; ++i) {
        ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
               (unsigned char)(std::min(float(1), image[i].y) * 255) <<
               (unsigned char)(std::min(float(1), image[i].z) * 255);
    }
    ofs.close();
    delete [] image;
}