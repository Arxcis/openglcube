#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <LoadTexture.hpp>

#include <LoadShader.hpp>
#include <Logger.h>



namespace overkill
{

struct Vertex 
{
    float x,y,z;
    float r,g,b,a;
    float u,v;
};

struct Triangle 
{
    unsigned int a,b,c;
};

struct Camera 
{
    glm::mat4 view;
    glm::mat4 projection;
    
    float x = 0,y = 0,z = 3;
};

struct Material 
{
    GLuint shader;
    GLuint texture;
};

struct Transform 
{
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 position;
    glm::mat4 model;
};

struct Mesh 
{
    glm::vec4 color;
    std::vector<Vertex>   vertices;
    std::vector<Triangle> triangles;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint glposition;
    GLuint glcolor;
    GLuint gltexel;
};

struct Entity 
{
    Mesh     mesh;
    Material material;
    Transform transform;
};


void renderCube(Entity cube);

}

namespace overkill::Init
{

auto logtag = LOG_TAG{ "Init" };

auto GLFW_GLEW(
    const int openglMajor,
    const int openglMinor, 
    const int wwidth, 
    const int wheight, 
    const char* wname) -> GLFWwindow*;
void graphics(const glm::vec4 background); 
auto makeCube(const Material mat) -> Entity;



} // ::init

constexpr int  openglMajor = 4;
constexpr int  openglMinor = 1;
constexpr int  winWidth    = 800;
constexpr int  winHeight   = 800;
constexpr char winname[]   = "Overkill Studio - Assignment1";
const auto    background   = glm::vec4{.1f, .1f, .3f, 1.0f};

overkill::Camera cam;

int main() 
{
    using namespace overkill;
    auto logtag = LOG_TAG{ "main" };



    auto window = Init::GLFW_GLEW(openglMajor, openglMinor, winWidth, winHeight, winname);
    LOG_INFO(Init::logtag, "Window ready...")

    auto shader = Load::shaderProgram("./general.vert", "./general.frag");
    LOG_INFO(Init::logtag, "Shader ready...")

    Init::graphics(background);
    LOG_INFO(Init::logtag, "Graphics ready...")

    auto texture = Load::texture("./cube.png");

    auto cube = Init::makeCube(Material{shader, texture});    
    LOG_INFO(Init::logtag, "Graphics ready...")
    

    for(;;) 
    {
        if ((glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window) != 0))
            break;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        renderCube(cube);

        glfwSwapBuffers(window);
    }

    //LOG_ERROR(Init::logtag, (window)?"window is true": "window is false");

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

namespace overkill 
{


auto Init::makeCube(Material mat) -> Entity
{
    Mesh mesh = {
        glm::vec4 { 1.0f, 1.0f, 1.0f, 1.0f },
        std::vector<Vertex> 
        {
            Vertex { -0.5f, -0.5f,  0.5f,   0,0   },  // 0 front bottom left
            Vertex {  0.5f, -0.5f,  0.5f,   1,0   },  // 1 front bottom right
            Vertex { -0.5f,  0.5f,  0.5f,   0,1   },  // 2 front top left
            Vertex {  0.5f,  0.5f,  0.5f,   1,1   },  // 3 front top right 

            Vertex { -0.5f, -0.5f,  -0.5f,  0,1   },  // 4 back bottom left
            Vertex {  0.5f, -0.5f,  -0.5f,  1,1   },  // 5 back bottom right
            Vertex { -0.5f,  0.5f,  -0.5f,  0,0   },  // 6 back top left
            Vertex {  0.5f,  0.5f,  -0.5f,  1,0   },  // 7 back top right 


            Vertex {  0.5f, -0.5f,  0.5f,   0,0   },  // 8(1)  front bottom right
            Vertex {  0.5f, -0.5f,  -0.5f,  1,0   },  // 9(5)  back bottom right
            Vertex {  0.5f,  0.5f,  0.5f,   0,1   },  // 10(3) front top right 
            Vertex {  0.5f,  0.5f,  -0.5f,  1,1   },  // 11(7) back top right 

            Vertex { -0.5f, -0.5f,  -0.5f,  0,0   },  // 12(4) back bottom left
            Vertex { -0.5f, -0.5f,  0.5f,   1,0   },  // 13(0) front bottom left
            Vertex { -0.5f,  0.5f,  -0.5f,  0,1   },  // 14(6) back top left        
            Vertex { -0.5f,  0.5f,  0.5f,   1,1   },  // 15(2) front top left
        },
        std::vector<Triangle> 
        {
            Triangle { 0, 1, 3 }, // front
            Triangle { 3, 2, 0 },

            Triangle { 6, 7, 5 }, // back
            Triangle { 5, 4, 6 },

            Triangle { 7, 6, 3 }, // top
            Triangle { 2, 3, 6 },

            Triangle { 1, 0, 5 }, // bot
            Triangle { 4, 5, 0 },

            Triangle { 8, 9, 11 }, // right
            Triangle { 11, 10, 8 },

            Triangle { 12, 13, 15 }, // left
            Triangle { 15, 14, 12 },
        }
    };

    glUseProgram(mat.shader);
    
    // Vertex attribute array object
    glGenVertexArrays(1, &(mesh.vao));
    glBindVertexArray(mesh.vao);

    // Vertex buffer object
    glGenBuffers(1, &(mesh.vbo));
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                 mesh.vertices.size() * sizeof(Vertex), 
                 mesh.vertices.data(), 
                 GL_STATIC_DRAW);

    // Element buffer object
    glGenBuffers(1, &(mesh.ebo));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 mesh.triangles.size() * sizeof(Triangle), 
                 mesh.triangles.data(), 
                 GL_STATIC_DRAW);

    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mat.texture);
    glUniform1i(   glGetUniformLocation(mat.shader, "diffuse"),  0);

    // Vertex attributes
    mesh.glposition = glGetAttribLocation(mat.shader, "position");
    glVertexAttribPointer(mesh.glposition, 
                          3,
                          GL_FLOAT, 
                          GL_FALSE, 
                          sizeof(Vertex), 
                          0);

    mesh.gltexel = glGetAttribLocation(mat.shader, "texel");
    glVertexAttribPointer(mesh.gltexel,
                          2, 
                          GL_FLOAT, 
                          GL_FALSE, 
                          sizeof(Vertex), 
                          (void*)(3*sizeof(float)) );

    // Enable vertex attributes
    glEnableVertexAttribArray(mesh.glposition);
    glEnableVertexAttribArray(mesh.gltexel);

 //   glDrawElements(GL_TRIANGLES, mesh.triangles.size()*3, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);



    Transform trans {
        glm::vec3 { 1.0f,1.0f,1.0f },     // size
        glm::vec3 { 45.0f,45.0f,45.0f },   // rotation
        glm::vec3 { 0.5f,0.0f,0.0f },     // position
    };

    Entity cube {
        mesh,
        mat,
        trans
    };
    return cube;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    key = (action==GLFW_PRESS)?key:0;
    LOG_INFO(Init::logtag, std::string("key: "+std::to_string(key)+"\n").c_str());

    switch(key) {
        case GLFW_KEY_UP:
            cam.x += .1;
            break;

        case GLFW_KEY_DOWN:
            cam.x -= .1;        
            break;

        case GLFW_KEY_LEFT:
            cam.y += .1;
            break;

        case GLFW_KEY_RIGHT:
            cam.y -= .1;        
            break;
    
    
        case GLFW_KEY_LEFT_SHIFT:
            cam.z += .1;
            break;

        case GLFW_KEY_LEFT_ALT:
            cam.z -= .1;        
            break;
    }
}


void Init::graphics(const glm::vec4 background)
{
    // INIT OPENGL
    //
    glEnable(GL_PROGRAM_POINT_SIZE);
    glClearColor(background.x, background.y, background.z, background.w);

    glCullFace(GL_CCW); // GL_CCW | GL_CW

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void renderCube(Entity _cube) 
{
    // Bind material
    glUseProgram(_cube.material.shader);
    // shader.bind()
    // texture.bind()

        
    auto glmodel = glGetUniformLocation(_cube.material.shader, "model");
    auto glview  = glGetUniformLocation(_cube.material.shader, "view");
    auto glproj  = glGetUniformLocation(_cube.material.shader, "proj");
  
    float deltatime  = glfwGetTime() * 0.01f;
    auto  view       = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-10));
    auto  proj       = glm::perspective(glm::radians(45.0f),              // FOV
                                  float(winWidth)/float(winHeight),       // ratio
                                  0.1f,                                   // near clip
                                  100.0f);                                // far clip

    // shader.setModel(model)
    // shader.setView(view)
    // shader.setProjection(projection)
    // shader.setDiffuse()
    glUniformMatrix4fv(glview,
                       1, 
                       GL_FALSE, 
                       glm::value_ptr(view));

    glUniformMatrix4fv(glproj,
                      1, 
                      GL_FALSE, 
                      glm::value_ptr(proj));

    glUniform1i(   glGetUniformLocation(_cube.material.shader, "diffuse"),  0);

    // mesh.bind();
    glBindVertexArray(_cube.mesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _cube.material.texture);
    
    for (std::size_t i = 0; i < 4; ++i) 
    {
        auto cube = _cube;
        auto offset = (i);
        auto model = glm::translate(glm::mat4(1.0f), cube.transform.position + glm::vec3(offset, 0,0));
        model      = glm::rotate(model, (cube.transform.rotation.x*deltatime), glm::vec3(1,0,0));
        model      = glm::rotate(model, (cube.transform.rotation.y*deltatime), glm::vec3(0,1,0));
        model      = glm::rotate(model, (cube.transform.rotation.z*deltatime), glm::vec3(0,0,1));

        //auto mvp = proj * view * model;
        glUniformMatrix4fv(glmodel,
                           1, 
                           GL_FALSE, 
                           glm::value_ptr(model));


        glDrawElements(GL_TRIANGLES, cube.mesh.triangles.size() * 3, GL_UNSIGNED_INT, 0);



    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


auto Init::GLFW_GLEW(
    const int openglMajor, 
    const int openglMinor, 
    const int wwidth, 
    const int wheight, 
    const char* wname) -> GLFWwindow*
{
    // init G L F W
    if (!glfwInit()){
        glfwTerminate();
        LOG_ERROR(Init::logtag, "Failed to init GLFW");
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglMinor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // OPEN G L F W WINDOW AND HOOK UP THE OPEN GL CONTEXT
    //
    GLFWwindow* window = glfwCreateWindow(wwidth, wheight, wname, NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL) {
        glfwTerminate();
        LOG_ERROR(Init::logtag, "Failed to open GLFW window");
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSwapInterval(1);

    // init G L E W
    //
    glewExperimental = GL_TRUE;  // MACOS/intel cpu support
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        LOG_ERROR(Init::logtag, "Failed to init GLEW");
    }
    glfwSetKeyCallback(window, key_callback);
    return window;
}

} //::overkill