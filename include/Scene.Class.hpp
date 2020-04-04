#ifndef SCENE_CLASS_HPP
# define SCENE_CLASS_HPP

# include <vector>
# include <glm/glm.hpp>

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
// The number of attributes in the struct. No Reflection yet in C++.
// Will be needed for uploading the scene in the SceneManager.
constexpr unsigned int NumAttributesLights = 3;

//Alignment will be 16 bytes therefore take two vec4 vectors
struct Cube
{
    glm::mat4 transMat;
    glm::vec4 min;
    glm::vec4 max;
};

struct Material
{
    glm::vec4   diffuse;
    glm::vec4   specularity;
    glm::vec4   emission;
    float       shininess;
};
constexpr unsigned int NumAttributesMaterial = 4;

struct Object
{
    Cube c;
    int material_index;
};
constexpr unsigned int NumAttributesObjects = 4;

struct Scene
{
    std::vector<std::pair<Cube, int>> cubes;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::vector<Material> materials;

    int numObjects()
    {
        return cubes.size();
    }

    int numLights()
    {
        return pointLights.size() + directionalLights.size();
    }

    void clear()
    {
        cubes.clear();
        pointLights.clear();
        directionalLights.clear();
        materials.clear();
    }
};

#endif