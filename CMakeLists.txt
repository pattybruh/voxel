cmake_minimum_required(VERSION 3.7)
project(Voxel)

set(CMAKE_CXX_STANDARD 17)

#============================================================================
# Set directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

# Collect source files (don't include .h files here)
file(GLOB_RECURSE SOURCE_FILES ${SRC_DIR}/*.cpp ${SRC_DIR}/*.c)

# Include directories for headers
include_directories(GLAD/include)
include_directories(glm/include)

# Add dependencies (if you have a 'dependencies' folder, keep this)
add_subdirectory(dependencies)

# Add GLAD source directly
set(GLAD_SRC GLAD/src/glad.c)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Create executable
add_executable(Voxel ${SOURCE_FILES} ${GLAD_SRC}
        include/Chunk.h
        src/Chunk.cpp
        include/Renderer.h
        src/Renderer.cpp
        include/VertexBuffer.h
        src/VertexBuffer.cpp
        src/IndexBuffer.cpp
        include/IndexBuffer.h
        include/VertexArray.h
        src/VertexArray.cpp
        include/VertexBufferLayout.h
        src/VertexBufferLayout.cpp)

# Link necessary libraries
target_link_libraries(Voxel
    glfw
    ${OPENGL_gl_LIBRARY}  # this is crucial on Linux
    dl                    # dynamic linker (often needed with OpenGL)
)

# Ensure the compiler can find the header files
target_include_directories(Voxel PRIVATE ${INCLUDE_DIR})

# Copy resources and shaders
file(COPY resources DESTINATION ${BUILD_DIR})
file(COPY shaders DESTINATION ${BUILD_DIR})

file(GLOB SHADERS shaders/*.glsl)
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
        DEPENDS ${SHADERS}
)
add_dependencies(Voxel copy_shaders)

