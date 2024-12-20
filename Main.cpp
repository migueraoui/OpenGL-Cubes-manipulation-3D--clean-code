#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

// Screen dimensions
const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

// Cube vertices and indices
GLfloat cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f
};

GLuint cubeIndices[] = {
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    0, 1, 5, 0, 5, 4,
    2, 3, 7, 2, 7, 6,
    0, 3, 7, 0, 7, 4,
    1, 2, 6, 1, 6, 5
};

// Function to initialize a cube
void InitializeCube(VAO& vao, VBO& vbo, EBO& ebo) {
    vao.Bind();
    vbo = VBO(cubeVertices, sizeof(cubeVertices));
    ebo = EBO(cubeIndices, sizeof(cubeIndices));
    vao.LinkVBO(vbo, 0);
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rotating Cubes", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Compile shaders
    Shader shaderProgram("default.vert", "default.frag");

    // Initialize cubes
    VAO cubeVAO1, cubeVAO2;
    VBO cubeVBO1, cubeVBO2;
    EBO cubeEBO1, cubeEBO2;

    InitializeCube(cubeVAO1, cubeVBO1, cubeEBO1);
    InitializeCube(cubeVAO2, cubeVBO2, cubeEBO2);

    // Rotation variables
    float rotationY = 0.0f; // Rotation for the first cube (around Y)
    float rotationZ = 0.0f; // Rotation for the second cube (around Z)
    float sceneRotation = 0.0f; // Scene rotation around Y (global axis)
    double previousTime = glfwGetTime();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Handle ESC key to close the window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Clear the screen
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader program
        shaderProgram.Activate();

        // Update rotation
        double currentTime = glfwGetTime();
        if (currentTime - previousTime >= 1.0 / 60.0) {
            rotationY += 0.5f;
            rotationZ += 0.5f;
            sceneRotation += 0.1f;
            previousTime = currentTime;
        }

        // Create camera transformation
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

        // Global scene rotation
        glm::mat4 sceneRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(sceneRotation), glm::vec3(0.0f, 1.0f, 0.0f));

        // Draw the first cube (rotation around Y)
        glm::mat4 model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 1.0f, 0.0f));
        model1 = glm::rotate(model1, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        model1 = glm::scale(model1, glm::vec3(0.5f));
        model1 = sceneRotationMatrix * model1;

        int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        int colorLoc = glGetUniformLocation(shaderProgram.ID, "color");
        glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); // Green for the first cube

        cubeVAO1.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Draw the second cube (rotation around Z)
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -1.0f, 0.0f));
        model2 = glm::rotate(model2, glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
        model2 = glm::scale(model2, glm::vec3(0.5f));
        model2 = sceneRotationMatrix * model2;

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
        glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f); // Blue for the second cube

        cubeVAO2.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up resources
    cubeVAO1.Delete();
    cubeVBO1.Delete();
    cubeEBO1.Delete();
    cubeVAO2.Delete();
    cubeVBO2.Delete();
    cubeEBO2.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

