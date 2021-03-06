
#include <iostream>
#include <list>

#define GLEW_STATIC 1
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include "shader_compile.h"
#include "stb_image.h"
#include "Cube.h"
#include <array>
#include <vector>
#define _USE_MATH_DEFINES //so I can use math constants like PI
#include <math.h>
#include "Sphere.h"
#include "shaderloader.h"

using namespace glm;
using namespace std;


bool initContext();
float RNGpos();
GLuint loadTexture(const char* filename);

bool initContext();
float RNGpos();
vec3 crossProduct(vec3, vec3, vec3);
vector<vec3> generateSphere(float, int);
int loadSHADER(string, string);

GLFWwindow* window = NULL;

//different settings you can change 
float normalCameraSpeed = 10.0f;
float fastCameraSpeed = 50.0f;
float FOV = 70.0f;
const float mouseSensitivity = 50.0f;

// settings
const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;

vec3 defaultSize = vec3(1.0f, 6.5f, 1.0f);

// lighting
vec3 lightPos = vec3(0.0f, 30.0f, 0.0f);

int main()
{
    if (!initContext()) return -1;

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Compile and link shaders here
    Shader AffectedByLightingShader("AffectedByLighting.vert", "AffectedByLighting.frag");
    Shader NotAffectedByLightingShader("NotAffectedByLighting.vert", "NotAffectedByLighting.frag");
    Shader ShadowShader("shadowVertex.glsl", "shadowFragment.glsl");

    //Initiating camera
    vec3 cameraPosition(0.6f, 1.0f, 10.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    float cameraTiltAngle = 90.0f;
    float cameraSpeed = 0.0f;

    // Set projection matrix for shader, this won't change

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

    AffectedByLightingShader.use();
    AffectedByLightingShader.setMat4("viewMatrix", viewMatrix);

    NotAffectedByLightingShader.use();
    NotAffectedByLightingShader.setMat4("viewMatrix", viewMatrix);
 
    struct Vertex
    {
        Vertex(vec3 _position, vec3 _color, vec3 _normal, vec2 _uv)
            : position(_position), color(_color), normal(_normal), uv(_uv) {}

        vec3 position;
        vec3 color;
        vec3 normal;
        vec2 uv;
    };

    const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;

    ///////////////////////////////////////SHADOW MAPPING///////////////////////////////////////

      // Variable storing index to framebuffer used for shadow mapping
    GLuint depth_map_texture;
    // Get the texture
    glGenTextures(1, &depth_map_texture);
    // Bind the texture so the next glTex calls affect it
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    // Create the texture and specify it's attributes, including width and height, components (only depth is stored, no colour information)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // Set texture sampler parameters
    // The two calls below tell the texture sampler the shader how to upsample and downsample the texture. Here we choose the
    //    nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1]
    //    range. Here we decide to just tile the image.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    ////////////////////////////////DEPTH MAP////////////////////////////////////////////

    // Variable storing index to framebuffer used for shadow mapping
    GLuint depth_map_fbo; //fbo: framebuffer object
    // Get the flamebuffer
    glGenFramebuffers(1, &depth_map_fbo);
    // Bind the framebuffer so the next glFramebuffer calls affect it
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    //Attach the depth map texture to the depth map framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_fbo, 0);
    glDrawBuffer(GL_NONE); // Disable rendering colors, only write depth values

    const Vertex vertexArray[] = {  
        // position,                            color                        normal                                                                                                       uv
        Vertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f)),  vec2(0.0f, 0.0f)),
        Vertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f)),  vec2(0.0f, 1.0f)),
        Vertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f)),  vec2(1.0f, 1.0f)),

        Vertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(0.0f, 0.0f)),
        Vertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(1.0f, 1.0f)),
        Vertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(1.0f, 0.0f)),

        Vertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f, 0.5f,-0.5f), vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(1.0f, 1.0f)),
        Vertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f, 0.5f,-0.5f), vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(0.0f, 0.0f)),
        Vertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f, 0.5f,-0.5f), vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(0.0f, 1.0f)),

        Vertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f, 0.5f,-0.5f), vec3(0.5f,-0.5f,-0.5f), vec3(-0.5f,-0.5f,-0.5f)),  vec2(1.0f, 1.0f)),
        Vertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f, 0.5f,-0.5f), vec3(0.5f,-0.5f,-0.5f), vec3(-0.5f,-0.5f,-0.5f)),  vec2(1.0f, 0.0f)),
        Vertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f, 0.5f,-0.5f), vec3(0.5f,-0.5f,-0.5f), vec3(-0.5f,-0.5f,-0.5f)),  vec2(0.0f, 0.0f)),

        Vertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f,-0.5f), vec3(0.5f,-0.5f,-0.5f)), vec2(1.0f, 1.0f)),
        Vertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f,-0.5f), vec3(0.5f,-0.5f,-0.5f)), vec2(0.0f, 0.0f)),
        Vertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f,-0.5f), vec3(0.5f,-0.5f,-0.5f)), vec2(1.0f, 0.0f)),

        Vertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f,-0.5f)), vec2(1.0f, 1.0f)),
        Vertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f,-0.5f)), vec2(0.0f, 1.0f)),
        Vertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(-0.5f,-0.5f,-0.5f)), vec2(0.0f, 0.0f)),

        Vertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(0.0f, 1.0f)),
        Vertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(0.0f, 0.0f)),
        Vertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(-0.5f, 0.5f, 0.5f), vec3(-0.5f,-0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(1.0f, 0.0f)),

        Vertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(1.0f, 1.0f)),
        Vertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(0.0f, 1.0f)),
        Vertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(1.0f, 0.0f)),

        Vertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f,-0.5f), vec3(0.5f, 0.5f,-0.5f)),  vec2(1.0f, 1.0f)),
        Vertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f,-0.5f), vec3(0.5f, 0.5f,-0.5f)),  vec2(0.0f, 0.0f)),
        Vertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f,-0.5f), vec3(0.5f, 0.5f,-0.5f)),  vec2(1.0f, 0.0f)),

        Vertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f,-0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(0.0f, 0.0f)),
        Vertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f,-0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(1.0f, 1.0f)),
        Vertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f,-0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f,-0.5f, 0.5f)),  vec2(0.0f, 1.0f)),

        Vertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f,-0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(1.0f, 1.0f)),
        Vertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f,-0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(1.0f, 0.0f)),
        Vertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f,-0.5f), vec3(-0.5f, 0.5f,-0.5f)),  vec2(0.0f, 0.0f)),

        Vertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f,-0.5f), vec3(-0.5f, 0.5f, 0.5f)),  vec2(1.0f, 1.0f)),
        Vertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f,-0.5f), vec3(-0.5f, 0.5f, 0.5f)),  vec2(0.0f, 0.0f)),
        Vertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), crossProduct(vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f, 0.5f,-0.5f), vec3(-0.5f, 0.5f, 0.5f)),  vec2(0.0f, 1.0f)),
    };

    // Create a vertex array
    GLuint cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(2);

    // Texture attribute
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(vec3)));
    glEnableVertexAttribArray(3);    

    vector<vec3> sphereArray = generateSphere(1.0f, 48);

    //creating sphere VAO:
    GLuint sphereVAO;
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereArray.size() * sizeof(vec3), &sphereArray[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(0);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    GLuint lightSourceVAO;
    glGenVertexArrays(1, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data ( 9 * sizeof(vec3) since we only care about position here)

    //loading texture pack
     // Load Textures
#if defined(PLATFORM_OSX)
    GLuint woodTextureID = loadTexture("Textures/container.jpg");
    GLuint metalTextureIDID = loadTexture("Textures/scratched-metal.jpg");
#else
    GLuint woodTextureID = loadTexture("../Assets/Textures/container.jpg");
    GLuint metalTextureID = loadTexture("../Assets/Textures/scratched-metal.jpg");
#endif
   
    float lightAngleOuter = radians(0.0f);
    float lightAngleInner = radians(180.0f);

    // Set light cutoff angles on scene shader
    AffectedByLightingShader.use();
    AffectedByLightingShader.setFloat("light_cutoff_inner", cos(lightAngleInner));
    AffectedByLightingShader.setFloat("light_cutoff_outer", cos(lightAngleOuter));


    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    //enable OpenGL components
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    int width, height;
    float scaleFactor = 0.0f;

    const float textOffset = 8.0f;

    float currentScaleFactorJoseph = 1.0f;
    float currentScaleFactorJacob = 1.0f;
    float currentScaleFactorBad = 1.0f;
    float currentScaleFactorAdam = 1.0f;
    float currentScaleFactorAvnish = 1.0f;

    float rotateFactorX = 0.0f;
    float rotateFactorY = 0.0f;
    mat4 rotateMatrixX = mat4(1.0f);
    mat4 rotateMatrixY = mat4(1.0f);
    int modelModeJoseph = 0; //0 = triangle, 1 = line, 2 = point
    int modelModeJacob = 0;
    int modelModeBad = 0;
    int modelModeAdam = 0;
    int modelModeAvnish = 0;

    int choose = 6; //choosing the model
    float charRotateFactorx = 0.0f;
    float charRotateFactorJacobx = 0.0f;
    float charRotateFactorBadx = 0.0f;
    float charRotateFactorAdamx = 0.0f;
    float charRotateFactorAvnishx = 0.0f;

    float charRotateFactory = 0.0f;
    float charRotateFactorJacoby = 0.0f;
    float charRotateFactorBady = 0.0f;
    float charRotateFactorAdamy = 0.0f;
    float charRotateFactorAvnishy = 0.0f;

    float charRotateFactorz = 0.0f;
    float charRotateFactorJacobz = 0.0f;
    float charRotateFactorBadz = 0.0f;
    float charRotateFactorAdamz = 0.0f;
    float charRotateFactorAvnishz = 0.0f;

    mat4 charRotationz = mat4(1.0f);
    mat4 charRotationJacobz = mat4(1.0f);
    mat4 charRotationBadz = mat4(1.0f);
    mat4 charRotationAdamz = mat4(1.0f);
    mat4 charRotationAvnishz = mat4(1.0f);

    mat4 charRotationy = mat4(1.0f);
    mat4 charRotationJacoby = mat4(1.0f);
    mat4 charRotationBady = mat4(1.0f);
    mat4 charRotationAdamy = mat4(1.0f);
    mat4 charRotationAvnishy = mat4(1.0f);

    mat4 charRotationx = mat4(1.0f);
    mat4 charRotationJacobx = mat4(1.0f);
    mat4 charRotationBadx = mat4(1.0f);
    mat4 charRotationAdamx = mat4(1.0f);
    mat4 charRotationAvnishx = mat4(1.0f);


    vec3 RNGvecJacob = vec3(1.0f);
    vec3 RNGvecJoseph = vec3(1.0f);
    vec3 RNGvecBad = vec3(1.0f);
    vec3 RNGvecAdam = vec3(1.0f);
    vec3 RNGvecAvnish = vec3(1.0f);

    vec3 JacobLetterOffset = vec3(-49.5f, 6.5f, -49.5f);
    vec3 JosephLetterOffset = vec3(0.0f, 6.5f, 0.0f);
    vec3 BadLetterOffset = vec3(-49.5f, 6.5f, 49.5f);
    vec3 AdamLetterOffset = vec3(49.5f, 6.5f, -49.5f);
    vec3 AvnishLetterOffset = vec3(49.5f, 6.5f, 49.5f);

    bool enableTexture = true;

    // Entering Game Loop
    while (!glfwWindowShouldClose(window))
    {
        NotAffectedByLightingShader.use();
        NotAffectedByLightingShader.setInt("currentAxis", 0);

        //changing projection view every second to accomodate for zoom
        mat4 projectionMatrix = perspective(FOV, 1024.0f / 768.0f, 0.01f, 1000.0f);

        AffectedByLightingShader.use();
        AffectedByLightingShader.setMat4("projectionMatrix", projectionMatrix);

        NotAffectedByLightingShader.use();
        NotAffectedByLightingShader.setMat4("projectionMatrix", projectionMatrix);

        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        // Each frame, reset buffers
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        //modules for controlling model and world behaviour =================================================================================================================

        //enabling/disabling texture: 
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
            if (enableTexture == true) {
                enableTexture = false;
            }
            else {
                enableTexture = true;
            }
        }

        //scaling the model up and down with U and J

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) //scale up by 1%
        {
            scaleFactor = 0.01;

        }
        else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) //scale down by 1%
        {
            scaleFactor = -0.01;
        }
        else {
            scaleFactor = 0;

        }

        switch (choose) {
        case(1): currentScaleFactorJoseph *= (1 + scaleFactor);
            currentScaleFactorJacob *= (1 + scaleFactor);
            currentScaleFactorBad *= (1 + scaleFactor);
            currentScaleFactorAdam *= (1 + scaleFactor);
            currentScaleFactorAvnish *= (1 + scaleFactor);
            break;

        case(2): currentScaleFactorJoseph *= (1 + scaleFactor);
            break;

        case(3): currentScaleFactorJacob *= (1 + scaleFactor);
            break;

        case(4): currentScaleFactorBad *= (1 + scaleFactor);
            break;
        case(5): currentScaleFactorAdam *= (1 + scaleFactor);
            break;
        case(6): currentScaleFactorAvnish *= (1 + scaleFactor);
            break;
        default: currentScaleFactorJoseph *= (1 + scaleFactor);
            currentScaleFactorJacob *= (1 + scaleFactor);
            currentScaleFactorBad *= (1 + scaleFactor);
            currentScaleFactorAdam *= (1 + scaleFactor);
            currentScaleFactorAvnish *= (1 + scaleFactor);
            break;
        }

        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) //reset scaling 
        {
            currentScaleFactorJoseph = 1.0f;
            currentScaleFactorJacob = 1.0f;
            currentScaleFactorBad = 1.0f;
            currentScaleFactorAdam = 1.0f;
            currentScaleFactorAvnish = 1.0f;
        }

        //Translating the model to directions using R,D,F,G on the keyboard
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            switch (choose) {
            case(1): JosephLetterOffset.z += 0.1f;
                JacobLetterOffset.z += 0.1f;
                BadLetterOffset.z += 0.1f;
                AdamLetterOffset.z += 0.1f;
                AvnishLetterOffset.z += 0.1f;
                break;

            case(2): JosephLetterOffset.z += 0.1f;
                break;

            case(3): JacobLetterOffset.z += 0.1f;
                break;

            case(4): BadLetterOffset.z += 0.1f;
                break;

            case(5):  AdamLetterOffset.z += 0.1f;
                break;

            case(6):  AvnishLetterOffset.z += 0.1f;
                break;

            default: JosephLetterOffset.z += 0.1f;
                JacobLetterOffset.z += 0.1f;
                BadLetterOffset.z += 0.1f;
                AdamLetterOffset.z += 0.1f;
                AvnishLetterOffset.z += 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            switch (choose) {
            case(1): JosephLetterOffset.x -= 0.1f;
                JacobLetterOffset.x -= 0.1f;
                BadLetterOffset.x -= 0.1f;
                AdamLetterOffset.x -= 0.1f;
                AvnishLetterOffset.x -= 0.1f;
                break;

            case(2): JosephLetterOffset.x -= 0.1f;
                break;

            case(3): JacobLetterOffset.x -= 0.1f;
                break;

            case(4): BadLetterOffset.x -= 0.1f;
                break;

            case(5): AdamLetterOffset.x -= 0.1f;
                break;
            case(6): AvnishLetterOffset.x -= 0.1f;
                break;
            default: JosephLetterOffset.x -= 0.1f;
                JacobLetterOffset.x -= 0.1f;
                BadLetterOffset.x -= 0.1f;
                AdamLetterOffset.x -= 0.1f;
                AvnishLetterOffset.x -= 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            switch (choose) {
            case(1): JosephLetterOffset.x += 0.1f;
                JacobLetterOffset.x += 0.1f;
                BadLetterOffset.x += 0.1f;
                AdamLetterOffset.x += 0.1f;
                AvnishLetterOffset.x += 0.1f;
                break;

            case(2): JosephLetterOffset.x += 0.1f;
                break;

            case(3): JacobLetterOffset.x += 0.1f;
                break;

            case(4): BadLetterOffset.x += 0.1f;
                break;

            case(5): AdamLetterOffset.x += 0.1f;
                break;

            case(6): AvnishLetterOffset.x += 0.1f;
                break;

            default: JosephLetterOffset.x += 0.1f;
                JacobLetterOffset.x += 0.1f;
                BadLetterOffset.x += 0.1f;
                AdamLetterOffset.x += 0.1f;
                AvnishLetterOffset.x += 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            switch (choose) {
            case(1): JosephLetterOffset.z -= 0.1f;
                JacobLetterOffset.z -= 0.1f;
                BadLetterOffset.z -= 0.1f;
                AdamLetterOffset.z -= 0.1f;
                AvnishLetterOffset.z -= 0.1f;
                break;

            case(2): JosephLetterOffset.z -= 0.1f;
                break;

            case(3): JacobLetterOffset.z -= 0.1f;
                break;

            case(4): BadLetterOffset.z -= 0.1f;

            case(5): AdamLetterOffset.z -= 0.1f;
                break;

            case(6): AvnishLetterOffset.z -= 0.1f;
                break;

            default: JosephLetterOffset.z -= 0.1f;
                JacobLetterOffset.z -= 0.1f;
                BadLetterOffset.z -= 0.1f;
                AdamLetterOffset.z -= 0.1f;
                AvnishLetterOffset.z -= 0.1f;
                break;
            }
        }

        //changing the orientation of the letters

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            switch (choose) {
            case(1): charRotateFactorz += 0.1f;
                charRotateFactorJacobz += 0.1f;
                charRotateFactorBadz += 0.1f;
                charRotateFactorAdamz += 0.1f;
                charRotateFactorAvnishz += 0.1f;
                break;
            case(2): charRotateFactorz += 0.1f;
                break;
            case(3): charRotateFactorJacobz += 0.1f;
                break;
            case(4): charRotateFactorBadz += 0.1f;
                break;
            case(5): charRotateFactorAdamz += 0.1f;
                break;
            case (6): charRotateFactorAvnishz += 0.1f;
                break;
            default: charRotateFactorz += 0.1f;
                charRotateFactorJacobz += 0.1f;
                charRotateFactorBadz += 0.1f;
                charRotateFactorAdamz += 0.1f;
                charRotateFactorAvnishz += 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            switch (choose) {
            case(1): charRotateFactorz -= 0.1f;
                charRotateFactorJacobz -= 0.1f;
                charRotateFactorBadz -= 0.1f;
                charRotateFactorAdamz -= 0.1f;
                charRotateFactorAvnishz -= 0.1f;
                break;
            case(2): charRotateFactorz -= 0.1f;
                break;
            case(3): charRotateFactorJacobz -= 0.1f;
                break;
            case(4): charRotateFactorBadz -= 0.1f;
                break;
            case(5): charRotateFactorAdamz -= 0.1f;
                break;
            case(6): charRotateFactorAvnishz -= 0.1f;
                break;
            default: charRotateFactorz -= 0.1f;
                charRotateFactorJacobz -= 0.1f;
                charRotateFactorBadz -= 0.1f;
                charRotateFactorAdamz -= 0.1f;
                charRotateFactorAvnishz -= 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            switch (choose) {
            case(1): charRotateFactory -= 0.1f;
                charRotateFactorJacoby -= 0.1f;
                charRotateFactorBady -= 0.1f;
                charRotateFactorAdamy -= 0.1f;
                charRotateFactorAvnishy -= 0.1f;
                break;
            case(2): charRotateFactory -= 0.1f;
                break;
            case(3): charRotateFactorJacoby -= 0.1f;
                break;
            case(4): charRotateFactorBady -= 0.1f;
                break;
            case(5): charRotateFactorAdamy -= 0.1f;
                break;
            case(6): charRotateFactorAvnishy -= 0.1f;
                break;
            default: charRotateFactory -= 0.1f;
                charRotateFactorJacoby -= 0.1f;
                charRotateFactorBady -= 0.1f;
                charRotateFactorAdamy -= 0.1f;
                charRotateFactorAvnishy -= 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            switch (choose) {
            case(1): charRotateFactory += 0.1f;
                charRotateFactorJacoby += 0.1f;
                charRotateFactorBady += 0.1f;
                charRotateFactorAdamy += 0.1f;
                charRotateFactorAvnishy += 0.1f;
                break;
            case(2): charRotateFactory += 0.1f;
                break;
            case(3): charRotateFactorJacoby += 0.1f;
                break;
            case(4): charRotateFactorBady += 0.1f;
                break;
            case(5): charRotateFactorAdamy += 0.1f;
                break;
            case (6): charRotateFactorAvnishy += 0.1f;
                break;
            default: charRotateFactory += 0.1f;
                charRotateFactorJacoby += 0.1f;
                charRotateFactorBady += 0.1f;
                charRotateFactorAdamy += 0.1f;
                charRotateFactorAvnishy += 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            switch (choose) {
            case(1): charRotateFactorx += 0.1f;
                charRotateFactorJacobx += 0.1f;
                charRotateFactorBadx += 0.1f;
                charRotateFactorAdamx += 0.1f;
                charRotateFactorAvnishx += 0.1f;
                break;
            case(2): charRotateFactorx += 0.1f;
                break;
            case(3): charRotateFactorJacobx += 0.1f;
                break;
            case(4): charRotateFactorBadx += 0.1f;
                break;
            case(5): charRotateFactorAdamx += 0.1f;
                break;
            case (6): charRotateFactorAvnishx += 0.1f;
                break;
            default: charRotateFactorx += 0.1f;
                charRotateFactorJacobx += 0.1f;
                charRotateFactorBadx += 0.1f;
                charRotateFactorAdamx += 0.1f;
                charRotateFactorAvnishx += 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
            switch (choose) {
            case(1): charRotateFactorx -= 0.1f;
                charRotateFactorJacobx -= 0.1f;
                charRotateFactorBadx -= 0.1f;
                charRotateFactorAdamx -= 0.1f;
                charRotateFactorAvnishx -= 0.1f;
                break;
            case(2): charRotateFactorx -= 0.1f;
                break;
            case(3): charRotateFactorJacobx -= 0.1f;
                break;
            case(4): charRotateFactorBadx -= 0.1f;
                break;
            case(5): charRotateFactorAdamx -= 0.1f;
                break;
            case (6): charRotateFactorAvnishx -= 0.1f;
                break;
            default: charRotateFactorx -= 0.1f;
                charRotateFactorJacobx -= 0.1f;
                charRotateFactorBadx -= 0.1f;
                charRotateFactorAdamx -= 0.1f;
                charRotateFactorAvnishx -= 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            charRotateFactorx = 0.0f;
            charRotateFactorJacobx = 0.0f;
            charRotateFactorBadx = 0.0f;
            charRotateFactorAdamx = 0.0f;
            charRotateFactorAvnishx = 0.0f;

            charRotateFactory = 0.0f;
            charRotateFactorJacoby = 0.0f;
            charRotateFactorBady = 0.0f;
            charRotateFactorAdamy = 0.0f;
            charRotateFactorAvnishy = 0.0f;

            charRotateFactorz = 0.0f;
            charRotateFactorJacobz = 0.0f;
            charRotateFactorBadz = 0.0f;
            charRotateFactorAdamz = 0.0f;
            charRotateFactorAvnishz = 0.0f;
        }

        charRotationz = rotate(mat4(1.0f), radians(charRotateFactorz), vec3(0.0f, 0.0f, 1.0f));
        charRotationJacobz = rotate(mat4(1.0f), radians(charRotateFactorJacobz), vec3(0.0f, 0.0f, 1.0f));
        charRotationBadz = rotate(mat4(1.0f), radians(charRotateFactorBadz), vec3(0.0f, 0.0f, 1.0f));
        charRotationAdamz = rotate(mat4(1.0f), radians(charRotateFactorAdamz), vec3(0.0f, 0.0f, 1.0f));
        charRotationAvnishz = rotate(mat4(1.0f), radians(charRotateFactorAvnishz), vec3(0.0f, 0.0f, 1.0f));

        charRotationy = rotate(mat4(1.0f), radians(charRotateFactory), vec3(0.0f, 1.0f, 0.0f));
        charRotationJacoby = rotate(mat4(1.0f), radians(charRotateFactorJacoby), vec3(0.0f, 1.0f, 0.0f));
        charRotationBady = rotate(mat4(1.0f), radians(charRotateFactorBady), vec3(0.0f, 1.0f, 0.0f));
        charRotationAdamy = rotate(mat4(1.0f), radians(charRotateFactorAdamy), vec3(0.0f, 1.0f, 0.0f));
        charRotationAvnishy = rotate(mat4(1.0f), radians(charRotateFactorAvnishy), vec3(0.0f, 1.0f, 0.0f));

        charRotationx = rotate(mat4(1.0f), radians(charRotateFactorx), vec3(1.0f, 0.0f, 0.0f));
        charRotationJacobx = rotate(mat4(1.0f), radians(charRotateFactorJacobx), vec3(1.0f, 0.0f, 0.0f));
        charRotationBadx = rotate(mat4(1.0f), radians(charRotateFactorBadx), vec3(1.0f, 0.0f, 0.0f));
        charRotationAdamx = rotate(mat4(1.0f), radians(charRotateFactorAdamx), vec3(1.0f, 0.0f, 0.0f));
        charRotationAvnishx = rotate(mat4(1.0f), radians(charRotateFactorAvnishx), vec3(1.0f, 0.0f, 0.0f));


        //changing the orientation of the world.

        bool worldRotate = false;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            rotateFactorX += 1.0;
            rotateMatrixX = rotate(mat4(1.0f), radians(rotateFactorX), vec3(1.0f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            rotateFactorX -= 1.0;
            rotateMatrixX = rotate(mat4(1.0f), radians(rotateFactorX), vec3(1.0f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            rotateFactorY -= 1.0;
            rotateMatrixY = rotate(mat4(1.0f), radians(rotateFactorY), vec3(0.0f, 1.0f, 0.0f));

        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            rotateFactorY += 1.0;
            rotateMatrixY = rotate(mat4(1.0f), radians(rotateFactorY), vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            rotateFactorX = 0.0f;
            rotateFactorY = 0.0f;
            rotateMatrixX = mat4(1.0f);
            rotateMatrixY = mat4(1.0f);
        }


        //returning to where we started
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
        {
            cameraPosition.x = 0.6f;
            cameraPosition.y = 1.0f;
            cameraPosition.z = 10.0f;
            rotateFactorX = 0.0f;
            rotateFactorY = 0.0f;
            rotateMatrixX = mat4(1.0f);
            rotateMatrixY = mat4(1.0f);
        }


        //switching view mode between Triangle, Line, Point (using T, L, P)
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            switch (choose) {
            case 1:
                modelModeJoseph = 0;
                modelModeJacob = 0;
                modelModeBad = 0;
                modelModeAdam = 0;
                modelModeAvnish = 0;
                break;
            case 2:
                modelModeJoseph = 0;
                break;
            case 3:
                modelModeJacob = 0;
                break;
            case 4:
                modelModeBad = 0;
                break;
            case 5:
                modelModeAdam = 0;
                break;
            case 6:
                modelModeAvnish = 0;
                break;
            default:
                modelModeJoseph = 0;
                modelModeJacob = 0;
                modelModeBad = 0;
                modelModeAdam = 0;
                modelModeAvnish = 0;
                break;
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            switch (choose)
            {
            case 1:
                modelModeJoseph = 1;
                modelModeJacob = 1;
                modelModeBad = 1;
                modelModeAdam = 1;
                modelModeAvnish = 1;
                break;
            case 2:
                modelModeJoseph = 1;
                break;
            case 3:
                modelModeJacob = 1;
                break;
            case 4:
                modelModeBad = 1;
                break;
            case 5:
                modelModeAdam = 1;
                break;
            case 6:
                modelModeAvnish = 1;
                break;
            default:
                modelModeJoseph = 1;
                modelModeJacob = 1;
                modelModeBad = 1;
                modelModeAdam = 1;
                modelModeAvnish = 1;
                break;
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            switch (choose)
            {
            case 1:
                modelModeJoseph = 2;
                modelModeJacob = 2;
                modelModeBad = 2;
                modelModeAdam = 2;
                modelModeAvnish = 2;
                break;
            case 2:
                modelModeJoseph = 2;
                break;
            case 3:
                modelModeJacob = 2;
                break;
            case 4:
                modelModeBad = 2;
                break;
            case 5:
                modelModeAdam = 2;
                break;
            case 6:
                modelModeAvnish = 2;
                break;
            default:
                modelModeJoseph = 2;
                modelModeJacob = 2;
                modelModeBad = 2;
                modelModeAdam = 2;
                modelModeAvnish = 2;
                break;
            }
        }

        //reset the models to original position
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            JacobLetterOffset = vec3(-49.5f, 6.5f, -49.5f);
            JosephLetterOffset = vec3(0.0f, 6.5f, 0.0f);
            BadLetterOffset = vec3(-49.5f, 6.5f, 49.5f);
            AdamLetterOffset = vec3(49.5f, 6.5f, -49.5f);
            AvnishLetterOffset = vec3(49.5f, 6.5f, 49.5f);
        }

        //choosing between the moodels
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            choose = 1;
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            choose = 2;
        else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            choose = 3;
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            choose = 4;
        else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            choose = 5;
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            choose = 6;


        //drawing everything ==================================================================================================
        
        // light parameters
        //float phi = glfwGetTime() * 0.5f * M_PI;
        //vec3(cosf(phi)* cosf(phi), sinf(phi), -cosf(phi) * sinf(phi)) * 5.0f;  // variable position

        vec3 lightFocus(0, -1, 0); // the point in 3D space the light "looks" at
        vec3 lightDirection = normalize(lightFocus - lightPos);

        float lightNearPlane = 0.01f;
        float lightFarPlane = 400.0f;
        //frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
        //mat4 lightProjMatrix = perspective(50.0f, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
        mat4 lightProjMatrix = ortho(-50.0f, 50.0f, -50.0f, 50.0f, lightNearPlane, lightFarPlane);
        mat4 lightViewMatrix = lookAt(lightPos, lightFocus, vec3(0, 1, 0));

        // //be sure to activate shader when setting uniforms/drawing objects
        AffectedByLightingShader.use();
        AffectedByLightingShader.setMat4("light_proj_view_matrix", lightProjMatrix * lightFarPlane);
        AffectedByLightingShader.setFloat("light_near_plane", lightNearPlane);
        AffectedByLightingShader.setFloat("light_far_plane", lightFarPlane);
        AffectedByLightingShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        AffectedByLightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        AffectedByLightingShader.setVec3("lightPos", lightPos);
        AffectedByLightingShader.setVec3("light_direction", lightDirection);
        AffectedByLightingShader.setVec3("viewPos", cameraPosition);

        //////////////////////////////////////////////
        //FIRST PASS/////////////////////////////////
        //////////////////////////////////////////////
        ShadowShader.use();
        // Use proper image output size
        glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
        // Bind depth map texture as output framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
        // Clear depth data on the framebuffer
        glClear(GL_DEPTH_BUFFER_BIT);
        // Bind geometry
        AffectedByLightingShader.use();
        glBindVertexArray(cubeVAO);

        /////////////////////Ground: MESH STYLE///////////////////////////////////

        ////drawing ground mesh 
        //for (int i = 0; i <= 100; i++) {
        //    //horizontal
        //    mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.01f, -50.0f + i)) * scale(mat4(1.0f), vec3(100.0f, 0.02f, 0.02f));
        //    groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
        //    NotAffectedByLightingShader.setMat4("worldMatrix", groundWorldMatrix);

        //    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0


        //    //vertical
        //    groundWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + i, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 100.0f));
        //    groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
        //    NotAffectedByLightingShader.setMat4("worldMatrix", groundWorldMatrix);


        //    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        //}

        ///////////////////Ground: TILE STYLE/////////////////////////////////////
        if(enableTexture == true)
        glBindTexture(GL_TEXTURE_2D, metalTextureID);
        for (int i = 0; i <= 10; i++) {
            for (int j = 0; j <= 10; j++) {
                mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + i * 10, -0.01f, -50.0f + j * 10)) * scale(mat4(1.0f), vec3(10.0f, 0.02f, 10.0f));
                groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
                AffectedByLightingShader.setMat4("worldMatrix", groundWorldMatrix);

                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            }
            
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        //drawing the XYZ line:
        NotAffectedByLightingShader.use();

        mat4 cordMarker = translate(mat4(1.0f), vec3(2.5f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.02f, 0.02f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        NotAffectedByLightingShader.setMat4("worldMatrix", cordMarker);
        NotAffectedByLightingShader.setInt("currentAxis", 1);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        cordMarker = translate(mat4(1.0f), vec3(0.0f, 3.5f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 5.0f, 0.02f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        NotAffectedByLightingShader.setMat4("worldMatrix", cordMarker);
        NotAffectedByLightingShader.setInt("currentAxis", 2);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        cordMarker = translate(mat4(1.0f), vec3(0.0f, 1.0f, 2.5f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 5.0f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        NotAffectedByLightingShader.setMat4("worldMatrix", cordMarker);
        NotAffectedByLightingShader.setInt("currentAxis", 3);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        NotAffectedByLightingShader.setInt("currentAxis", 0);
        glBindVertexArray(0);


        // //be sure to activate shader when setting uniforms/drawing objects
        AffectedByLightingShader.use();
        AffectedByLightingShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        AffectedByLightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        AffectedByLightingShader.setVec3("lightPos", lightPos);
        AffectedByLightingShader.setVec3("viewPos", cameraPosition);

        //Draw the lamp object
        mat4 world = translate(mat4(1.0f), vec3(lightPos)) * scale(mat4(1.0f), vec3(0.2f));
        NotAffectedByLightingShader.use();
        NotAffectedByLightingShader.setMat4("worldMatrix", world);
        NotAffectedByLightingShader.setInt("lightSource", 1);

        //initializing shadow 
        glBindVertexArray(lightSourceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        NotAffectedByLightingShader.setInt("lightSource", 0);
        glBindVertexArray(0);
        
        //initializing texture
        AffectedByLightingShader.use();
        glBindVertexArray(cubeVAO);
       
        //glBindVertexArray(lightSourceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        NotAffectedByLightingShader.setInt("lightSource", 0);
        glBindVertexArray(0);
       
        //drawing sphere
        AffectedByLightingShader.use();
        glBindVertexArray(cubeVAO);

        //drawing the letters
        //jacob's letter and digit
        array<Cube, 7> Jacob = {
            //C vertical
            Cube(vec3(0.0f, 3.25f, 0.0f), false, ShadowShader),

            //C horizontal
            Cube(vec3(2.5f, 0.0f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f, 6.5f, 0.0f), true, ShadowShader),

            //9 vertical
            Cube(vec3(5.0f + textOffset, 3.25f, 0.0f), false, ShadowShader),
            Cube(vec3(0.0f + textOffset, 3.25 * 1.5, 0.0f), false, ShadowShader),

            //9 horizontal
            Cube(vec3(2.5f + textOffset, 6.5f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 3.25, 0.0f), true, ShadowShader)
        };

        //perform any changes necesary 
        Jacob[4].setDefaultSize(vec3(1.0f, 6.5 / 2, 1.0f));

        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 3 || choose == 1)) {
            RNGvecJacob = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            JacobLetterOffset = RNGvecJacob;
        }

        for (int i = 0; i < Jacob.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 3) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            Jacob[i].setCustomScaling(currentScaleFactorJacob);
            Jacob[i].setCustomRotation(charRotationJacobz * charRotationJacoby * charRotationJacobx);
            Jacob[i].setCustomTranslation(JacobLetterOffset);
            Jacob[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            Jacob[i].setMode(modelModeJacob);
            Jacob[i].drawModel();
        }

        glBindVertexArray(0);
        glBindVertexArray(sphereVAO);

        Sphere sphereJacob = Sphere(vec3(7.5, 10.0, 0.0), 7.5, sphereArray.size() / 3, ShadowShader);
        sphereJacob.setCurrentShader(ShadowShader);
        sphereJacob.setCustomScaling(currentScaleFactorJacob);
        sphereJacob.setCustomRotation(charRotationJacobz * charRotationJacoby * charRotationJacobx);
        sphereJacob.setCustomTranslation(JacobLetterOffset);
        sphereJacob.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereJacob.drawModel();

        glBindVertexArray(0);
        glBindVertexArray(cubeVAO);

        //Joseph's letter and digit
        array<Cube, 8> Joseph = {
            //U vertical
            Cube(vec3(0.0f, 3.25f, 0.0f), false, ShadowShader),
            Cube(vec3(5.0f, 3.25f, 0.0f), false, ShadowShader),

            //U horizontal
            Cube(vec3(2.5f, 0.0f, 0.0f), true, ShadowShader),

            //6 vertical
            Cube(vec3(0.0f + textOffset, 3.25f, 0.0f), false, ShadowShader),
            Cube(vec3(5.0f + textOffset, 3.25 / 2, 0.0f), false, ShadowShader),

            //6 horizontal
            Cube(vec3(2.5f + textOffset, 0.0f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 3.25f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 6.5f, 0.0f), true, ShadowShader),
        };

        //perform any size change necesary 
        Joseph[4].setDefaultSize(vec3(1.0f, 3.25f, 1.0f));

        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 2 || choose == 1)) {
            RNGvecJoseph = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            JosephLetterOffset = RNGvecJoseph;
        }

        for (int i = 0; i < Joseph.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 3) {
                glBindTexture(GL_TEXTURE_2D, 0);
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            Joseph[i].setCustomScaling(currentScaleFactorJoseph);
            Joseph[i].setCustomRotation(charRotationz * charRotationy * charRotationx);
            Joseph[i].setCustomTranslation(JosephLetterOffset);
            Joseph[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            Joseph[i].setMode(modelModeJoseph);
            Joseph[i].drawModel();
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(sphereVAO);

        Sphere sphereJoseph = Sphere(vec3(7.5, 10.0, 0.0), 7.5, sphereArray.size() / 3, ShadowShader);
        sphereJoseph.setCurrentShader(ShadowShader);
        sphereJoseph.setCustomScaling(currentScaleFactorJoseph);
        sphereJoseph.setCustomRotation(charRotationz * charRotationy * charRotationx);
        sphereJoseph.setCustomTranslation(JosephLetterOffset);
        sphereJoseph.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereJoseph.drawModel();

        glBindVertexArray(0);
        glBindVertexArray(cubeVAO);

        //Sphere sphereJoseph = Sphere(vec3()

        // Badreddine letter and digit
        array<Cube, 9> Bad = {
            //D vertical
            Cube(vec3(0.0f, 3.25f, 0.0f), false, ShadowShader),
            Cube(vec3(5.5f, 3.25f, 0.0f), false, ShadowShader),

            //D horizontal
            Cube(vec3(2.5f, 0.0f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f, 6.5f, 0.0f), true, ShadowShader),

            //2 vertical
            Cube(vec3(0.0f + textOffset, 3.25 * 0.5, 0.0f), false, ShadowShader),
            Cube(vec3(5.0f + textOffset, 3.25 * 1.5, 0.0f), false, ShadowShader),

            //2 horizontal
            Cube(vec3(2.5f + textOffset, 0.0f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 3.25f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 6.5f, 0.0f), true, ShadowShader),
        };

        Bad[0].setDefaultSize(vec3(1.0f, 7, 1.0f));
        Bad[1].setDefaultSize(vec3(1.0f, 6, 1.0f));
        Bad[3].setDefaultSize(vec3(1.0f, 6.0, 1.0f));
        Bad[4].setDefaultSize(vec3(1.0f, 6.5 / 2, 1.0f));
        Bad[5].setDefaultSize(vec3(1.0f, 6.5 / 2, 1.0f));


        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 4 || choose == 1)) {
            RNGvecBad = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            BadLetterOffset = RNGvecBad;
        }

        for (int i = 0; i < Bad.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 4) {
                glBindTexture(GL_TEXTURE_2D, 0);
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            Bad[i].setCustomScaling(currentScaleFactorBad);
            Bad[i].setCustomRotation(charRotationBadz * charRotationBady * charRotationBadx);
            Bad[i].setCustomTranslation(BadLetterOffset);
            Bad[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            Bad[i].setMode(modelModeBad);
            Bad[i].drawModel();
        };

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBindVertexArray(sphereVAO);

        Sphere sphereBad = Sphere(vec3(7.5, 10.0, 0.0), 7.5, sphereArray.size() / 3, ShadowShader);
        sphereBad.setCurrentShader(ShadowShader);
        sphereBad.setCustomScaling(currentScaleFactorBad);
        sphereBad.setCustomRotation(charRotationBadz * charRotationBady * charRotationBadx);
        sphereBad.setCustomTranslation(BadLetterOffset);
        sphereBad.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereBad.drawModel();

        glBindVertexArray(0);
        glBindVertexArray(cubeVAO);

        //Adam's Letter and digit
        array<Cube, 9> adam = {
            //A vertical
            Cube(vec3(0.0f, 3.25f, 0.0f), false, ShadowShader),
            Cube(vec3(5.0f, 3.25f, 0.0f), false, ShadowShader),

            //A horizontal
            Cube(vec3(2.5f, 3.25f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f, 6.5f, 0.0f), true, ShadowShader),

            //2 vertical
            Cube(vec3(0.0f + textOffset, 3.25 * 0.5, 0.0f), false, ShadowShader),
            Cube(vec3(5.0f + textOffset, 3.25 * 1.5, 0.0f), false, ShadowShader),

            //2 horizontal
            Cube(vec3(2.5f + textOffset, 0.0f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 3.25f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 6.5f, 0.0f), true, ShadowShader),
        };

        adam[4].setDefaultSize(vec3(1.0f, 6.5 / 2, 1.0f));
        adam[5].setDefaultSize(vec3(1.0f, 6.5 / 2, 1.0f));

        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 5 || choose == 1)) {
            RNGvecAdam = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            AdamLetterOffset = RNGvecAdam;
        }

        for (int i = 0; i < adam.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 4) {
                glBindTexture(GL_TEXTURE_2D, 0);
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            adam[i].setCustomScaling(currentScaleFactorAdam);
            adam[i].setCustomRotation(charRotationAdamz * charRotationAdamy * charRotationAdamx);
            adam[i].setCustomTranslation(AdamLetterOffset);
            adam[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            adam[i].setMode(modelModeAdam);
            adam[i].drawModel();
        };

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBindVertexArray(sphereVAO);

        Sphere sphereAdam = Sphere(vec3(7.5, 10.0, 0.0), 7.5, sphereArray.size() / 3, ShadowShader);
        sphereAdam.setCurrentShader(ShadowShader);
        sphereAdam.setCustomScaling(currentScaleFactorAdam);
        sphereAdam.setCustomRotation(charRotationAdamz * charRotationAdamy * charRotationAdamx);
        sphereAdam.setCustomTranslation(AdamLetterOffset);
        sphereAdam.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereAdam.drawModel();

        glBindVertexArray(0);
        glBindVertexArray(cubeVAO);

        //Avnish's letter and digit
        array<Cube, 8> avnish = {
            //N vertical
            Cube(vec3(0.0f, 3.25f, 0.0f), false, ShadowShader),
            Cube(vec3(5.0f, 3.25f, 0.0f), false, ShadowShader),

            //N diagonal
            Cube(vec3(2.5f, 3.25f, 0.0f), false, ShadowShader),

            //2 vertical
            Cube(vec3(0.0f + textOffset, 3.25 * 0.5, 0.0f), false, ShadowShader),
            Cube(vec3(5.0f + textOffset, 3.25 * 1.5, 0.0f), false, ShadowShader),

            //2 horizontal
            Cube(vec3(2.5f + textOffset, 0.0f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 3.25f, 0.0f), true, ShadowShader),
            Cube(vec3(2.5f + textOffset, 6.5f, 0.0f), true, ShadowShader),

        };

        avnish[2].setDefaultRotation(45);
        avnish[3].setDefaultSize(vec3(1.0f, 6.5 / 2, 1.0f));
        avnish[4].setDefaultSize(vec3(1.0f, 6.5 / 2, 1.0f));

        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 6 || choose == 1)) {
            RNGvecAvnish = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            AvnishLetterOffset = RNGvecAvnish;
        }

        for (int i = 0; i < avnish.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 3) {
                glBindTexture(GL_TEXTURE_2D, 0);
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            avnish[i].setCustomScaling(currentScaleFactorAvnish);
            avnish[i].setCustomRotation(charRotationAvnishz * charRotationAvnishy * charRotationAvnishx);
            avnish[i].setCustomTranslation(AvnishLetterOffset);
            avnish[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            avnish[i].setMode(modelModeAvnish);
            avnish[i].drawModel();

        };
        
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(sphereVAO);

        Sphere sphereAvnish = Sphere(vec3(7.5, 10.0, 0.0), 7.5, sphereArray.size() / 3, ShadowShader);
        sphereAvnish.setCurrentShader(ShadowShader);
        sphereAvnish.setCustomScaling(currentScaleFactorAvnish);
        sphereAvnish.setCustomRotation(charRotationAvnishz * charRotationAvnishy * charRotationAvnishx);
        sphereAvnish.setCustomTranslation(AvnishLetterOffset);
        sphereAvnish.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereAvnish.drawModel();

        glBindVertexArray(0);
        //glBindTexture(GL_TEXTURE_2D, 0);

        //////////////////////////////////
        ///////////SECOND PASS////////////
        //////////////////////////////////

         AffectedByLightingShader.use();
        // Use proper image output size
        // Side note: we get the size from the framebuffer instead of using WIDTH
        // and HEIGHT because of a bug with highDPI displays
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
         // Bind screen as output framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Clear color and depth data on framebuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         // Bind depth map texture: not needed, by default it is active
         // glActiveTexture(GL_TEXTURE0);
         // Activate any texture unit you use for your models
         // Bind geometry
        NotAffectedByLightingShader.use();
        glBindVertexArray(cubeVAO);
        ////drawing ground mesh 
        //for (int i = 0; i <= 100; i++) {
        //    //horizontal
        //    mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.01f, -50.0f + i)) * scale(mat4(1.0f), vec3(100.0f, 0.02f, 0.02f));
        //    groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
        //    NotAffectedByLightingShader.setMat4("worldMatrix", groundWorldMatrix);

        //    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0


        //    //vertical
        //    groundWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + i, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 100.0f));
        //    groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
        //    NotAffectedByLightingShader.setMat4("worldMatrix", groundWorldMatrix);


        //    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        //}
        if (enableTexture == true)
        glBindTexture(GL_TEXTURE_2D, metalTextureID);

        for (int i = 0; i <= 10; i++) {
            for (int j = 0; j <= 10; j++) {
                mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + i * 10, -0.01f, -50.0f + j * 10)) * scale(mat4(1.0f), vec3(10.0f, 0.02f, 10.0f));
                groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
                AffectedByLightingShader.use();
                AffectedByLightingShader.setMat4("worldMatrix", groundWorldMatrix);

                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            }

        }
        glBindTexture(GL_TEXTURE_2D, 0);

        //drawing the XYZ line: 
        cordMarker = translate(mat4(1.0f), vec3(2.5f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.02f, 0.02f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        NotAffectedByLightingShader.setMat4("worldMatrix", cordMarker);
        NotAffectedByLightingShader.setInt("currentAxis", 1);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        cordMarker = translate(mat4(1.0f), vec3(0.0f, 3.5f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 5.0f, 0.02f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        NotAffectedByLightingShader.setMat4("worldMatrix", cordMarker);
        NotAffectedByLightingShader.setInt("currentAxis", 2);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        cordMarker = translate(mat4(1.0f), vec3(0.0f, 1.0f, 2.5f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 5.0f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        NotAffectedByLightingShader.setMat4("worldMatrix", cordMarker);
        NotAffectedByLightingShader.setInt("currentAxis", 3);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        NotAffectedByLightingShader.setInt("currentAxis", 0);
        glBindVertexArray(0);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        mat4 cube = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(1.0f));
        // view/projection transformations
        //AffectedByLightingShader.setMat4("projectionMatrix", projectionMatrix);
        //AffectedByLightingShader.setMat4("viewMatrix", viewMatrix);

        //world transformation
        AffectedByLightingShader.setMat4("worldMatrix", cube);
        ShadowShader.setMat4("transform_in_light_space", lightProjMatrix * lightViewMatrix * cube);
        
        
        //Draw geometry
        glBindVertexArray(cubeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        NotAffectedByLightingShader.use();
        glBindVertexArray(cubeVAO);
        //Draw the lamp object
        world = translate(mat4(1.0f), vec3(lightPos)) * scale(mat4(1.0f), vec3(0.2f));
        NotAffectedByLightingShader.use();
        NotAffectedByLightingShader.setMat4("worldMatrix", world);
        NotAffectedByLightingShader.setInt("lightSource", 1);

        //glBindVertexArray(lightSourceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        NotAffectedByLightingShader.setInt("lightSource", 0);
        glBindVertexArray(0);
       
        //drawing sphere
        AffectedByLightingShader.use();

        glBindVertexArray(cubeVAO);

        //drawing the letters
        //jacob's letter and digit
       
        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 3 || choose == 1)) {
            RNGvecJacob = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            JacobLetterOffset = RNGvecJacob;
        }

        for (int i = 0; i < Jacob.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 3) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            Jacob[i].setCurrentShader(AffectedByLightingShader);
            Jacob[i].setCustomScaling(currentScaleFactorJacob);
            Jacob[i].setCustomRotation(charRotationJacobz * charRotationJacoby * charRotationJacobx);
            Jacob[i].setCustomTranslation(JacobLetterOffset);
            Jacob[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            Jacob[i].setMode(modelModeJacob);
            Jacob[i].drawModel();
        }

        glBindVertexArray(0);
        glBindVertexArray(sphereVAO);

        sphereJacob.setCurrentShader(AffectedByLightingShader);
        sphereJacob.setCustomScaling(currentScaleFactorJacob);
        sphereJacob.setCustomRotation(charRotationJacobz* charRotationJacoby* charRotationJacobx);
        sphereJacob.setCustomTranslation(JacobLetterOffset);
        sphereJacob.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereJacob.drawModel();

        glBindVertexArray(0);
        glBindVertexArray(cubeVAO);

        //Joseph's letter and digit
       
        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 2 || choose == 1)) {
            RNGvecJoseph = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            JosephLetterOffset = RNGvecJoseph;
        }

        for (int i = 0; i < Joseph.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 3) {
                glBindTexture(GL_TEXTURE_2D, 0);
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            Joseph[i].setCurrentShader(AffectedByLightingShader);
            Joseph[i].setCustomScaling(currentScaleFactorJoseph);
            Joseph[i].setCustomRotation(charRotationz * charRotationy * charRotationx);
            Joseph[i].setCustomTranslation(JosephLetterOffset);
            Joseph[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            Joseph[i].setMode(modelModeJoseph);
            Joseph[i].drawModel();
        }

        glBindVertexArray(0);
        glBindVertexArray(sphereVAO);

        sphereJoseph.setCurrentShader(AffectedByLightingShader);
        sphereJoseph.setCustomScaling(currentScaleFactorJoseph);
        sphereJoseph.setCustomRotation(charRotationz * charRotationy * charRotationx);
        sphereJoseph.setCustomTranslation(JosephLetterOffset);
        sphereJoseph.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereJoseph.drawModel();

        glBindVertexArray(0);
        glBindVertexArray(cubeVAO);

        // Badreddine letter and digit
      
        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 4 || choose == 1)) {
            RNGvecBad = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            BadLetterOffset = RNGvecBad;
        }

        for (int i = 0; i < Bad.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 4) {
                glBindTexture(GL_TEXTURE_2D, 0);
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            Bad[i].setCurrentShader(AffectedByLightingShader);
            Bad[i].setCustomScaling(currentScaleFactorBad);
            Bad[i].setCustomRotation(charRotationBadz * charRotationBady * charRotationBadx);
            Bad[i].setCustomTranslation(BadLetterOffset);
            Bad[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            Bad[i].setMode(modelModeBad);
            Bad[i].drawModel();
        };

        glBindVertexArray(0);
        glBindVertexArray(sphereVAO);

        sphereBad.setCurrentShader(AffectedByLightingShader);
        sphereBad.setCustomScaling(currentScaleFactorBad);
        sphereBad.setCustomRotation(charRotationBadz* charRotationBady* charRotationBadx);
        sphereBad.setCustomTranslation(BadLetterOffset);
        sphereBad.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereBad.drawModel();

        glBindVertexArray(0);
        glBindVertexArray(cubeVAO);

        //Adam's Letter and digit
        
        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 5 || choose == 1)) {
            RNGvecAdam = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            AdamLetterOffset = RNGvecAdam;
        }

        for (int i = 0; i < adam.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 4) {
                glBindTexture(GL_TEXTURE_2D, 0);
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            adam[i].setCurrentShader(AffectedByLightingShader);
            adam[i].setCustomScaling(currentScaleFactorAdam);
            adam[i].setCustomRotation(charRotationAdamz * charRotationAdamy * charRotationAdamx);
            adam[i].setCustomTranslation(AdamLetterOffset);
            adam[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            adam[i].setMode(modelModeAdam);
            adam[i].drawModel();
        };

        glBindVertexArray(0);
        glBindVertexArray(sphereVAO);

        sphereAdam.setCurrentShader(AffectedByLightingShader);
        sphereAdam.setCustomScaling(currentScaleFactorAdam);
        sphereAdam.setCustomRotation(charRotationAdamz * charRotationAdamy * charRotationAdamx);
        sphereAdam.setCustomTranslation(AdamLetterOffset);
        sphereAdam.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereAdam.drawModel();

        glBindVertexArray(0);
        glBindVertexArray(cubeVAO);

        //Avnish's letter and digit
        
        if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (choose == 6 || choose == 1)) {
            RNGvecAvnish = vec3(vec3(RNGpos(), 6.5f, RNGpos()));
            AvnishLetterOffset = RNGvecAvnish;
        }

        for (int i = 0; i < avnish.size(); i++) {
            //adding different texture
            if (i == 0) {
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, woodTextureID);
            }
            else if (i == 3) {
                glBindTexture(GL_TEXTURE_2D, 0);
                if (enableTexture == true)
                glBindTexture(GL_TEXTURE_2D, metalTextureID);
            }

            //applying custom model mod
            avnish[i].setCurrentShader(AffectedByLightingShader);
            avnish[i].setCustomScaling(currentScaleFactorAvnish);
            avnish[i].setCustomRotation(charRotationAvnishz * charRotationAvnishy * charRotationAvnishx);
            avnish[i].setCustomTranslation(AvnishLetterOffset);
            avnish[i].rotateWithWorld(rotateMatrixX, rotateMatrixY);
            avnish[i].setMode(modelModeAvnish);
            avnish[i].drawModel();
        };

        glBindVertexArray(0);
        glBindVertexArray(sphereVAO);

        sphereAvnish.setCurrentShader(AffectedByLightingShader);
        sphereAvnish.setCustomScaling(currentScaleFactorAvnish);
        sphereAvnish.setCustomRotation(charRotationAvnishz* charRotationAvnishy* charRotationAvnishx);
        sphereAvnish.setCustomTranslation(AvnishLetterOffset);
        sphereAvnish.rotateWithWorld(rotateMatrixX, rotateMatrixY);
        sphereAvnish.drawModel();

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);


        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            //turn mouse into zoom mode 
            FOV += dy / 100;
            if (FOV > 71) {
                FOV = 71;
            }
            else if (FOV < 69.2) {
                FOV = 69.2;
            }
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            cameraTiltAngle += dx / 100;
            cout << cameraTiltAngle << endl;
        }
        else {
            //mouse in normal fps mode 
            // Convert to spherical coordinate 
            cameraHorizontalAngle -= dx * mouseSensitivity * dt;
            cameraVerticalAngle -= dy * mouseSensitivity * dt;

            //limit the vertical camera angel
            if (cameraVerticalAngle > 85.0f) {
                cameraVerticalAngle = 85.0f;
            }
            else if (cameraVerticalAngle < -85.0f) {
                cameraVerticalAngle = -85.0f;
            }

            if (cameraHorizontalAngle > 360)
            {
                cameraHorizontalAngle -= 360;
            }
            else if (cameraHorizontalAngle < -360)
            {
                cameraHorizontalAngle += 360;
            }

            float theta = radians(cameraHorizontalAngle);
            float phi = radians(cameraVerticalAngle);

            cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
            vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

            glm::normalize(cameraSideVector);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // increase camera movement speed 
        {
            cameraSpeed = fastCameraSpeed;
        }
        else {
            cameraSpeed = normalCameraSpeed;
        }

        //using WASD to control movement adjusted 
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
        {
            cameraPosition.z -= cameraSpeed * dt * cameraLookAt.x;
            cameraPosition.x += cameraSpeed * dt * cameraLookAt.z;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition.z += cameraSpeed * dt * cameraLookAt.x;
            cameraPosition.x -= cameraSpeed * dt * cameraLookAt.z;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera forward
        {
            cameraPosition.x -= cameraSpeed * dt * cameraLookAt.x;
            cameraPosition.z -= cameraSpeed * dt * cameraLookAt.z;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera backwards
        {
            cameraPosition.x += cameraSpeed * dt * cameraLookAt.x;
            cameraPosition.z += cameraSpeed * dt * cameraLookAt.z;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition.y += cameraSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition.y -= cameraSpeed * dt;
        }

        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        AffectedByLightingShader.use();
        AffectedByLightingShader.setMat4("viewMatrix", viewMatrix);
        NotAffectedByLightingShader.use();
        NotAffectedByLightingShader.setMat4("viewMatrix", viewMatrix);


    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}

bool initContext() {     // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 1024x768
    window = glfwCreateWindow(1024, 768, "COMP 371 - PROJECT 2A", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return false;
    }
    return true;
}


//TODO Maybe this is wrong
GLuint loadTexture(const char* filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

//randomly generate character position 
float RNGpos() {
    return (rand() % 101 - 50);
}

//using cross product to find the normal vector to a surface 
vec3 crossProduct(vec3 point1, vec3 point2, vec3 point3) {
    vec3 vector1 = point2 - point1;
    vec3 vector2 = point3 - point1;
    return cross(vector1, vector2);
}

vector<vec3> generateSphere(float radius, int polyCount) {
    float sectorCount = polyCount;
    float stackCount = polyCount;
    vector<vec3> vertices;
    vector<vec3> result;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(vec3(x, y, z));
        }
    }

    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                result.push_back(vertices[k1]);
                result.push_back(vec3(0.3, 0.3, 0.3));
                result.push_back(vec3(0.0, 1.0, 0.0));
                result.push_back(vertices[k2]);
                result.push_back(vec3(0.3, 0.3, 0.3));
                result.push_back(vec3(0.0, 1.0, 0.0));
                result.push_back(vertices[k1 + 1]);
                result.push_back(vec3(0.3, 0.3, 0.3));
                result.push_back(vec3(0.0, 1.0, 0.0));
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                result.push_back(vertices[k1 + 1]);
                result.push_back(vec3(0.3, 0.3, 0.3));
                result.push_back(vec3(0.0, 1.0, 0.0));
                result.push_back(vertices[k2]);
                result.push_back(vec3(0.3, 0.3, 0.3));
                result.push_back(vec3(0.0, 1.0, 0.0));
                result.push_back(vertices[k2 + 1]);
                result.push_back(vec3(0.3, 0.3, 0.3));
                result.push_back(vec3(0.0, 1.0, 0.0));
            }
        }
    }
    return result;
}