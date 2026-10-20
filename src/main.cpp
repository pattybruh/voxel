#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Renderer/Camera.h"
#include "Renderer/FlagRenderer.h"
#include "Renderer/Renderer.h"
#include "stb_image.h"
#include "Simulation/Simulation.h"
#include "Simulation/SimulationServer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

int main(int argc, char* argv[])
{
    std::string listen_address = "0.0.0.0:50051";
    for(int i = 1; i < argc; ++i){
        const std::string arg = argv[i];
        if(arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [--listen ADDRESS]\n"
                      << "  --listen ADDRESS  gRPC server address (default: 0.0.0.0:50051)\n";
            return 0;
        }
        if(arg == "--listen"){
            if(i + 1 >= argc || std::string(argv[i + 1]).empty() || argv[i + 1][0] == '-'){
                std::cerr << "--listen requires an address, for example localhost:50052\n";
                return 1;
            }
            listen_address = argv[++i];
        }
        else{
            std::cerr << "Unknown argument: " << arg << ". Use --help for usage.\n";
            return 1;
        }
    }

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
        Simulation sim(terrain_settings, glm::vec2{40, 20});
        SimulationServer server(listen_address);
        Renderer renderer;
        FlagRenderer flag_renderer;
        Camera camera(glm::vec3(20.0f, 20.0f, 20.0f));

        double prev_frame_time = glfwGetTime();
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        constexpr float PHEIGHT = 1.75f;
        bool observation_key_was_down = false;

        while (!glfwWindowShouldClose(window))
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
                break;
            }

            double curr_frame_time = glfwGetTime();
            double delta_time = curr_frame_time - prev_frame_time;
            delta_time = std::min(delta_time, 0.25);
            prev_frame_time = curr_frame_time;
            glfwGetCursorPos(window, &mouse_x, &mouse_y);
            camera.update(window, delta_time, mouse_x, mouse_y);

            server.processPendingCmds(sim);
            const auto playerId = sim.sole_agent_id();

            const bool observation_key_down = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
            if (playerId && observation_key_down && !observation_key_was_down) {
                printHeightmap(sim.observe(*playerId));
            }
            observation_key_was_down = observation_key_down;

            if (playerId) {
                camera.set_position(sim.get_agent(*playerId).position+glm::vec3{0, PHEIGHT, 0});
            }

            // render
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.setMat4("view", camera.get_view_matrix());

            sim.getChunk().update_dirty_chunks();
            sim.getChunk().render(renderer, shader);
            flag_renderer.draw(renderer, shader, sim.flag_position());


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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
