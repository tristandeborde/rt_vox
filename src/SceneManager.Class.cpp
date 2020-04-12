#include "SceneManager.Class.hpp"
#include "Shader.Class.hpp"
#include <stdlib.h>
#include <time.h>

void SceneManager::readScene(const std::string &filepath)
{
    m_scene.clear();

    /* TODO: Create obj file parser
    auto data = reader.getData(); */

    srand(time(0));
    std::vector<Cube> cubes = {
        /* The ground */
        {glm::mat4(1.f), glm::vec4(-5.0f, -0.1f, -5.0f, 1.f), glm::vec4(5.0f, 0.0f, 5.0f, 1.f)},
        /* Smol Cubes */
        {glm::mat4(1.f), glm::vec4(-0.5f, 0.0f, -0.5f, 1.f), glm::vec4(0.5f, 1.0f, 0.5f, 1.f)},
        {glm::mat4(1.f), glm::vec4(-3.5f, 0.5f, 1.5f, 1.f), glm::vec4(-2.5f, 1.5f, 2.5f, 1.f)},
        {glm::mat4(1.f), glm::vec4(-3.f, 2.0f, -3.8f, 1.f), glm::vec4(-2.f, 3.0f, -2.8f, 1.f)},
        {glm::mat4(1.f), glm::vec4(-2.1f, 0.0f, -2.5f, 1.f), glm::vec4(-1.1f, 1.0f, -1.5f, 1.f)}
    };
    m_scene.cubes.reserve(cubes.size());
    for (const auto &c : cubes) {
        int material_index = rand() % 2;
        m_scene.cubes.push_back(std::make_pair(c, material_index));
    }

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

void SceneManager::initialize(GLuint computeShaderID)
{
    const GLchar* oNames[] = {"objects[0].c.transMat", "objects[0].c.min", "objects[0].c.max", "objects[0].material_index"};
    const GLchar* mNames[] = {"materials[0].diffuse", "materials[0].specularity", "materials[0].emission", "materials[0].shininess"};
    const GLchar* lNames[] = {"lights[0].pos_dir", "lights[0].color", "lights[0].attenuation"};

    auto oIndices = new int[NumAttributesObjects + 1];
    auto mIndices = new int[NumAttributesMaterial + 1];
    auto lIndices = new int[NumAttributesLights];
    m_oOffsets = new int[NumAttributesObjects + 1];
    m_mOffsets = new int[NumAttributesMaterial + 1];
    m_lOffsets = new int[NumAttributesLights];

    const GLenum props[] = {GL_BUFFER_DATA_SIZE};
    const GLenum props2[] = {GL_OFFSET};

    // Get index (GLuint, not an offset) of each attribute (BUFFER_VARIABLE) relative to its struct
    auto getIndices = [&](int length, const GLchar** const names, GLint* indices){
        for (int i = 0; i < length; ++i) {
            indices[i] = glGetProgramResourceIndex(computeShaderID, GL_BUFFER_VARIABLE, names[i]);
        }
    };

    getIndices(NumAttributesObjects, oNames, oIndices);
    getIndices(NumAttributesMaterial, mNames, mIndices);
    getIndices(NumAttributesLights, lNames, lIndices);

    // Get index (GLuint, not an offset) of each of the 3 buffers
    m_oBlockIndex = glGetProgramResourceIndex(computeShaderID, GL_SHADER_STORAGE_BLOCK, "PrimitiveBuffer");
    m_mBlockIndex = glGetProgramResourceIndex(computeShaderID, GL_SHADER_STORAGE_BLOCK, "MaterialBuffer");
    m_lBlockIndex = glGetProgramResourceIndex(computeShaderID, GL_SHADER_STORAGE_BLOCK, "LightBuffer");

    // Get offset in buffer for each attribute of one variable (e.g., objects[0].b.max == 1*sizeof(glm::vec4))
    for (unsigned int i = 0; i < NumAttributesMaterial; ++i) {
        glGetProgramResourceiv(computeShaderID, GL_BUFFER_VARIABLE, mIndices[i], 1, props2, 1, NULL, &m_mOffsets[i]);
    }

    for (unsigned int i = 0; i < NumAttributesObjects; ++i) {
        glGetProgramResourceiv(computeShaderID, GL_BUFFER_VARIABLE, oIndices[i], 1, props2, 1, NULL, &m_oOffsets[i]);
    }

    for (unsigned int i = 0; i < NumAttributesLights; ++i) {
        glGetProgramResourceiv(computeShaderID, GL_BUFFER_VARIABLE, lIndices[i], 1, props2, 1, NULL, &m_lOffsets[i]);
    }

    int oBlockSize = 0;
    int mBlockSize = 0;
    int lBlockSize = 0;
    // Get size of one element in each buffer (one Light, one Object, etc.).
    glGetProgramResourceiv(computeShaderID, GL_SHADER_STORAGE_BLOCK, m_oBlockIndex, 1, props, 1, NULL, &oBlockSize);
    glGetProgramResourceiv(computeShaderID, GL_SHADER_STORAGE_BLOCK, m_mBlockIndex, 1, props, 1, NULL, &mBlockSize);
    glGetProgramResourceiv(computeShaderID, GL_SHADER_STORAGE_BLOCK, m_lBlockIndex, 1, props, 1, NULL, &lBlockSize);

    m_oAlignOffset = (oBlockSize%16 == 0 ? oBlockSize : oBlockSize-(oBlockSize%16)+16);
    m_mAlignOffset = (mBlockSize%16 == 0 ? mBlockSize : mBlockSize-(mBlockSize%16)+16);
    m_lAlignOffset = (lBlockSize%16 == 0 ? lBlockSize : lBlockSize-(lBlockSize%16)+16);
}

void SceneManager::uploadScenes(const std::vector<std::string> &filepaths, GLuint computeShaderID, GLuint* computeShaderstorageBufferIDs)
{
    initialize(computeShaderID);

    // TODO: replace by loop over all filepaths to be parsed
    std::cout << "Initializing scene..." << '\n';
    this->readScene(filepaths[0]);
    std::cout << "Starting scene upload..." << '\n';
    this->uploadScene(computeShaderID, computeShaderstorageBufferIDs);
    std::cout << "Completed scene upload.\n\n";
}

void SceneManager::uploadScene(GLuint computeShaderID, GLuint* computeShaderstorageBufferIDs)
{
    ////////////////////////////////////////////////////// LOADING OBJECTS //////////////////////////////////////////////////////
    void* p = malloc(m_numObjInShader * m_oAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeShaderstorageBufferIDs[0]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numObjInShader * m_oAlignOffset, (void *)(p));

    // Allocate a Shaderstorage buffer on the GPU memory
    if (m_numObjInShader * m_oAlignOffset == 0) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numObjInShader + m_scene.numObjects()) * m_oAlignOffset, NULL, GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numObjInShader + m_scene.numObjects()) * m_oAlignOffset, p, GL_DYNAMIC_DRAW);
        free(p);
    }

    if (m_scene.numObjects() != 0) {
        // glMapBuffer is used to get a pointer to the GPU memory 
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &c : m_scene.cubes) {
            const auto matIdx = c.second + m_numMaterialsInShader;

            std::memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[0], &(c.first.transMat) , sizeof(glm::mat4));
            std::memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[1], &(c.first.min) , sizeof(glm::vec4));
            std::memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[2], &(c.first.max) , sizeof(glm::vec4));
            std::memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[4], &(matIdx) , sizeof(int));
            m_numObjInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(computeShaderID, m_oBlockIndex, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ////////////////////////////////////////////////////// LOADING MATERIALS //////////////////////////////////////////////////////
    p = malloc(m_numMaterialsInShader * m_mAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeShaderstorageBufferIDs[1]);
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
        glShaderStorageBlockBinding(computeShaderID, m_mBlockIndex, 1);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ////////////////////////////////////////////////////// LOADING LIGHTS //////////////////////////////////////////////////////
    p = malloc(m_numLightsInShader * m_lAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, computeShaderstorageBufferIDs[2]);
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
        glShaderStorageBlockBinding(computeShaderID, m_lBlockIndex, 2);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // TODO: use these in rt.comp
    glUseProgram(computeShaderID);
    auto uniID = glGetUniformLocation(computeShaderID, "numObj");
    glUniform1ui(uniID, m_numObjInShader);
    uniID = glGetUniformLocation(computeShaderID, "numLights");
    glUniform1ui(uniID, m_numLightsInShader);
    uniID = glGetUniformLocation(computeShaderID, "reflectionDepth");
    glUniform1ui(uniID, m_reflectionDepth);
}