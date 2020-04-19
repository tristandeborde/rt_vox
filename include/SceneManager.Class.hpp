#ifndef SCENEMANAGER_CLASS_HPP
# define SCENEMANAGER_CLASS_HPP

# pragma once
# include <glad/glad.h>
# include <glm/glm.hpp>
# include <string>
# include <cstring>
# include <vector>
# include "Scene.Class.hpp"

class SceneManager
{
public:
    SceneManager(GLuint computeShaderID);
    ~SceneManager();
    SceneManager() = delete;
    SceneManager(SceneManager &) = delete;
    SceneManager &operator=(SceneManager &) = delete;

    void readScene();
    void uploadScene();
    Scene &getScene();
    void uploadObjects();

private:
    void initialize();
    GLuint          m_computeShaderID;
    GLuint          *m_storageBufferIDs;
    unsigned int    m_numObjInShader = 0;
    unsigned int    m_numMaterialsInShader = 0;
    unsigned int    m_numLightsInShader = 0;
    unsigned int    m_reflectionDepth = 5;

    int m_oBlockIndex = 0;
    int m_mBlockIndex = 0;
    int m_lBlockIndex = 0;

    int m_oAlignOffset = 0;
    int m_mAlignOffset = 0;
    int m_lAlignOffset = 0;

    int *m_oOffsets = nullptr;
    int *m_mOffsets = nullptr;
    int *m_lOffsets = nullptr;

    Scene m_scene;

    void uploadMaterials();
    void uploadLights();

};

#endif