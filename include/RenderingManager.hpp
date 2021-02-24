#ifndef SCENEMANAGER_CLASS_HPP
# define SCENEMANAGER_CLASS_HPP
# define SHADOW_TEXTURE true

# pragma once
# include <glad/glad.h>
# include <glm/glm.hpp>
# include <string>
# include <cstring>
# include <vector>
# include "Scene.Class.hpp"

class RenderingManager
{
public:
    RenderingManager(GLuint computeShaderID, int stex_width, int stex_height, int stex_depth);
    ~RenderingManager();
    RenderingManager() = delete;
    RenderingManager(RenderingManager &) = delete;
    RenderingManager &operator=(RenderingManager &) = delete;

    void uploadShadowTexture(unsigned char *data, unsigned int len);
    void uploadScene(Scene &sc);
    void uploadObjects(Scene &sc);
    Object addBox(float x, float y, float z);
    unsigned int    getStexWidth();
    unsigned int    getStexHeight();
    unsigned int    getStexDepth();
    unsigned int    getStexSize();
    GLuint          getShadowTexID();

private:
    GLuint          m_computeShaderID;
    GLuint          m_shadowTexID;
    GLuint          *m_storageBufferIDs;
    unsigned int    m_numObjInShader = 0;
    unsigned int    m_numMaterialsInShader = 0;
    unsigned int    m_numLightsInShader = 0;
    unsigned int    m_reflectionDepth = 0;

    const unsigned int m_stex_width;
    const unsigned int m_stex_height;
    const unsigned int m_stex_depth;

    int m_oBlockIndex = 0;
    int m_mBlockIndex = 0;
    int m_lBlockIndex = 0;

    int m_oAlignOffset = 0;
    int m_mAlignOffset = 0;
    int m_lAlignOffset = 0;

    int *m_oOffsets = nullptr;
    int *m_mOffsets = nullptr;
    int *m_lOffsets = nullptr;

    void initShadowTex();
    void initialize();
    void uploadMaterials(Scene &sc);
    void uploadLights(Scene &sc);

};

#endif