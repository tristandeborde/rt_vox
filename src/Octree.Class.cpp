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

std::vector<glm::vec3> Octree::voxel_traversal(glm::vec3 ray_start, glm::vec3 ray_end) {
    //J. Amanatides, A. Woo. A Fast Voxel Traversal Algorithm for Ray Tracing. Eurographics '87
    std::vector<glm::vec3> visited_voxels;

    // This id of the first/current voxel hit by the ray.
    // Using floor (round down) is actually very important,
    // the implicit int-casting will round up for negative numbers.
    // WARNING ! Raytracing will be broken if ray_start is outside octree
    glm::vec3 current_voxel(std::floor(ray_start[0]/_bin_size),
                                    std::floor(ray_start[1]/_bin_size),
                                    std::floor(ray_start[2]/_bin_size));

    // The id of the last voxel hit by the ray.
    // TODO: what happens if the end point is on a border?
    // We clip ray_end to the octree's max dimensions.
    glm::vec3 last_voxel(std::max(std::floor(ray_end[0]/_bin_size), this->_width - 1),
                                std::max(std::floor(ray_end[1]/_bin_size), this->_height - 1),
                                std::max(std::floor(ray_end[2]/_bin_size), this->_depth - 1));

    // Compute normalized ray direction.
    glm::vec3 ray = ray_end-ray_start;
    //ray.normalize();

    // In which direction the voxel ids are incremented.
    double stepX = (ray[0] >= 0) ? 1:-1; // correct
    double stepY = (ray[1] >= 0) ? 1:-1; // correct
    double stepZ = (ray[2] >= 0) ? 1:-1; // correct

    // Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
    double next_voxel_boundary_x = (current_voxel[0]+stepX)*_bin_size; // correct
    double next_voxel_boundary_y = (current_voxel[1]+stepY)*_bin_size; // correct
    double next_voxel_boundary_z = (current_voxel[2]+stepZ)*_bin_size; // correct

    // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
    // the value of t at which the ray crosses the first vertical voxel boundary
    double tMaxX = (ray[0]!=0) ? (next_voxel_boundary_x - ray_start[0])/ray[0] : DBL_MAX; //
    double tMaxY = (ray[1]!=0) ? (next_voxel_boundary_y - ray_start[1])/ray[1] : DBL_MAX; //
    double tMaxZ = (ray[2]!=0) ? (next_voxel_boundary_z - ray_start[2])/ray[2] : DBL_MAX; //

    // tDeltaX, tDeltaY, tDeltaZ --
    // how far along the ray we must move for the horizontal component to equal the width of a voxel
    // the direction in which we traverse the grid
    // can only be FLT_MAX if we never go in that direction
    double tDeltaX = (ray[0]!=0) ? _bin_size/ray[0]*stepX : DBL_MAX;
    double tDeltaY = (ray[1]!=0) ? _bin_size/ray[1]*stepY : DBL_MAX;
    double tDeltaZ = (ray[2]!=0) ? _bin_size/ray[2]*stepZ : DBL_MAX;

    glm::vec3 diff(0,0,0);
    bool neg_ray=false;
    if (current_voxel[0]!=last_voxel[0] && ray[0]<0) { diff[0]--; neg_ray=true; }
    if (current_voxel[1]!=last_voxel[1] && ray[1]<0) { diff[1]--; neg_ray=true; }
    if (current_voxel[2]!=last_voxel[2] && ray[2]<0) { diff[2]--; neg_ray=true; }
    visited_voxels.push_back(current_voxel);
    if (neg_ray) {
        current_voxel += diff;
        visited_voxels.push_back(current_voxel);
    }

    while(last_voxel != current_voxel) {
        if (tMaxX < tMaxY) {
        if (tMaxX < tMaxZ) {
            current_voxel[0] += stepX;
            tMaxX += tDeltaX;
        } else {
            current_voxel[2] += stepZ;
            tMaxZ += tDeltaZ;
        }
        } else {
        if (tMaxY < tMaxZ) {
            current_voxel[1] += stepY;
            tMaxY += tDeltaY;
        } else {
            current_voxel[2] += stepZ;
            tMaxZ += tDeltaZ;
        }
        }
        visited_voxels.push_back(current_voxel);
    }
    return visited_voxels;
}
