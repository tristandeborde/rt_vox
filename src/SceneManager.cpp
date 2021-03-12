#include "SceneManager.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>


SceneManager::SceneManager(Camera &cam, PhysicsManager &pm, RenderingManager &rm)
    : m_cam(cam), m_pm(pm), m_rm(rm) {
    m_sc = Scene();
    m_objects.reserve(1000);
    m_cube_radius = glm::sqrt(2*(pow(CUBE_SIZE/2, 2)));
    this->readScene();
    m_selection_mat_idx = 2;
    m_selected_plane = std::make_pair(m_sc.objects.end(), Planes::left);
    return;
}

SceneManager::~SceneManager(){
    return;
}

bool SceneManager::intersectPlanes(const glm::vec3 &axis, std::vector<Object>::iterator it, PlaneInfo &p_info) {
	// Test intersection between ray and the 2 planes perpendicular to one OBB axis
    int swapped = 0;

    if (fabs(p_info.ray_proj) > 0.000001f) {
        float plane_intersect1 = (p_info.center_proj - it->c.halfSize)/p_info.ray_proj; // Intersection with "left" (for xaxis) plane
        float plane_intersect2 = (p_info.center_proj + it->c.halfSize)/p_info.ray_proj; // Intersection with "right" (for xaxis) plane

        // We want plane_intersect1 to be nearest intersection, so if it's not, invert plane_intersect1 and plane_intersect2
        if (plane_intersect1 > plane_intersect2) {
            float tmp = plane_intersect1;
            plane_intersect1 = plane_intersect2;
            plane_intersect2 = tmp;
            swapped = 1; // Used to invert the normal (~ choose opposite cube face)
        }

        p_info.max_intersect = plane_intersect2 < p_info.max_intersect ? plane_intersect2 : p_info.max_intersect; // max_intersect = nearest "far" intersection
        if (plane_intersect1 > p_info.min_intersect) { // min_intersect = farthest "near" intersection
            p_info.min_intersect = plane_intersect1;
            p_info.plane = static_cast<Planes>((p_info.ax_index * 2) + swapped);
        }

        // The trick : if "far" is closer than "near", then there is NO intersection.
        if (p_info.max_intersect < p_info.min_intersect)
            return false;
    }
    else if(-p_info.center_proj - it->c.halfSize + it->c.transMat[3][p_info.ax_index] > 0.0f ||
                -p_info.center_proj + it->c.halfSize + it->c.transMat[3][p_info.ax_index] < 0.0f) {
        // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
        return false;
    }
    return true;
}

PlaneInfo SceneManager::intersectBox(Ray &r, std::vector<Object>::iterator it) {
    // https://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
    glm::vec3 center = {it->c.transMat[3].x, it->c.transMat[3].y, it->c.transMat[3].z};
    PlaneInfo p_info = {0, 0.f, 0.f, 0.f, 100000.0f, Planes::left};

    for (int ax_index = 0; ax_index < 3; ax_index++) {
        glm::vec3 axis = {it->c.transMat[ax_index].x, it->c.transMat[ax_index].y, it->c.transMat[ax_index].z}; 
        p_info.center_proj = glm::dot(axis, (center - r.origin)); // Projection of (center - ray_orig) on axis
        p_info.ray_proj = glm::dot(axis, r.dir); // Projection of ray on axis
        p_info.ax_index = ax_index;
        if (!intersectPlanes(axis, it, p_info)) {
            p_info.min_intersect = -1.f;
            return p_info;
        }
    }

	return p_info;
}

PlaneHitInfo SceneManager::RaycastBoxes(Ray r) {
    PlaneInfo   p_info;
    float       shortest_dist = 100.0f;
    Planes      plane = Planes::left;
    std::vector<Object>::iterator intersect_it = m_sc.objects.end();

    for (std::vector<Object>::iterator it = m_sc.objects.begin(); it != m_sc.objects.end(); it++) {
        p_info = intersectBox(r, it);
        if (p_info.min_intersect > -1.f
            && p_info.min_intersect < shortest_dist) {
            shortest_dist = p_info.min_intersect;
            plane = p_info.plane;
            intersect_it = it;
        }
    }
    return {intersect_it, plane};
}

void SceneManager::selectPlane(void) {
    Ray         r = {m_cam.getPos(), m_cam.getLookDir()};
    PlaneHitInfo hit = this->RaycastBoxes(r);

    if (hit.first != m_sc.objects.end() && hit != m_selected_plane && hit.first - m_sc.objects.begin() != 0) {
        // Save previous material index for this plane
        m_prev_selection_mat_idx = hit.first->material_index[static_cast<int>(hit.second)];
        // Assign selection_material_index for this place
        hit.first->material_index[static_cast<int>(hit.second)] = m_selection_mat_idx;
        // Restore previously selected plane's material index
        if (m_selected_plane.first != m_sc.objects.end())
            m_selected_plane.first->material_index[static_cast<int>(m_selected_plane.second)] = m_prev_selection_mat_idx;
        m_selected_plane = hit;
    }
}

void SceneManager::addBox(clock_t last_update, const glm::vec3 &look){
    // Add box to renderer
    glm::vec3 position = m_cam.getPos();
    Object box;
    box.c.transMat = setCenter(position.x, position.y, position.z);
    box.c.halfSize = 1.f;

    int init_materials[6] = {0, 0, 0, 0, 0, 0};
    std::copy(std::begin(init_materials), std::end(init_materials), std::begin(box.material_index));

    box.mass = 5.f;
    m_sc.objects.push_back(box);
    
    // Add box to Bullet Physics
    glm::vec4 origin = box.c.transMat[3];
    btRigidBody *body = m_pm.addBox(origin.x, origin.y, origin.z, box.mass, box.c.halfSize);
    body->setLinearVelocity(btVector3(look.x*5, look.y*5, look.z*5));
}

void SceneManager::addCompositeBox(clock_t last_update, const glm::vec3 &look) {
    // Add boxes to renderer
    glm::vec3 position = m_cam.getPos();
    Object boxA, boxB;
    boxA.c.transMat = setCenter(position.x, position.y, position.z);
    boxB.c.transMat = setCenter(position.x + 2.f, position.y, position.z);
    boxA.c.halfSize = 1.f;
    boxB.c.halfSize = 1.f;

    int init_materials[6] = {0, 0, 0, 0, 0, 0};
    std::copy(std::begin(init_materials), std::end(init_materials), std::begin(boxA.material_index));
    std::copy(std::begin(init_materials), std::end(init_materials), std::begin(boxB.material_index));
    boxA.mass = 5.f;
    boxB.mass = 5.f;
    m_sc.objects.push_back(boxA);
    m_sc.objects.push_back(boxB);

    // Add boxes to Bullet Physics
    glm::vec4 originA = boxA.c.transMat[3];
    glm::vec4 originB = boxB.c.transMat[3];
    btRigidBody *bodyA = m_pm.addBox(originA.x, originA.y, originA.z, boxA.mass, boxA.c.halfSize);
    btRigidBody *bodyB = m_pm.addBox(originB.x, originB.y, originB.z, boxB.mass, boxB.c.halfSize);

    // Stick boxes together
    btVector3 bt_offset = btVector3(1.f, 0.f, 0.f);
    m_pm.stickBox(bodyA, bodyB, bt_offset);

    // body->setLinearVelocity(btVector3(look.x*5, look.y*5, look.z*5));
}

void SceneManager::stickBox(clock_t last_update, const glm::vec3 &look){
    // Get existing box_index
    int box_index = m_selected_plane.first - m_sc.objects.begin();

    // Compute new box pos
    glm::vec3 selected_box_pos = m_selected_plane.first->c.transMat[3];
    int old_box_size = m_selected_plane.first->c.halfSize;
    glm::vec3 offset(0.f);
    offset[static_cast<int>(m_selected_plane.second) / 2] = old_box_size;
    float offset_sign = static_cast<int>(m_selected_plane.second) % 2;
    offset = offset_sign != 0 ? offset * 1.f : offset * -1.f;
    // TODO here: transform offset by transMat
    glm::vec3 new_box_pos = selected_box_pos + 2.f * offset;

    // Add box to renderer
    Object new_box;
    new_box.c.transMat = glm::mat4(m_selected_plane.first->c.transMat);
    new_box.c.transMat[3][0] = new_box_pos.x;
    new_box.c.transMat[3][1] = new_box_pos.y;
    new_box.c.transMat[3][2] = new_box_pos.z;
    new_box.c.halfSize = old_box_size;

    int init_materials[6] = {0, 0, 0, 0, 0, 0};
    std::copy(std::begin(init_materials), std::end(init_materials), std::begin(new_box.material_index));
    new_box.mass = 5.f;
    m_sc.objects.push_back(new_box);

    // Add box to Bullet Physics
    glm::vec4 new_origin = new_box.c.transMat[3];
    btRigidBody *new_body = m_pm.addBox(new_origin.x, new_origin.y, new_origin.z, new_box.mass, new_box.c.halfSize);

    // Get existing box
    btRigidBody *existing_body = m_pm.getBox(box_index);

    // Stick boxes together
    btVector3 bt_offset = btVector3(offset.x, offset.y, offset.z);
    m_pm.stickBox(existing_body, new_body, bt_offset);
}

glm::mat4 SceneManager::setCenter(float x, float y, float z) {
    auto position = glm::mat4(1.f);
    position[3][0] = x;
    position[3][1] = y;
    position[3][2] = z;
    return position;
}

void SceneManager::readScene()
{
    m_sc.clear();

    /* TODO: Create obj file parser
    auto data = reader.getData(); */

    // m_sc.objects = {
    //     /* The ground */
    //     // {{setCenter(0.f, -10.f, 0.f), 10.f}, {1, 1, 1, 1, 1, 1}, 0},
    //     /* Smol Cubes */
    //     {{setCenter(-3.f, 1.f, 2.f), 1.f}, {0, 0, 0, 0, 0, 0}, 1, {0.,0.,0.}},
    //     {{setCenter(-3.f, 6.f, 2.f), 1.f}, {0, 0, 0, 0, 0, 0}, 1, {0.,0.,0.}},
    //     {{setCenter(-2.5f, 1.5f, -2.f), 1.f}, {0, 0, 0, 0, 0, 0}, 1, {0.,0.,0.}},
    //     {{setCenter(4.f, 5.f, -2.f), 1.f}, {0, 0, 0, 0, 0, 0}, 1, {0.,0.,0.}},
    // };
    this->generateWorld(40,12,40);

    std::vector<PointLight> p_lights = {
        {glm::vec4(0.f, 10.f, 0.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 0.5f, 1.f)},
        {glm::vec4(10.f, -10.f, 0.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 3.f, 1.f)},
        {glm::vec4(10.f, 10.f, 0.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 3.f, 1.f)},
    };
    m_sc.pointLights.reserve(p_lights.size());
    for (const auto &pl : p_lights) {
        m_sc.pointLights.push_back(pl);
    }

    glm::vec4 emission_neg = {0.f, 0.f, 0.f, 0.f};
    std::vector<Material> materials = {
        {glm::vec4(1.0f, 0.5f, 0.31f, 0.f), glm::vec4(1.0f, 0.5f, 0.31f, 0.f), glm::vec4(0.5f, 0.5f, 0.5f, 0.f), 32.0f},
        {glm::vec4(0.1, 0.35, 0.75, 0.f), glm::vec4(1, 1, 1, 0.f), emission_neg, 100.f},
        {glm::vec4(0.3f, 1.0f, 0.0f, 0.f), glm::vec4(00.3f, 1.0f, 0.0f, 0.f), glm::vec4(0.5f, 0.5f, 0.5f, 0.f), 32.0f},
    };
    m_sc.materials.reserve(materials.size());
    for (const auto &m : materials) {
        m_sc.materials.push_back(m);
    }
}

Scene &SceneManager::getScene(){
    return m_sc;
}

void SceneManager::generateWorld(int l, int w, int h) {
    Object curr_cube;
    for (int i=-l/2; i < l/2; i+=2) {
        for (int j=-w/2; j < w/2; j+=2) {
            for (int k=-h/2; k < h/2; k+=2) {
                // object_data = map[i][j][k]
                if (j == 0.f) {
                    curr_cube.c = {setCenter(i, j, k), 1.f};
                    int init_materials[6] = {0, 0, 0, 0, 0, 0};
                    std::copy(std::begin(init_materials), std::end(init_materials), std::begin(curr_cube.material_index));
                    curr_cube.mass = 0;
                    m_sc.objects.push_back(curr_cube);
                    if (SHADOW_TEXTURE)
                        this->updateShadowTextureOneBox(m_sc.objects.back());
                }
            }
        }
    }
    // unsigned char *data = new unsigned char[4062500];
    // for (int i = 0; i < 4062500; i++)
    //     data[i] = 1;
    // m_rm.updateShadowTexture(2, -20.0, -6.0, -20.0, 250, 65, 250, data);
    // this->m_rm.getShadowTexture();
    // delete data;
}

void SceneManager::updateShadowTextureOneBox(Object &box) {
    glm::vec3 box_pos = {box.c.transMat[3].x, box.c.transMat[3].y, box.c.transMat[3].z};
    float max_voxel_reach = (m_cube_radius + LOWEST_VOXEL_SIZE/2); // Max dist between box center and intersecting voxel's center
    unsigned char data = 1;

    // Ugly but more precise coarse check;
    // TODO if too slow, replace by regular cube's sphere vs voxel's sphere check
    for (int mip_level = 0; mip_level < 3; mip_level++) {
        std::vector<std::vector<float>> all_candidates;
        glm::fvec3 min_bounds = glm::ceil(box_pos - max_voxel_reach);
        min_bounds -= glm::mod(min_bounds, static_cast<float>(pow(2, mip_level)));
        glm::fvec3 max_bounds = box_pos + max_voxel_reach; 
        for (int axis_index = 0; axis_index < 3; axis_index++) {
            std::vector<float> axis_candidates;
            for (int centroid = min_bounds[axis_index]; centroid <= max_bounds[axis_index]; centroid += pow(2, mip_level)) {
                axis_candidates.push_back(centroid);
            }
            all_candidates.push_back(axis_candidates);
        }
        int total = 0;
        for (auto &x_candidate: all_candidates[0]) {
            for (auto &y_candidate: all_candidates[1]) {
                for (auto &z_candidate: all_candidates[2]) {
                    // TODO: Box-box intersection test to get rid of some candidates
                    // Update Texture
                    m_rm.updateShadowTexture(mip_level, x_candidate, y_candidate, z_candidate, 1, 1, 1, &data);
                    box.containing_voxels[box.containing_voxels_count] = {x_candidate, y_candidate, z_candidate};
                    box.containing_voxels_count += 1;
                    total++;
                }
            }
        }
    }
}

// #################################################
// OBB-OBB Collision funcs
// TODO: Try to simplify with glm
// bool SceneManager::getSeparatingPlane(const vec3& RPos, const vec3& Plane, const Cube& box1, const Cube&box2)
// {
//     // Check if there's a separating plane in between the selected axes
    
//     return (fabs(RPos*Plane) > 
//         (fabs((box1.AxisX*box1.Half_size.x)*Plane) +
//         fabs((box1.AxisY*box1.Half_size.y)*Plane) +
//         fabs((box1.AxisZ*box1.Half_size.z)*Plane) +
//         fabs((box2.AxisX*box2.Half_size.x)*Plane) + 
//         fabs((box2.AxisY*box2.Half_size.y)*Plane) +
//         fabs((box2.AxisZ*box2.Half_size.z)*Plane)));
// }

// bool SceneManager::getCollision(const Cube& box1, const Cube&box2)
// {
//     // test for separating planes in all 15 axes
//     static glm::vec3 RPos = box2.Pos - box1.Pos;

//     return !(getSeparatingPlane(RPos, box1.AxisX, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisY, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisZ, box1, box2) ||
//         getSeparatingPlane(RPos, box2.AxisX, box1, box2) ||
//         getSeparatingPlane(RPos, box2.AxisY, box1, box2) ||
//         getSeparatingPlane(RPos, box2.AxisZ, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisX^box2.AxisX, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisX^box2.AxisY, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisX^box2.AxisZ, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisY^box2.AxisX, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisY^box2.AxisY, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisY^box2.AxisZ, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisZ^box2.AxisX, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisZ^box2.AxisY, box1, box2) ||
//         getSeparatingPlane(RPos, box1.AxisZ^box2.AxisZ, box1, box2));
// }