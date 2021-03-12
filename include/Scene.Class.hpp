#ifndef SCENE_CLASS_HPP
# define SCENE_CLASS_HPP

# include <vector>
# include <glm/glm.hpp>
# include <limits>
///////////// Structs that will be transfered to the GPU /////////////

//Alignment: 16 bytes, therefore two vec4 vectors
struct PointLight
{
    glm::vec4 position;
    glm::vec4 color;
    glm::vec4 attenuation;
};

struct DirectionalLight
{
    glm::vec4 direction;
    glm::vec4 color;
    glm::vec4 attenuation;
};

//Alignment will be 16 bytes therefore take two vec4 vectors
struct Cube
{
    glm::mat4   transMat;
    float       halfSize;
};

struct Material
{
    glm::vec4   diffuse;
    glm::vec4   specularity;
    glm::vec4   emission;
    float       shininess;
};

struct Object
{
    Object() {
        this->containing_voxels_count = 0;
        for(int i = 0; i < 8; i++) {
            this->containing_voxels[i] = glm::vec3(std::numeric_limits<int>::max());
        };
    };
    Cube        c;
    int         material_index[6];
    glm::vec3   containing_voxels[8]; // Changing order will break padding!
    int         containing_voxels_count;
    float       mass;
};

struct Scene
{
    std::vector<Object> objects;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::vector<Material> materials;

    // The number of attributes in the struct. No Reflection yet in C++.
    // Will be needed for uploading the scene in the SceneManager.
    static const unsigned int m_NumAttributesLights = 3;
    static const unsigned int m_NumAttributesMaterial = 4;
    static const unsigned int m_NumAttributesObjects = 5;


    unsigned int numObjects()
    {
        return objects.size();
    }

    unsigned int numLights()
    {
        return pointLights.size() + directionalLights.size();
    }

    void clear()
    {
        objects.clear();
        pointLights.clear();
        directionalLights.clear();
        materials.clear();
    }
};

#endif