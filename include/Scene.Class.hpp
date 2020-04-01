#ifndef SCENE_CLASS_HPP
# define SCENE_CLASS_HPP

# include <vector>
# include <glm/glm.hpp>

///////////// Structs that will be transfered to the GPU /////////////

//Alignment: 16 bytes, therefore two vec4 vectors
struct PointLight
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 attenuation;
};

struct DirectionalLight
{
    glm::vec3 direction;
    glm::vec3 color;
    glm::vec3 attenuation;
};
// The number of attributes in the struct. No Reflection yet in C++.
// Will be needed for uploading the scene in the SceneManager.
constexpr unsigned int NumAttributesLights = 3;

//Alignment will be 16 bytes therefore take two vec4 vectors
struct Cube
{
    glm::mat4 transMat;
    glm::vec3 min;
    glm::vec3 max;
};

struct Material
{
    glm::vec3   diffuse;
    glm::vec3   specularity;
    glm::vec3   emission;
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
    std::vector<glm::vec3> vertices;
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
        vertices.clear();
        cubes.clear();
        pointLights.clear();
        directionalLights.clear();
        materials.clear();
    }
};

#endif