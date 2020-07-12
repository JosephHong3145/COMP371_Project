
#include <iostream>
#include <list>

#define GLEW_STATIC 1
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>


using namespace glm;
using namespace std;

const char* getVertexShaderSource();
const char* getFragmentShaderSource();
int compileAndLinkShaders();
int createVertexArrayObject();
bool initContext();

GLFWwindow* window = NULL;

//different settings you can change 

float FOV = 70.0f;
const float mouseSensitivity = 50.0f;

int main()
{
    if (!initContext()) return -1;

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Compile and link shaders here
    int shaderProgram = compileAndLinkShaders();
    glUseProgram(shaderProgram);
    GLint currentAxisLocation = glGetUniformLocation(shaderProgram, "currentAxis");

    //Initiating camera
    vec3 cameraPosition(0.6f, 1.0f, 10.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    float cameraTiltAngle = 90.0f;

    // Set projection matrix for shader, this won't change

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    // Upload to VAO 
    int vao = createVertexArrayObject();

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

    float textOffset = 8.0f;

    float currentScaleFactorJoseph = 1.0f;
    float currentScaleFactorJacob = 1.0f;
    float currentScaleFactorBad = 1.0f;

    float rotateFactorX = 0.0f;
    float rotateFactorY = 0.0f;
    mat4 rotateMatrixX = mat4(1.0f);
    mat4 rotateMatrixY = mat4(1.0f);
    int modelMode = 0; //0 = triangle, 1 = line, 2 = point
    int choose = 0; //choosing the model
    float cameraSpeed = 10.0f;
    float cameraFastSpeed = 2 * cameraSpeed;

    // Entering Game Loop
    while (!glfwWindowShouldClose(window))
    {
        glUniform1i(currentAxisLocation, 0);

        //changing projection view every second to accomodate for zoom
        mat4 projectionMatrix = glm::perspective(FOV, 1024.0f / 768.0f, 0.01f, 100.0f);
        GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        // Each frame, reset buffers
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw geometry
        glBindVertexArray(vao);

        GLuint worldMatrixLocation = NULL;

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

        switch(choose){
        case(1): currentScaleFactorJoseph *= (1 + scaleFactor);
            currentScaleFactorJacob *= (1 + scaleFactor);
            currentScaleFactorBad *= (1 + scaleFactor);
            break;
            
        case(2): currentScaleFactorJoseph *= (1 + scaleFactor);
            break;

        case(3): currentScaleFactorJacob *= (1 + scaleFactor);
            break;

        case(4): currentScaleFactorBad *= (1 + scaleFactor);
            break;

        default: currentScaleFactorJoseph *= (1 + scaleFactor);
            currentScaleFactorJacob *= (1 + scaleFactor);
            currentScaleFactorBad *= (1 + scaleFactor);
            break;
		}
        
        
        //Translating the model to directions using R,D,F,G on the keyboard
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            switch(choose){
            case(1): zOffSetJoseph += 0.01;
                zOffSetJacob += 0.01;
                zOffSetBad += 0.01;
                break;

            case(2): zOffSetJoseph += 0.01;
                break;
                
            case(3): zOffSetJacob += 0.01;
                break;

            case(4): zOffSetBad += 0.01;
                break;

            default: zOffSetJoseph += 0.01;
                zOffSetJacob += 0.01;
                zOffSetBad += 0.01;
                break;
			}
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            switch(choose){
            case(1): xOffSetJoseph -= 0.01;
                xOffSetJacob -= 0.01;
                xOffSetBad -= 0.01;
                break;

            case(2): xOffSetJoseph -= 0.01;
                break;
                
            case(3): xOffSetJacob -= 0.01;
                break;

            case(4): xOffSetBad -= 0.01;
                break;

            default: xOffSetJoseph -= 0.01;
                xOffSetJacob -= 0.01;
                xOffSetBad -= 0.01;
                break;
			}
        }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            switch(choose){
            case(1): xOffSetJoseph += 0.01;
                xOffSetJacob += 0.01;
                xOffSetBad += 0.01;
                break;

            case(2): xOffSetJoseph += 0.01;
                break;
                
            case(3): xOffSetJacob += 0.01;
                break;

            case(4): xOffSetBad += 0.01;
                break;

            default: xOffSetJoseph += 0.01;
                xOffSetJacob += 0.01;
                xOffSetBad += 0.01;
                break;
			}
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            switch(choose){
            case(1): zOffSetJoseph -= 0.01;
                zOffSetJacob -= 0.01;
                zOffSetBad -= 0.01;
                break;

            case(2): zOffSetJoseph -= 0.01;
                break;
                
            case(3): zOffSetJacob -= 0.01;
                break;

            case(4): zOffSetBad -= 0.01;
                break;

            default: zOffSetJoseph -= 0.01;
                zOffSetJacob -= 0.01;
                zOffSetBad -= 0.01;
                break;
			}
        }

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
        
        //Fast camera movement
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;

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
            modelMode = 0;
        }
        else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            modelMode = 1;
        }
        else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            modelMode = 2;
        }

        //choosing between the moodels
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            choose = 1;
        else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            choose = 2;
        else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            choose = 3;
        else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
            choose = 4;
        else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
            choose = 5;
        else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
            choose = 6;

        //=====================================================================================================================


        //drawing everything ==================================================================================================

        //drawing ground mesh 
        for (int i = 0; i < 100; i++) {
            //horizontal
            mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.01f, -50.0f + i)) * scale(mat4(1.0f), vec3(100.0f, 0.02f, 0.02f));
            groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
            worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);

            glDrawArrays(GL_LINES, 0, 36); // 36 vertices, starting at index 0


            //vertical
            groundWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + i, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 100.0f));
            groundWorldMatrix = rotateMatrixY * rotateMatrixX * groundWorldMatrix;
            worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);

            glDrawArrays(GL_LINES, 0, 36); // 36 vertices, starting at index 0

        }

        //drawing the XYZ line: 
        mat4 cordMarker = translate(mat4(1.0f), vec3(2.5f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.02f, 0.02f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &cordMarker[0][0]);
        glUniform1i(currentAxisLocation, 1);

        glDrawArrays(GL_LINES, 0, 36); // 36 vertices, starting at index 0

        cordMarker = translate(mat4(1.0f), vec3(0.0f, 3.5f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 5.0f, 0.02f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &cordMarker[0][0]);
        glUniform1i(currentAxisLocation, 2);

        glDrawArrays(GL_LINES, 0, 36); // 36 vertices, starting at index 0

        cordMarker = translate(mat4(1.0f), vec3(0.0f, 1.0f, 2.5f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 5.0f));
        cordMarker = rotateMatrixY * rotateMatrixX * cordMarker;
        worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &cordMarker[0][0]);
        glUniform1i(currentAxisLocation, 3);

        glDrawArrays(GL_LINES, 0, 36); // 36 vertices, starting at index 0
        glUniform1i(currentAxisLocation, 0);
        
        //drawing the letters

            
        //Jacob's letter and digit
        //C
        //vertical
        mat4 pillarWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + xOffSetJacob, 4.25f, -50.0f + zOffSetJacob) * currentScaleFactorJacob) * scale(mat4(1.0f), vec3(1.0f, 6.5f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode)
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
        pillarWorldMatrix = translate(mat4(1.0f), vec3(-48.0f + xOffSetJacob, 0.5f, -50.0f + zOffSetJacob) * currentScaleFactorJacob) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode)
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
        pillarWorldMatrix = translate(mat4(1.0f), vec3(-48.0f + xOffSetJacob, 8.0f, -50.0f + zOffSetJacob) * currentScaleFactorJacob) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode)
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
        pillarWorldMatrix = translate(mat4(1.0f), vec3(-42.0f + xOffSetJacob, 3.75f, 0.0f + zOffSetJacob) * currentScaleFactorJacob) * scale(mat4(1.0f), vec3(1.0f, 7.5f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode)
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

        pillarWorldMatrix = translate(mat4(1.0f), vec3(4.0f + xOffSetJacob, 6.0f, 0.0f + zOffSetJacob) * currentScaleFactorJacob) * scale(mat4(1.0f), vec3(1.0f, 3.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode)
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
        pillarWorldMatrix = translate(mat4(1.0f), vec3(6.0f + xOffSetJacob, 8.0f, 0.0f + zOffSetJacob) * currentScaleFactorJacob) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode)
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

        pillarWorldMatrix = translate(mat4(1.0f), vec3(5.5f + xOffSetJacob, 4.0f, 0.0f + zOffSetJacob) * currentScaleFactorJacob) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(4.0f, 1.0f, 1.0f) * currentScaleFactorJacob);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode)
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

        //vertical

        //Joseph's letter and digit
        //U
        pillarWorldMatrix = translate(mat4(1.0f), vec3(0.0f + xOffSetJoseph, 5.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * scale(mat4(1.0f), vec3(1.0f, 10.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        pillarWorldMatrix = translate(mat4(1.0f), vec3(5.0f + xOffSetJoseph, 5.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * scale(mat4(1.0f), vec3(1.0f, 10.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode) {
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
        pillarWorldMatrix = translate(mat4(1.0f), vec3(0.0f + textOffset + xOffSetJoseph, 5.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * scale(mat4(1.0f), vec3(1.0f, 10.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        pillarWorldMatrix = translate(mat4(1.0f), vec3(5.0f + textOffset + xOffSetJoseph, 2.5f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * scale(mat4(1.0f), vec3(1.0f, 5.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode) {
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
        pillarWorldMatrix = translate(mat4(1.0f), vec3(2.5f + xOffSetJoseph, 0.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode) {
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
        pillarWorldMatrix = translate(mat4(1.0f), vec3(2.5f + textOffset + xOffSetJoseph, 0.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        pillarWorldMatrix = translate(mat4(1.0f), vec3(2.5f + textOffset + xOffSetJoseph, 5.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode) {
        case 0: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        case 1: glDrawArrays(GL_LINES, 0, 36);
            break;
        case 2: glDrawArrays(GL_POINTS, 0, 36);
            break;
        default: glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        pillarWorldMatrix = translate(mat4(1.0f), vec3(2.5f + textOffset + xOffSetJoseph, 10.0f + yOffSetJoseph, 0.0f + zOffSetJoseph) * currentScaleFactorJoseph) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorJoseph);
        pillarWorldMatrix = rotateMatrixY * rotateMatrixX * pillarWorldMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
        switch (modelMode) {
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
        mat4 pillarWorldMatrix1 = translate(mat4(1.0f), vec3(-35.0f + xOffSetBad, 4.25f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * scale(mat4(1.0f), vec3(1.0f, 6.5f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix1[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //vertical right
        pillarWorldMatrix1 = translate(mat4(1.0f), vec3(-30.0f + xOffSetBad, 4.25f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * scale(mat4(1.0f), vec3(1.0f, 6.5f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix1[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //low bar
        pillarWorldMatrix1 = translate(mat4(1.0f), vec3(-33.0f + xOffSetBad, 0.5f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix1[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }
        //top bar
        pillarWorldMatrix1 = translate(mat4(1.0f), vec3(-33.0f + xOffSetBad, 8.0f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix1[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //2
       //vertical left
        mat4 pillarWorldMatrix2 = translate(mat4(1.0f), vec3(-25.0f + xOffSetBad, 2.25f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * scale(mat4(1.0f), vec3(1.0f, 3.5f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix2[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //vertical right
        pillarWorldMatrix2 = translate(mat4(1.0f), vec3(-20.0f + xOffSetBad, 6.25f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * scale(mat4(1.0f), vec3(1.0f, 2.5f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix2[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //low bar
        pillarWorldMatrix2 = translate(mat4(1.0f), vec3(-22.5f + xOffSetBad, 0.5f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix2[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //horizontal
        //middle bar
        pillarWorldMatrix2 = translate(mat4(1.0f), vec3(-22.5f + xOffSetBad, 4.5f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix2[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
            glDrawArrays(GL_POINTS, 0, 36);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
        }

        //top bar
        pillarWorldMatrix2 = translate(mat4(1.0f), vec3(-22.5f + xOffSetBad, 8.0f + yOffSetJoseph, 30.0f + zOffSetBad) * currentScaleFactorBad) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 1.0f, 1.0f) * currentScaleFactorBad);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix2[0][0]);
        switch (modelMode)
        {
        case 1:
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case 3:
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

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            //turn mouse into zoom mode 
            FOV += dy / 100;
            if (FOV > 71) {
                FOV = 71;
            }
            else if (FOV < 69.2) {
                FOV = 69.2;
            }
        }else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
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

        //using WADS to control movement adjusted 
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
        {
            cameraPosition.z -= currentCameraSpeed * dt * cameraLookAt.x;
            cameraPosition.x += currentCameraSpeed * dt * cameraLookAt.z;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition.z += currentCameraSpeed * dt * cameraLookAt.x;
            cameraPosition.x -= currentCameraSpeed * dt * cameraLookAt.z;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera forward
        {
            cameraPosition.x -= currentCameraSpeed * dt * cameraLookAt.x;
            cameraPosition.z -= currentCameraSpeed * dt * cameraLookAt.z;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera backwards
        {
            cameraPosition.x += currentCameraSpeed * dt * cameraLookAt.x;
            cameraPosition.z += currentCameraSpeed * dt * cameraLookAt.z;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition.y += currentCameraSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition.y -= currentCameraSpeed * dt;
        }

        mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}

const char* getVertexShaderSource()
{
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"
        "uniform mat4 projectionMatrix = mat4(1.0);"
        "uniform int currentAxis = 0;"
        "out vec3 vertexColor;"
        "void main()"
        "{"
        "   if(currentAxis == 1) { vertexColor = vec3(1, 0, 0); }"
        "   else if(currentAxis == 2) { vertexColor = vec3(0, 1, 0); }"
        "   else if(currentAxis == 3) { vertexColor = vec3(0, 0, 1); }"
        "   else { vertexColor = aColor; }"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "}";
}

const char* getFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "in vec3 vertexColor;"
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
        "}";
}

int compileAndLinkShaders()
{
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Error: Vertex shader compilation failed. " << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = getFragmentShaderSource();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Error: Fragment shader compilation failed. " << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Shader linking failed. " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int createVertexArrayObject()
{
    // Cube model
    vec3 vertexArray[] = {  // position,                            color
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(-0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f), // far - blue
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f),
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), // bottom - turquoise
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(-0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), // near - green
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), // right - purple
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f,-0.5f,-0.5f), vec3(0.6f, 0.6f, 0.6f),
        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f), // top - yellow
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.3f, 0.3f, 0.3f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.6f, 0.6f, 0.6f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f),

    };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindVertexArray(0);

    return vertexArrayObject;
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
