#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Chunk.h"
#include "stb_image.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

//camera
glm::vec3 camEye = glm::vec3(20.0f, 20.0f, 20.0f);
glm::vec3 camTarget = glm::vec3(8.0f, 0.0f, 8.0f);
glm::vec3 camDir = glm::normalize(camEye-camTarget);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camRight = glm::cross(camDir, camUp);

int main()
{
    //glfw: init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "voxel", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    //shader program
    Shader shader("shaders/vertex.glsl", "shaders/frag.glsl");

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    //view = glm::lookAt(camEye, camTarget, camUp);
    view = glm::lookAt(camEye, camEye+camFront, camUp);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(65.0f), 16.f/9.0f, 0.1f, 1000.0f);

    shader.use();
    int modelLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    int viewLoc = glGetUniformLocation(shader.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    int projLoc = glGetUniformLocation(shader.ID, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    const float radius = 15.0f;

    //Renderer renderer;
    {
        Chunk chunk;
        while (!glfwWindowShouldClose(window))
        {
            // input
            processInput(window);

            // render
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float camX = sin(glfwGetTime()*0.5)*radius;
            float camZ = cos(glfwGetTime()*0.5)*radius;
            //view = glm::lookAt(camEye, camEye+camFront, camUp);
            view = glm::lookAt(glm::vec3(camX, 10.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            shader.setMat4("view", view);

            //drawing a single chunk
            model = glm::mat4(1.0f);
            shader.setMat4("model", model);
            shader.use();
            chunk.render();
            //renderer.draw(*va, *ib);


            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
