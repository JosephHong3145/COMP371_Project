
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

using namespace glm;
using namespace std;

//const char* getVertexShaderSource();
//const char* getFragmentShaderSource();
//int createVertexArrayObject();
bool initContext();

GLFWwindow* window = NULL;

//different settings you can change 
float normalCameraSpeed = 10.0f;
float fastCameraSpeed = 50.0f;
float FOV = 70.0f;
const float mouseSensitivity = 50.0f;

// settings
const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;

// lighting
vec3 lightPos(2.0f, 2.0f, 2.0f);

int main()
{
    if (!initContext()) return -1;

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Compile and link shaders here
    //int shaderProgram = compileAndLinkShaders();
    Shader lightingShader("2.2.basic_lighting.vert", "2.2.basic_lighting.frag");
    Shader modelShader("model.vert", "model.frag");
    //GLint currentAxisLocation = glGetUniformLocation(shaderProgram, "currentAxis");

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

    modelShader.setMat4("viewMatrix", viewMatrix);
    //GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    //glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    // Upload to VAO 
    //int vao = createVertexArrayObject();

    /////////////////////////////////////////////////////////
    // Start
       
    // Cube model
    vec3 vertexArray[] = {  // position, color, normal
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(0.0f,  0.0f, -1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(0.0f,  0.0f, -1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(0.0f,  0.0f, -1.0f),

        vec3(-0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(0.0f,  0.0f, -1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(0.0f,  0.0f, -1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(0.0f,  0.0f, -1.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(0.0f,  0.0f,  1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(0.0f,  0.0f,  1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(0.0f,  0.0f,  1.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(0.0f,  0.0f,  1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(0.0f,  0.0f,  1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(0.0f,  0.0f,  1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(-1.0f,  0.0f,  0.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(-1.0f,  0.0f,  0.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(-1.0f,  0.0f,  0.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(-1.0f,  0.0f,  0.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(-1.0f,  0.0f,  0.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(-1.0f,  0.0f,  0.0f),

        vec3(-0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(1.0f,  0.0f,  0.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(1.0f,  0.0f,  0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(1.0f,  0.0f,  0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(1.0f,  0.0f,  0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(1.0f,  0.0f,  0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(1.0f,  0.0f,  0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(0.0f, -1.0f,  0.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(0.0f, -1.0f,  0.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(0.0f, -1.0f,  0.0f),

        vec3(0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(0.0f, -1.0f,  0.0f),
        vec3(0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(0.0f, -1.0f,  0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(0.0f, -1.0f,  0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(0.0f,  1.0f,  0.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(0.0f,  1.0f,  0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(0.0f,  1.0f,  0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), vec3(0.0f,  1.0f,  0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f), vec3(0.0f,  1.0f,  0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.1f, 0.1f, 0.1f), vec3(0.0f,  1.0f,  0.0f),

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
    GLuint lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(0);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);

    //END
    //////////////////////////////////////////////////

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    //enable OpenGL components
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    float scaleFactor = 0.0f;

    float xOffSetJoseph = 0.0f;
    float yOffSetJoseph = 10.0f;
    float zOffSetJoseph = 0.0f;

    float xOffSetJacob = 0.0f;
    float yOffSetJacob = 0.0f;
    float zOffSetJacob = 0.0f;

    float xOffSetBad = 0.0f;
    float yOffSetBad = 0.0f;
    float zOffSetBad = 0.0f;

    float xOffSetAdam = 0.0f;
    float yOffSetAdam = 0.0f;
    float zOffSetAdam = 0.0f;

    float xOffSetAvnish = 0.0f;
    float yOffSetAvnish = 0.0f;
    float zOffSetAvnish = 0.0f;

    float textOffset = 8.0f;

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

    int choose = 0; //choosing the model
    float charRotateFactor = 0.0f;
    float charRotateFactorJacob = 0.0f;
    float charRotateFactorBad = 0.0f;
    float charRotateFactorAdam = 0.0f;
    float charRotateFactorAvnish = 0.0f;

    mat4 charRotation = mat4(1.0f);
    mat4 charRotationJacob = mat4(1.0f);
    mat4 charRotationBad = mat4(1.0f);
    mat4 charRotationAdam = mat4(1.0f);
    mat4 charRotationAvnish = mat4(1.0f);

    // Entering Game Loop
    while (!glfwWindowShouldClose(window))
    {
        lightingShader.use();
        modelShader.use();

        modelShader.setInt("currentAxis", 0);

        //changing projection view every second to accomodate for zoom
        mat4 projectionMatrix = perspective(FOV, 1024.0f / 768.0f, 0.01f, 1000.0f);
        modelShader.setMat4("projectionMatrix", projectionMatrix);

        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        // Each frame, reset buffers
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

         //be sure to activate shader when setting uniforms/drawing objects
        //lightingShader.setVec3("objectColor", 0.5f, 0.5f, 0.31f);
        //lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        //lightingShader.setVec3("lightPos", lightPos);
        //lightingShader.setVec3("viewPos", cameraPosition);

        //// view/projection transformations
        //lightingShader.setMat4("projection", projectionMatrix);
        //lightingShader.setMat4("view", viewMatrix);

        // //world transformation
        //lightingShader.setMat4("model", mat4(1.0f));

        // //Draw geometry
        //glBindVertexArray(lightCubeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0);
        //glBindVertexArray(cubeVAO);

        //modules for controlling model and world behaviour =================================================================================================================

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
            case(1): zOffSetJoseph += 0.1;
                zOffSetJacob += 0.1;
                zOffSetBad += 0.1;
                zOffSetAdam += 0.1;
                zOffSetAvnish += 0.1;
                break;

            case(2): zOffSetJoseph += 0.1;
                break;

            case(3): zOffSetJacob += 0.1;
                break;

            case(4): zOffSetBad += 0.1;
                break;
            case(5): zOffSetAdam += 0.1;
                break;
            case(6): zOffSetAvnish += 0.1;
                break;
            default: zOffSetJoseph += 0.1;
                zOffSetJacob += 0.1;
                zOffSetBad += 0.1;
                zOffSetAdam += 0.1;
                zOffSetAvnish += 0.1;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            switch (choose) {
            case(1): xOffSetJoseph -= 0.1;
                xOffSetJacob -= 0.1;
                xOffSetBad -= 0.1;
                xOffSetAdam -= 0.1;
                xOffSetAvnish -= 0.1;
                break;

            case(2): xOffSetJoseph -= 0.1;
                break;

            case(3): xOffSetJacob -= 0.1;
                break;

            case(4): xOffSetBad -= 0.1;
                break;

            case(5): xOffSetAdam -= 0.1;
                break;
            case(6): xOffSetAvnish -= 0.1;
                break;
            default: xOffSetJoseph -= 0.1;
                xOffSetJacob -= 0.1;
                xOffSetBad -= 0.1;
                xOffSetAdam -= 0.1;
                xOffSetAvnish -= 0.1;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            switch (choose) {
            case(1): xOffSetJoseph += 0.1;
                xOffSetJacob += 0.1;
                xOffSetBad += 0.1;
                xOffSetAdam += 0.1;
                xOffSetAvnish += 0.1;
                break;

            case(2): xOffSetJoseph += 0.1;
                break;

            case(3): xOffSetJacob += 0.1;
                break;

            case(4): xOffSetBad += 0.1;
                break;

            case(5): xOffSetAdam += 0.1;
                break;

            case(6): xOffSetAvnish += 0.1;
                break;

            default: xOffSetJoseph += 0.1;
                xOffSetJacob += 0.1;
                xOffSetBad += 0.1;
                xOffSetAdam += 0.1;
                xOffSetAvnish += 0.1;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            switch (choose) {
            case(1): zOffSetJoseph -= 0.1;
                zOffSetJacob -= 0.1;
                zOffSetBad -= 0.1;
                zOffSetAdam -= 0.1;
                zOffSetAvnish -= 0.1;
                break;

            case(2): zOffSetJoseph -= 0.1;
                break;

            case(3): zOffSetJacob -= 0.1;
                break;

            case(4): zOffSetBad -= 0.1;
                break;

            case(5): zOffSetAdam -= 0.1;
                break;

            case(6): zOffSetAvnish -= 0.1;
                break;

            default: zOffSetJoseph -= 0.1;
                zOffSetJacob -= 0.1;
                zOffSetBad -= 0.1;
                zOffSetAdam -= 0.1;
                zOffSetAvnish -= 0.1;
                break;
            }
        }

        //changing the orientation of the letters

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            switch (choose) {
            case(1): charRotateFactor += 0.1f;
                charRotateFactorJacob += 0.1f;
                charRotateFactorBad += 0.1f;
                charRotateFactorAdam += 0.1f;
                charRotateFactorAvnish += 0.1f;
                break;
            case(2): charRotateFactor += 0.1f;
                break;
            case(3): charRotateFactorJacob += 0.1f;
                break;
            case(4): charRotateFactorBad += 0.1f;
                break;
            case(5): charRotateFactorAdam += 0.1f;
                break;
            case (6): charRotateFactorAvnish += 0.1f;
                break;
            default: charRotateFactor += 0.1f;
                charRotateFactorJacob += 0.1f;
                charRotateFactorBad += 0.1f;
                charRotateFactorAdam += 0.1f;
                charRotateFactorAvnish += 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            switch (choose) {
            case(1): charRotateFactor -= 0.1f;
                charRotateFactorJacob -= 0.1f;
                charRotateFactorBad -= 0.1f;
                charRotateFactorAdam -= 0.1f;
                charRotateFactorAvnish -= 0.1f;
                break;
            case(2): charRotateFactor -= 0.1f;
                break;
            case(3): charRotateFactorJacob -= 0.1f;
                break;
            case(4): charRotateFactorBad -= 0.1f;
                break;
            case(5): charRotateFactorAdam -= 0.1f;
                break;
            case(6): charRotateFactorAvnish -= 0.1f;
                break;
            default: charRotateFactor -= 0.1f;
                charRotateFactorJacob -= 0.1f;
                charRotateFactorBad -= 0.1f;
                charRotateFactorAdam -= 0.1f;
                charRotateFactorAvnish -= 0.1f;
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            charRotateFactor = 0.0f;
            charRotateFactorJacob = 0.0f;
            charRotateFactorBad = 0.0f;
            charRotateFactorAdam = 0.0f;
            charRotateFactorAvnish = 0.0f;
        }

        charRotation = rotate(mat4(1.0f), radians(charRotateFactor), vec3(0.0f, 0.0f, 1.0f));
        charRotationJacob = rotate(mat4(1.0f), radians(charRotateFactorJacob), vec3(0.0f, 0.0f, 1.0f));
        charRotationBad = rotate(mat4(1.0f), radians(charRotateFactorBad), vec3(0.0f, 0.0f, 1.0f));
        charRotationAdam = rotate(mat4(1.0f), radians(charRotateFactorAdam), vec3(0.0f, 0.0f, 1.0f));
        charRotationAvnish = rotate(mat4(1.0f), radians(charRotateFactorAvnish), vec3(0.0f, 0.0f, 1.0f));


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

        //GLuint rotationMatrixLocation = glGetUniformLocation(shaderProgram, "rotationMatrix");
        //glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

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
            xOffSetJoseph = 0.0;
            xOffSetJacob = 0.0;
            xOffSetBad = 0.0;
            xOffSetAdam = 0.0;
            xOffSetAvnish = 0.0;

            zOffSetJoseph = 0.0;
            zOffSetJacob = 0.0;
            zOffSetBad = 0.0;
            zOffSetAdam = 0.0;
            zOffSetAvnish = 0.0;
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

        //=====================================================================================================================


        //drawing everything ==================================================================================================
        glBindVertexArray(cubeVAO);
        //drawing ground mesh 
        for (int i = 0; i <= 100; i++) {
            //horizontal
            mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.01f, -50.0f + i)) * scale(mat4(1.0f), vec3(100.0f, 0.02f, 0.02f));
            groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
            modelShader.setMat4("worldMatrix", groundWorldMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0


            //vertical
            groundWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + i, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 100.0f));
            groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
            modelShader.setMat4("worldMatrix", groundWorldMatrix);


            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        }

        //drawing the XYZ line: 
        mat4 cordMarker = translate(mat4(1.0f), vec3(2.5f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.02f, 0.02f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        modelShader.setMat4("worldMatrix", cordMarker);
        modelShader.setInt("currentAxis", 1);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        cordMarker = translate(mat4(1.0f), vec3(0.0f, 3.5f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 5.0f, 0.02f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        modelShader.setMat4("worldMatrix", cordMarker);
        modelShader.setInt("currentAxis", 2);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        cordMarker = translate(mat4(1.0f), vec3(0.0f, 1.0f, 2.5f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 5.0f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        modelShader.setMat4("worldMatrix", cordMarker);
        modelShader.setInt("currentAxis", 3);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        modelShader.setInt("currentAxis", 0);

        glBindVertexArray(0);
        // also draw the lamp object
        glBindVertexArray(cubeVAO);
        mat4 world = translate(mat4(1.0f), vec3(lightPos)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
        world = rotateMatrixY * rotateMatrixX * world;
        modelShader.setMat4("worldMatrix", world);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glBindVertexArray(cubeVAO);
        //drawing the letters

        //Jacob's letter and digit
        //C
        //vertical
        mat4 pillarWorldMatrix = charRotationJacob * translate(mat4(1.0f), vec3(-49.5f + xOffSetJacob, 4.25f, -49.5f + zOffSetJacob) * currentScaleFactorJacob) * scale(mat4(1.0f), vec3(1.0f, 6.5f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJacob)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //low bar
        pillarWorldMatrix = charRotationJacob * translate(mat4(1.0f), vec3(-47.5f + xOffSetJacob, 0.5f, -49.5f + zOffSetJacob) * currentScaleFactorJacob) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJacob)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        //top bar
        pillarWorldMatrix = charRotationJacob * translate(mat4(1.0f), vec3(-47.5f + xOffSetJacob, 8.0f, -49.5f + zOffSetJacob) * currentScaleFactorJacob) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJacob)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }


        //9
        //vertical
        pillarWorldMatrix = charRotationJacob * translate(mat4(1.0f), vec3(-45.5f + xOffSetJacob + textOffset, 3.75f, -49.5f + zOffSetJacob) * currentScaleFactorJacob) * scale(mat4(1.0f), vec3(1.0f, 7.5f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJacob)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        pillarWorldMatrix = charRotationJacob * translate(mat4(1.0f), vec3(-49.5f + xOffSetJacob + textOffset, 6.0f, -49.5f + zOffSetJacob) * currentScaleFactorJacob) * scale(mat4(1.0f), vec3(1.0f, 3.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJacob)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        pillarWorldMatrix = charRotationJacob * translate(mat4(1.0f), vec3(-47.5f + xOffSetJacob + textOffset, 8.0f, -49.5f + zOffSetJacob) * currentScaleFactorJacob) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJacob)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        pillarWorldMatrix = charRotationJacob * translate(mat4(1.0f), vec3(-48.0f + xOffSetJacob + textOffset, 4.0f, -49.5f + zOffSetJacob) * currentScaleFactorJacob) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(4.0f, 1.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJacob) {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //vertical

        //Joseph's letter and digit
        //U

        pillarWorldMatrix = charRotation * translate(mat4(1.0f), vec3(-6.0f + xOffSetJoseph, 5.5f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * scale(mat4(1.0f), vec3(1.0f, 10.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJoseph) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }


        pillarWorldMatrix = charRotation * translate(mat4(1.0f), vec3(-2.0f + xOffSetJoseph, 5.5f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * scale(mat4(1.0f), vec3(1.0f, 10.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJoseph) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        //6

        pillarWorldMatrix = charRotation * translate(mat4(1.0f), vec3(-6.0f + textOffset + xOffSetJoseph, 5.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * scale(mat4(1.0f), vec3(1.0f, 9.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJoseph) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        pillarWorldMatrix = charRotation * translate(mat4(1.0f), vec3(-2.0f + textOffset + xOffSetJoseph, 2.5f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * scale(mat4(1.0f), vec3(1.0f, 4.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJoseph) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        //horizontal

        //U

        pillarWorldMatrix = charRotation * translate(mat4(1.0f), vec3(-4.0f + xOffSetJoseph, 0.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJoseph) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        //6

        pillarWorldMatrix = charRotation * translate(mat4(1.0f), vec3(-4.0f + textOffset + xOffSetJoseph, 0.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJoseph) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        pillarWorldMatrix = charRotation * translate(mat4(1.0f), vec3(-3.5f + textOffset + xOffSetJoseph, 5.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(4.0f, 1.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJoseph) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        pillarWorldMatrix = charRotation * translate(mat4(1.0f), vec3(-4.0f + textOffset + xOffSetJoseph, 10.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeJoseph) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        // Badreddine letter and digit
        //D
        //vertical left
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-49.5f + xOffSetBad, 4.25f, 49.5f + zOffSetBad) * currentScaleFactorBad) * scale(mat4(1.0f), vec3(1.0f, 6.5f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //vertical right
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-44.5f + xOffSetBad, 4.25f, 49.5f + zOffSetBad) * currentScaleFactorBad) * scale(mat4(1.0f), vec3(1.0f, 6.5f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //low bar
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-47.5f + xOffSetBad, 0.5f, 49.5f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        //top bar
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-47.5f + xOffSetBad, 8.0f, 49.5f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //2
       //vertical left
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-49.5f + xOffSetBad + textOffset, 2.25f, 49.5f + zOffSetBad) * currentScaleFactorBad) * scale(mat4(1.0f), vec3(1.0f, 3.5f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //vertical right
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-44.5f + xOffSetBad + textOffset, 6.25f, 49.5f + zOffSetBad) * currentScaleFactorBad) * scale(mat4(1.0f), vec3(1.0f, 2.5f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //low bar
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-47.0f + xOffSetBad + textOffset, 0.5f, 49.5f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //middle bar
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-47.0f + xOffSetBad + textOffset, 4.5f, 49.5f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //top bar
        pillarWorldMatrix = charRotationBad * translate(mat4(1.0f), vec3(-47.0f + xOffSetBad + textOffset, 8.0f, 49.5f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeBad)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //Adam's Letter and digit
        //A
        //vertical bars
        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(44.5 + xOffSetAdam - textOffset, 3.75f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * scale(mat4(1.0f), vec3(1.0f, 7.5f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(49.5f + xOffSetAdam - textOffset, 3.75f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * scale(mat4(1.0f), vec3(1.0f, 7.5f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //middle bar
        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(46.5f + xOffSetAdam - textOffset, 5.0f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        //top bar
        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(47.0f + xOffSetAdam - textOffset, 8.0f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //2
       //vertical left
        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(44.5f + xOffSetAdam, 2.75f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * scale(mat4(1.0f), vec3(1.0f, 3.5f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //vertical right
        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(49.5f + xOffSetAdam, 6.25f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * scale(mat4(1.0f), vec3(1.0f, 2.5f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //low bar
        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(47.0f + xOffSetAdam, 0.5f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //middle bar
        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(47.0f + xOffSetAdam, 4.5f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //top bar
        pillarWorldMatrix = charRotationAdam * translate(mat4(1.0f), vec3(47.0f + xOffSetAdam, 8.0f, -49.5f + zOffSetAdam) * currentScaleFactorAdam) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorAdam);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAdam)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //Avnish's letter and digit
        //n
        //vertical right
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(49.5f + xOffSetAvnish - textOffset, 4.25f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * scale(mat4(1.0f), vec3(1.0f, 8.5f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //vertical left
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(44.5f + xOffSetAvnish - textOffset, 4.25f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * scale(mat4(1.0f), vec3(1.0f, 8.5f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //diagonal top left
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(45.5f + xOffSetAvnish - textOffset, 7.25f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * scale(mat4(1.0f), vec3(1.0f, 2.5f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //diagonal bottom right
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(48.5f + xOffSetAvnish - textOffset, 1.25f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * scale(mat4(1.0f), vec3(1.0f, 2.5f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //middle left
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(47.5f + xOffSetAvnish - textOffset, 3.25f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * scale(mat4(1.0f), vec3(1.0f, 2.25f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //middle right
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(46.5f + xOffSetAvnish - textOffset, 5.25f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * scale(mat4(1.0f), vec3(1.0f, 2.25f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //2
        //vertical left
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(44.5f + xOffSetAvnish, 2.75f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * scale(mat4(1.0f), vec3(1.0f, 3.5f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //vertical right
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(49.5f + xOffSetAvnish, 6.25f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * scale(mat4(1.0f), vec3(1.0f, 2.5f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //low bar
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(47.0f + xOffSetAvnish, 0.5f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //middle bar
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(47.0f + xOffSetAvnish, 4.5f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //top bar
        pillarWorldMatrix = charRotationAvnish * translate(mat4(1.0f), vec3(47.0f + xOffSetAvnish, 8.0f, 49.5f + zOffSetAvnish) * currentScaleFactorAvnish) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorAvnish);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        modelShader.setMat4("worldMatrix", pillarWorldMatrix);
        switch (modelModeAvnish)
        {
        case 0:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 1:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        glBindVertexArray(0);

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
            cameraVerticalAngle = max(-85.0f, min(85.0f, cameraVerticalAngle));
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

        mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        modelShader.setMat4("viewMatrix", viewMatrix);
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

    // Create Window and rendering context using GLFW, resolution is 800x600
    window = glfwCreateWindow(1024, 768, "COMP 371 - PROJECT 1A", NULL, NULL);
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