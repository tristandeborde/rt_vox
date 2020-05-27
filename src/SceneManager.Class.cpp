#include "SceneManager.Class.hpp"
#include "Shader.Class.hpp"
#include <stdlib.h>
#include <time.h>

SceneManager::SceneManager(GLuint computeShaderID):
    m_computeShaderID(computeShaderID){
    this->initialize();
}

SceneManager::~SceneManager() {
    return;
}

glm::mat4 setCenter(float x, float y, float z) {
    auto position = glm::mat4(1.f);
    position[3][0] = x;
    position[3][1] = y;
    position[3][2] = z;
    return position;
}

void SceneManager::addBox(float x, float y, float z)
{
    Object object;
    object.c.transMat = setCenter(x, y, z);
    object.c.min = glm::vec4(-0.5f, -0.5f, -0.5f, 1.f);
    object.c.max = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
    object.material_index = 2;
    object.mass = 5.f;
    m_scene.objects.push_back(object);
}


void SceneManager::readScene()
{
    m_scene.clear();

    /* TODO: Create obj file parser
    auto data = reader.getData(); */

    srand(time(0));
    m_scene.objects = {
        /* The ground */
        {{setCenter(0.f, 0.f, 0.f), glm::vec4(-5.0f, -0.2f, -5.0f, 1.f), glm::vec4(5.0f, 0.2f, 5.0f, 1.f)}, 1, 0},
        /* Smol Cubes */
        {{setCenter(-3.f, 1.f, 2.f), glm::vec4(-0.5f, -0.5f, -0.5f, 1.f), glm::vec4(0.5f, 0.5f, 0.5f, 1.f)}, 2, 5},
        {{setCenter(-3.f, 6.f, 2.f), glm::vec4(-0.5f, -0.5f, -0.5f, 1.f), glm::vec4(0.5f, 0.5f, 0.5f, 1.f)}, 1, 5},
        {{setCenter(-2.5f, 1.5f, -2.f), glm::vec4(-0.5f, -0.5f, -0.5f, 1.f), glm::vec4(0.5f, 0.5f, 0.5f, 1.f)}, 2, 5},
        {{setCenter(4.f, 5.f, -2.f), glm::vec4(-0.5f, -0.5f, -0.5f, 1.f), glm::vec4(0.5f, 0.5f, 0.5f, 1.f)}, 1, 5}
    };

    std::vector<PointLight> p_lights = {
        {glm::vec4(0.f, 10.f, 0.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 0.5f, 1.f)},
        {glm::vec4(10.f, -10.f, 0.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 3.f, 1.f)},
        {glm::vec4(10.f, 10.f, 0.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 3.f, 1.f)},
    };
    m_scene.pointLights.reserve(p_lights.size());
    for (const auto &pl : p_lights) {
        m_scene.pointLights.push_back(pl);
    }

    glm::vec4 emission_neg = {0.f, 0.f, 0.f, 0.f};
    std::vector<Material> materials = {
        {glm::vec4(0.1, 0.35, 0.75, 0.f), glm::vec4(0.7, 0.7, 0.7, 0.f), emission_neg, 20.f},
        {glm::vec4(0.1, 0.35, 0.75, 0.f), glm::vec4(1, 1, 1, 0.f), emission_neg, 100.f},
    };
    m_scene.materials.reserve(materials.size());
    for (const auto &m : materials) {
        m_scene.materials.push_back(m);
    }
}

void SceneManager::initialize()
{
    const GLchar* oNames[] = {"objects[0].c.transMat", "objects[0].c.min", "objects[0].c.max", "objects[0].material_index"};
    const GLchar* mNames[] = {"materials[0].diffuse", "materials[0].specularity", "materials[0].emission", "materials[0].shininess"};
    const GLchar* lNames[] = {"lights[0].pos_dir", "lights[0].color", "lights[0].attenuation"};

    auto oIndices = new int[Scene::m_NumAttributesObjects + 1];
    auto mIndices = new int[Scene::m_NumAttributesMaterial + 1];
    auto lIndices = new int[Scene::m_NumAttributesLights];
    m_oOffsets = new int[Scene::m_NumAttributesObjects + 1];
    m_mOffsets = new int[Scene::m_NumAttributesMaterial + 1];
    m_lOffsets = new int[Scene::m_NumAttributesLights];

    const GLenum props[] = {GL_BUFFER_DATA_SIZE};
    const GLenum props2[] = {GL_OFFSET};

    // Get index (GLuint, not an offset) of each attribute (BUFFER_VARIABLE) relative to its struct
    auto getIndices = [&](int length, const GLchar** const names, GLint* indices){
        for (int i = 0; i < length; ++i) {
            indices[i] = glGetProgramResourceIndex(m_computeShaderID, GL_BUFFER_VARIABLE, names[i]);
        }
    };

    getIndices(Scene::m_NumAttributesObjects, oNames, oIndices);
    getIndices(Scene::m_NumAttributesMaterial, mNames, mIndices);
    getIndices(Scene::m_NumAttributesLights, lNames, lIndices);

    // Get index (GLuint, not an offset) of each of the 3 buffers
    m_oBlockIndex = glGetProgramResourceIndex(m_computeShaderID, GL_SHADER_STORAGE_BLOCK, "PrimitiveBuffer");
    m_mBlockIndex = glGetProgramResourceIndex(m_computeShaderID, GL_SHADER_STORAGE_BLOCK, "MaterialBuffer");
    m_lBlockIndex = glGetProgramResourceIndex(m_computeShaderID, GL_SHADER_STORAGE_BLOCK, "LightBuffer");

    // Get offset in buffer for each attribute of one variable (e.g., objects[0].b.max == 1*sizeof(glm::vec4))
    for (unsigned int i = 0; i < Scene::m_NumAttributesMaterial; ++i) {
        glGetProgramResourceiv(m_computeShaderID, GL_BUFFER_VARIABLE, mIndices[i], 1, props2, 1, NULL, &m_mOffsets[i]);
    }

    for (unsigned int i = 0; i < Scene::m_NumAttributesObjects; ++i) {
        glGetProgramResourceiv(m_computeShaderID, GL_BUFFER_VARIABLE, oIndices[i], 1, props2, 1, NULL, &m_oOffsets[i]);
    }

    for (unsigned int i = 0; i < Scene::m_NumAttributesLights; ++i) {
        glGetProgramResourceiv(m_computeShaderID, GL_BUFFER_VARIABLE, lIndices[i], 1, props2, 1, NULL, &m_lOffsets[i]);
    }

    int oBlockSize = 0;
    int mBlockSize = 0;
    int lBlockSize = 0;
    // Get size of one element in each buffer (one Light, one Object, etc.).
    glGetProgramResourceiv(m_computeShaderID, GL_SHADER_STORAGE_BLOCK, m_oBlockIndex, 1, props, 1, NULL, &oBlockSize);
    glGetProgramResourceiv(m_computeShaderID, GL_SHADER_STORAGE_BLOCK, m_mBlockIndex, 1, props, 1, NULL, &mBlockSize);
    glGetProgramResourceiv(m_computeShaderID, GL_SHADER_STORAGE_BLOCK, m_lBlockIndex, 1, props, 1, NULL, &lBlockSize);

    m_oAlignOffset = (oBlockSize%16 == 0 ? oBlockSize : oBlockSize-(oBlockSize%16)+16);
    m_mAlignOffset = (mBlockSize%16 == 0 ? mBlockSize : mBlockSize-(mBlockSize%16)+16);
    m_lAlignOffset = (lBlockSize%16 == 0 ? lBlockSize : lBlockSize-(lBlockSize%16)+16);

    m_storageBufferIDs = new GLuint[3];
    glGenBuffers(3, m_storageBufferIDs);
}

void SceneManager::uploadScene()
{
    this->uploadObjects();
    this->uploadMaterials();
    this->uploadLights();
}

void SceneManager::uploadObjects() {
    ////////////////////////////////////////////////////// LOADING OBJECTS //////////////////////////////////////////////////////
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_storageBufferIDs[0]);
    // TODO: Copy *existing* buffer data from GPU to CPU, in order to update cleverly only a single cube in whole buffer 
    // glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_scene.objects.size() * m_oAlignOffset, (void *)(p));

    // Allocate a Shaderstorage buffer on the GPU memory
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_scene.objects.size() * m_oAlignOffset, NULL, GL_DYNAMIC_DRAW);

    m_numObjInShader = 0;
    if (m_scene.numObjects() != 0) {
        // glMapBuffer is used to get a pointer to the GPU memory 
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &obj : m_scene.objects) {
            std::memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[0], &(obj.c.transMat) , sizeof(glm::mat4));
            std::memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[1], &(obj.c.min) , sizeof(glm::vec4));
            std::memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[2], &(obj.c.max) , sizeof(glm::vec4));
            std::memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[4], &(obj.material_index) , sizeof(int));
            m_numObjInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_computeShaderID, m_oBlockIndex, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}

void SceneManager::uploadMaterials() {
    ////////////////////////////////////////////////////// LOADING MATERIALS //////////////////////////////////////////////////////
    void *p = malloc(m_numMaterialsInShader * m_mAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_storageBufferIDs[1]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numMaterialsInShader * m_mAlignOffset, p);
    if (m_numMaterialsInShader * m_mAlignOffset == 0) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numMaterialsInShader + m_scene.materials.size()) * m_mAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numMaterialsInShader + m_scene.materials.size()) * m_mAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if (!m_scene.materials.empty()) {
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &mt : m_scene.materials) {
            memcpy(ptr + m_numMaterialsInShader * m_mAlignOffset + m_mOffsets[0], &(mt.diffuse) , sizeof(glm::vec4));
            memcpy(ptr + m_numMaterialsInShader * m_mAlignOffset + m_mOffsets[1], &(mt.specularity) , sizeof(glm::vec4));
            memcpy(ptr + m_numMaterialsInShader * m_mAlignOffset + m_mOffsets[2], &(mt.emission) , sizeof(glm::vec4));
            memcpy(ptr + m_numMaterialsInShader * m_mAlignOffset + m_mOffsets[3], &(mt.shininess) , sizeof(float));
            m_numMaterialsInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_computeShaderID, m_mBlockIndex, 1);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    free(p);
}

void SceneManager::uploadLights() {
    ////////////////////////////////////////////////////// LOADING LIGHTS //////////////////////////////////////////////////////
    void *p = malloc(m_numLightsInShader * m_lAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_storageBufferIDs[2]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numLightsInShader * m_lAlignOffset, p);
    if (m_numLightsInShader * m_lAlignOffset == 0) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_scene.numLights() * m_lAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numLightsInShader + m_scene.numLights()) * m_lAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if (m_scene.numLights() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &pl : m_scene.pointLights) {
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[0], &(pl.position) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[1], &(pl.color) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[2], &(pl.attenuation) , sizeof(glm::vec4));
            m_numLightsInShader++;
        }

        for (const auto &dl : m_scene.directionalLights) {
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[0], &(dl.direction) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[1], &(dl.color) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[2], &(dl.attenuation) , sizeof(glm::vec4));
            m_numLightsInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_computeShaderID, m_lBlockIndex, 2);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    free(p);

    glUseProgram(m_computeShaderID);
    auto uniID = glGetUniformLocation(m_computeShaderID, "numObj");
    glUniform1ui(uniID, m_numObjInShader);
    uniID = glGetUniformLocation(m_computeShaderID, "numLights");
    glUniform1ui(uniID, m_numLightsInShader);
    uniID = glGetUniformLocation(m_computeShaderID, "reflectionDepth");
    glUniform1ui(uniID, m_reflectionDepth);
}

Scene &SceneManager::getScene(){
    return m_scene;
}