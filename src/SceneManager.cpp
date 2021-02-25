#include "SceneManager.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <cmath>
#define CUBE_SIZE 5 // in centimeters
#define LOWEST_VOXEL_SIZE CUBE_SIZE*2 // in centimeters


SceneManager::SceneManager(Camera &cam, PhysicsManager &pm, RenderingManager &rm)
    : m_cam(cam), m_pm(pm), m_rm(rm) {
    m_sc = Scene();
    this->readScene();
    m_selection_mat_idx = 2;
    m_selected_plane = std::make_pair(m_sc.objects.end(), Planes::left);
    m_cube_radius = glm::sqrt(2*(pow(CUBE_SIZE/2)));
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
    std::cout << offset_sign << std::endl;
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
    std::cout << box_index << std::endl;
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

    srand(time(0));
    m_sc.objects = {
        /* The ground */
        // {{setCenter(0.f, -10.f, 0.f), 10.f}, {1, 1, 1, 1, 1, 1}, 0},
        /* Smol Cubes */
        {{setCenter(-3.f, 1.f, 2.f), 1.f}, {0, 0, 0, 0, 0, 0}, 1},
        {{setCenter(-3.f, 6.f, 2.f), 1.f}, {0, 0, 0, 0, 0, 0}, 1},
        {{setCenter(-2.5f, 1.5f, -2.f), 1.f}, {0, 0, 0, 0, 0, 0}, 1},
        {{setCenter(4.f, 5.f, -2.f), 1.f}, {0, 0, 0, 0, 0, 0}, 1}
    };
    this->generateWorld(40,10,40);

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
    for (int i=-20.f; i < l; i+=2) {
        for (int j=-4.f; j < w; j+=2) {
            for (int k=-20.f; k < h; k+=2) {
                // object_data = map[i][j][k]
                if (j == 0.f) {
                    m_sc.objects.push_back({{setCenter(i, j, k), 1.f}, {0, 0, 0, 0, 0, 0}, 0});
                if (SHADOW_TEXTURE)
                    this->updateEntireShadowTexture(glm::vec3(i, j, k));
                }
            }
        }
    }
}

void SceneManager::updateEntireShadowTexture(glm::vec3 vox_pos) {
    std::vector<std::vector<float>> &centroid_set = m_all_centroids[0]
    std::vector<float> &x_axis_centroids = centroid_set[0];

    auto start = std::lower_bound(x_axis_centroids.begin(), x_axis_centroids.end(), vox_pos - m_cube_radius);
    auto end = std::upper_bound(x_axis_centroids.begin(), x_axis_centroids.end(), vox_pos - m_cube_radius);
    for (auto it = start; it != end; it++) {
        std::cout << *it << " ";
    }
    

    // Update texture
    m_rm.updateShadowTexture(0, x_offset, y_offset, z_offset, data));
    m_rm.updateShadowTexture(1, x_offset, y_offset, z_offset, data));
    m_rm.updateShadowTexture(2, x_offset, y_offset, z_offset, data));
}

void SceneManager::initAllCentroids(int level_count) {
    // Initialize all possible centroids used for shadowtexture rasterization
    for (int level; level < level_count; level++) {
        m_all_centroids.push_back(this->initCentroid(level));
    }
}

std::vector<std::vector<float>> SceneManager::initCentroid(int level) {
    std::vector<std::vector<float>> centroids_set;

    int curr_voxel_len = LOWEST_VOXEL_SIZE * pow(2, level);
    int voxels_per_meter = 100 / curr_voxel_len;
    std::vector<float> x_axis_centroids(m_rm.getStexWidth()*voxels_per_meter);
    centroids_set.push_back(x_axis_centroids);
    std::vector<float> y_axis_centroids(m_rm.getStexHeight()*voxels_per_meter);
    centroids_set.push_back(y_axis_centroids);
    std::vector<float> z_axis_centroids(m_rm.getStexDepth()*voxels_per_meter);
    centroids_set.push_back(z_axis_centroids);
    
    float curr_pos;
    for (auto &centroid_axis: centroids_set) {
        curr_pos = -(m_rm.getStexWidth()/2-curr_voxel_len/2); // Starting point of centroid vector
        for (auto &centroid: centroid_axis) {
            centroid = curr_pos;
            curr_pos += curr_voxel_len;
        }
    }
    return centroids_set;
}