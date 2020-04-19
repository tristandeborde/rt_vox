#ifndef RAYTRACER_CLASS_HPP
# define RAYTRACER_CLASS_HPP

# include <string>
# include <vector>
# include "ShaderCompute.Class.hpp"
# include "ShaderQuad.Class.hpp"
# include "Camera.Class.hpp"

class Raytracer
{
public:
    Raytracer(Camera *cam, OpenGL *gl);
    ~Raytracer();
    Raytracer() = delete;
    Raytracer(Raytracer &src) = delete;
    Raytracer &operator=(Raytracer &src) = delete;

    void mainLoop();
    void render_GPU();

    // Utility funcs
    static float    mix(const float &a, const float &b, const float &mix);
    static int      nextPowerOfTwo(unsigned int);
    GLuint getComputeShaderID();

private:
    // Raytracing attributes
    unsigned int        _width, _height;
    int                 _maxRayDepth;
    float               _bias;

    Camera          *_camera;
    OpenGL          *_gl;

    // Shaders
    ShaderCompute   *_cShader;
    ShaderQuad      *_qShader;
    
    // OpenGL objects
    GLuint          _vao;
    GLuint          _vbo;
    GLuint          _tex;
    GLuint          _tex2;
    GLuint          *_storageBufferIDs;

    void        quadFullScreenVao();
    void        createFramebufferTexture();
};


#endif