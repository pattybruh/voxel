#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
#include "stb_image.h"
#include "Simulation/Simulation.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
InputAction processInput(GLFWwindow *window, int agentId);

void printHeightmap(const StepResult& result) {
    const Observation& observation = result.observation;
    std::ostringstream output;
    constexpr int radius = Heightmap::SIZE / 2;
    output << std::fixed << std::setprecision(2);
    output << "\nTruncated: " << std::boolalpha << result.truncated << '\n';
    output << "\nHeightmap at feet position (" << observation.position.x << ", "
           << observation.position.y << ", " << observation.position.z << ")\n"
           << "Rows: x offsets +" << radius << " to " << -radius
           << "; columns: z offsets " << -radius << " to +" << radius << ".\n"
           << "Center: [" << radius << "][" << radius
           << "]; positive = above feet; -- = no surface.\n";

    const auto& reading = observation.heightmap;
    for (int x = Heightmap::SIZE-1; x >= 0; --x) {
        for (int z = 0; z < Heightmap::SIZE; ++z) {
            if (reading.valids[x][z]) {
                output << std::setw(9) << reading.heights[x][z];
            } else {
                output << std::setw(9) << "--";
            }
        }
        output << '\n';
    }
    std::cout << output.str() << std::flush;
}

// settings
constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;

//glfw and window init
bool windowSetup(GLFWwindow*& window){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment to fix compilation on OS X

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "voxel", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

int main()
{
    GLFWwindow* window;
    if(!windowSetup(window)) {
        return -1;
    }
    //shader program
    Shader shader("shaders/vertex.glsl", "shaders/frag.glsl");
    shader.use();
    glm::mat4 projection = glm::perspective(glm::radians(65.0f), 16.f/9.0f, 0.1f, 1000.0f);
    shader.setMat4("proj", projection);

    //PBody player = {1, {0.5,36, 0.5}, {0,0,0}, {0.3,0.9,0.3}};
    {
        const TerrainSettings terrain_settings{16.0f, 0.01f, 42};    //{amp, freq, seed}
        Simulation sim(terrain_settings);
        int playerId = sim.add_agent({0, 0});
        Renderer renderer;
        Camera camera(glm::vec3(20.0f, 20.0f, 20.0f));

        double prev_frame_time = glfwGetTime();
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        double acc = 0.0;
        constexpr float PHEIGHT = 1.75f;
        bool observation_key_was_down = false;

        //std::thread server_thread(RunServer);

        while (!glfwWindowShouldClose(window))
        {
            double curr_frame_time = glfwGetTime();
            double delta_time = curr_frame_time - prev_frame_time;
            delta_time = std::min(delta_time, 0.25);
            prev_frame_time = curr_frame_time;
            acc += delta_time;
            glfwGetCursorPos(window, &mouse_x, &mouse_y);
            camera.update(window, delta_time, mouse_x, mouse_y);

            InputAction action = processInput(window, playerId);

            while(acc >= Simulation::ACTION_DT) {
                sim.step({action});
                acc -= Simulation::ACTION_DT;
            }

            const bool observation_key_down = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
            if (observation_key_down && !observation_key_was_down) {
                printHeightmap(sim.observe(playerId));
            }
            observation_key_was_down = observation_key_down;

            camera.set_position(sim.get_agent(playerId).position+glm::vec3{0, PHEIGHT, 0});

            // render
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.setMat4("view", camera.get_view_matrix());

            sim.getChunk().update_dirty_chunks();
            sim.getChunk().render(renderer, shader);


            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        /*
        CloseServer();
        if(server_thread.joinable()) {
            server_thread.join();
        }
        */
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// Process all input, ESC to quit
// TODO: move out when gRPC is implemented
InputAction processInput(GLFWwindow *window, int agentId)
{
    InputAction action{};
    action.agentId = agentId;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        action.xDir = 1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        action.zDir = -1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        action.xDir = -1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        action.zDir = 1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        action.jump = true;
    }
    return action;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
