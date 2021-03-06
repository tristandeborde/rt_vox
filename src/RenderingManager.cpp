#include "RenderingManager.hpp"
#include "Shader.Class.hpp"
#include <stdlib.h>
#include <time.h>

RenderingManager::RenderingManager(GLuint computeShaderID, int shadow_tex_width, int shadow_tex_height, int shadow_tex_depth)
    :   m_computeShaderID(computeShaderID),
        m_stex_width(shadow_tex_width),
        m_stex_height(shadow_tex_height),
        m_stex_depth(shadow_tex_depth) {
    this->initialize();
}

RenderingManager::~RenderingManager() {
    return;
}

void RenderingManager::initialize()
{
    const GLchar* oNames[] = {"objects[0].c.transMat", "objects[0].c.halfSize", "objects[0].material_index"};
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

    if (SHADOW_TEXTURE)
        this->initShadowTex();
}

void RenderingManager::initShadowTex() {
    glGenTextures(1, &m_shadowTexID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_shadowTexID);
    glTexStorage3D(GL_TEXTURE_3D, 3, GL_R8, m_stex_width, m_stex_height, m_stex_depth);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void RenderingManager::updateShadowTexture(int level, int x_offset, int y_offset, int z_offset, int width, int height, int depth, unsigned char *data) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_shadowTexID);
    glTexSubImage3D(GL_TEXTURE_3D, level, x_offset, y_offset, z_offset, width, height, depth, GL_RED, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void RenderingManager::uploadScene(Scene &sc)
{
    this->uploadObjects(sc);
    this->uploadMaterials(sc);
    this->uploadLights(sc);
}

void RenderingManager::uploadObjects(Scene &sc) {
    ////////////////////////////////////////////////////// LOADING OBJECTS //////////////////////////////////////////////////////
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_storageBufferIDs[0]);
    // TODO: Copy *existing* buffer data from GPU to CPU, in order to update cleverly only a single cube in whole buffer 
    // glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sc.objects.size() * m_oAlignOffset, (void *)(p));
    // glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numMaterialsInShader * m_mAlignOffset, p);
    // if (m_numMaterialsInShader * m_mAlignOffset == 0) {
    //     glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numMaterialsInShader + sc.materials.size()) * m_mAlignOffset, NULL, GL_STATIC_DRAW);
    // } else {
    //     glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numMaterialsInShader + sc.materials.size()) * m_mAlignOffset, p, GL_STATIC_DRAW);
    //     free(p);
    // }

    // Allocate a Shaderstorage buffer on the GPU memory
    glBufferData(GL_SHADER_STORAGE_BUFFER, sc.objects.size() * m_oAlignOffset, NULL, GL_DYNAMIC_DRAW);

    m_numObjInShader = 0;
    if (sc.numObjects() != 0) {
        // glMapBuffer is used to get a pointer to the GPU memory
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &obj : sc.objects) {
            int block_offset = m_numObjInShader * m_oAlignOffset;
            std::memcpy(ptr + block_offset + m_oOffsets[0], &(obj.c.transMat), sizeof(glm::mat4));
            std::memcpy(ptr + block_offset + m_oOffsets[1], &(obj.c.halfSize), sizeof(float));
            std::memcpy(ptr + block_offset + m_oOffsets[2], obj.material_index, sizeof(int) * 6);
            m_numObjInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_computeShaderID, m_oBlockIndex, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    glUseProgram(m_computeShaderID);
    auto uniID = glGetUniformLocation(m_computeShaderID, "numObj");
    glUniform1ui(uniID, m_numObjInShader);
}

void RenderingManager::uploadMaterials(Scene &sc) {
    ////////////////////////////////////////////////////// LOADING MATERIALS //////////////////////////////////////////////////////
    void *p = malloc(m_numMaterialsInShader * m_mAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_storageBufferIDs[1]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numMaterialsInShader * m_mAlignOffset, p);
    if (m_numMaterialsInShader * m_mAlignOffset == 0) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numMaterialsInShader + sc.materials.size()) * m_mAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numMaterialsInShader + sc.materials.size()) * m_mAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if (!sc.materials.empty()) {
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &mt : sc.materials) {
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

void RenderingManager::uploadLights(Scene &sc) {
    ////////////////////////////////////////////////////// LOADING LIGHTS //////////////////////////////////////////////////////
    void *p = malloc(m_numLightsInShader * m_lAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_storageBufferIDs[2]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numLightsInShader * m_lAlignOffset, p);
    if (m_numLightsInShader * m_lAlignOffset == 0) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, sc.numLights() * m_lAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numLightsInShader + sc.numLights()) * m_lAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if (sc.numLights() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &pl : sc.pointLights) {
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[0], &(pl.position) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[1], &(pl.color) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[2], &(pl.attenuation) , sizeof(glm::vec4));
            m_numLightsInShader++;
        }

        for (const auto &dl : sc.directionalLights) {
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
    auto uniID = glGetUniformLocation(m_computeShaderID, "numLights");
    glUniform1ui(uniID, m_numLightsInShader);
    uniID = glGetUniformLocation(m_computeShaderID, "reflectionDepth");
    glUniform1ui(uniID, m_reflectionDepth);
}

unsigned int RenderingManager::getStexWidth() {
    return m_stex_width;
}

unsigned int RenderingManager::getStexHeight() {
    return m_stex_height;
}

unsigned int RenderingManager::getStexDepth() {
    return m_stex_depth;
}

glm::vec3 RenderingManager::getStexDims() {
    return glm::vec3(m_stex_width, m_stex_height, m_stex_depth);
}

unsigned int RenderingManager::getStexSize() {
    return m_stex_width*m_stex_height*m_stex_depth;
}

GLuint      RenderingManager::getShadowTexID() {
    return m_shadowTexID;
}