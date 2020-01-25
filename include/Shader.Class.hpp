#ifndef SHADER_CLASS_HPP
# define SHADER_CLASS_HPP

# include <glad/glad.h>
# include <glm/glm.hpp>
 
# include <string>
# include <fstream>
# include <sstream>
# include <iostream>

class Shader
{
public:
    Shader();
    virtual ~Shader() {};
    Shader(Shader &rhs) = delete;
    Shader &operator=(Shader &rhs) = delete;

    // activate the shader
    virtual void init() = 0;

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w);

    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    void use();
    unsigned int getID(void) const;

protected:
    unsigned int createShader(const char * filePath, int type);
    unsigned int _ID;

    void checkCompileErrors(GLuint shader, std::string type);
    void checkLinkingErrors(GLuint shader, std::string type);
    std::string _folder;

};

#endif