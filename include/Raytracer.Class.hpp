#ifndef RAYTRACER_CLASS_HPP
# define RAYTRACER_CLASS_HPP

# include <string>
# include <vector>
# include "Cube.Class.hpp"
# include "ShaderCompute.Class.hpp"
# include "ShaderQuad.Class.hpp"
# include "Camera.Class.hpp"

class Raytracer
{
public:
    Raytracer(std::vector<Cube >&c, Camera *cam);
    ~Raytracer();
    Raytracer() = delete;
    Raytracer(Raytracer &src) = delete;
    Raytracer &operator=(Raytracer &src) = delete;

    void render();
    void render_GPU();

    // Utility funcs
    static float    mix(const float &a, const float &b, const float &mix);
    static int      nextPowerOfTwo(unsigned int);

private:
    // TODO: Add a pointer to octree - for now objects are init'd in compute shader.
    std::vector<Cube>  &_cubes;
    
    // Raytracing attributes
    unsigned int        _width, _height;
    int                 _maxRayDepth;
    float               _bias;

    Camera          *_camera;

    // Shaders
    ShaderCompute   *_cShader;
    ShaderQuad      *_qShader;
    
    // OpenGL objects
    GLuint          _vao;
    GLuint          _vbo;
    GLuint          _tex;
    GLuint          _tex2;

    // glm::vec3   trace(const glm::vec3 &rayorig, const glm::vec3 &raydir, const int &depth);
    // const Cube  *searchCube(const glm::vec3 &rayorig, const glm::vec3 &raydir, float &tnear);
    // void        saveImage(const std::string &fn, const glm::vec3 *image);
    glm::vec3   computeDiffuse(const Cube *cube, const glm::vec3 &hit_point, const glm::vec3 &hit_normal);
    glm::vec3   computeReflRefr(const Cube *cube, const glm::vec3 &hit_point, const glm::vec3 &hit_normal, const glm::vec3 &raydir, const int &depth, const bool &inside);
    void        quadFullScreenVao();
    void        createFramebufferTexture();
};


#endif