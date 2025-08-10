#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "World/Chunk.h"
#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
#include "stb_image.h"
#include "Physics/Physics.h"
#include "World/ChunkManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

int main()
{
    //glfw: init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment to fix compilation on OS X

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //shader program
    Shader shader("shaders/vertex.glsl", "shaders/frag.glsl");
    shader.use();
    glm::mat4 projection = glm::perspective(glm::radians(65.0f), 16.f/9.0f, 0.1f, 1000.0f);
    shader.setMat4("proj", projection);

    {
        Renderer renderer;
        ChunkManager chunkman;
        Physics physics;
        PBody player = {1, {0,36,0}, {0,0,0}, {0.3,0.9,0.3} };

        Camera camera(glm::vec3(20.0f, 20.0f, 20.0f));

        double prev_frame_time = glfwGetTime();
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        double acc = 0.0;
        constexpr double DT = 1.0/60.0;
        constexpr float PHEIGHT = 1.75f;
        constexpr float PLAYERMS = 5.0f;
        while (!glfwWindowShouldClose(window))
        {
            double curr_frame_time = glfwGetTime();
            if ((int)(curr_frame_time*10) % 10 == 0) {
                std::cout << "pos{" << player.position.x << ", " << player.position.y
                          << ", " << player.position.z << "} onGround=" << player.is_grounded<< "\n";
            }

            double delta_time = curr_frame_time - prev_frame_time;
            delta_time = std::min(delta_time, 0.25);
            prev_frame_time = curr_frame_time;
            acc += delta_time;
            glfwGetCursorPos(window, &mouse_x, &mouse_y);
            camera.update(window, delta_time, mouse_x, mouse_y);

            // input
            move_player_horizontal(window, camera, player, PLAYERMS);
            try_jump(window, player);
            processInput(window);

            while(acc >= DT) {
                physics.step(chunkman, player, DT);
                acc -= DT;
            }
            camera.set_position(player.position+glm::vec3{0, PHEIGHT, 0});

            // render
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.setMat4("view", camera.get_view_matrix());

            chunkman.update_dirty_chunks();
            chunkman.render(renderer, shader);


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
