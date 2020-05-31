#ifndef SCENEMANAGER_CLASS_HPP
# define SCENEMANAGER_CLASS_HPP

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
    RenderingManager(GLuint computeShaderID);
    ~RenderingManager();
    RenderingManager() = delete;
    RenderingManager(RenderingManager &) = delete;
    RenderingManager &operator=(RenderingManager &) = delete;

    void uploadScene(Scene &sc);
    void uploadObjects(Scene &sc);
    Object addBox(float x, float y, float z);

private:
    void initialize();
    GLuint          m_computeShaderID;
    GLuint          *m_storageBufferIDs;
    unsigned int    m_numObjInShader = 0;
    unsigned int    m_numMaterialsInShader = 0;
    unsigned int    m_numLightsInShader = 0;
    unsigned int    m_reflectionDepth = 0;

    int m_oBlockIndex = 0;
    int m_mBlockIndex = 0;
    int m_lBlockIndex = 0;

    int m_oAlignOffset = 0;
    int m_mAlignOffset = 0;
    int m_lAlignOffset = 0;

    int *m_oOffsets = nullptr;
    int *m_mOffsets = nullptr;
    int *m_lOffsets = nullptr;

    void uploadMaterials(Scene &sc);
    void uploadLights(Scene &sc);

};

#endif