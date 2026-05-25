cmake_minimum_required(VERSION 3.7)
project(Voxel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
set(GLAD_SRC GLAD/src/glad.c
        src/Renderer/Camera.cpp
        src/Renderer/Camera.h
        src/World/ChunkManager.cpp
        src/World/ChunkManager.h
        src/World/NoiseGenerator.cpp
        src/World/NoiseGenerator.h
        src/Physics/Physics.cpp
        src/Physics/Physics.h
        src/Physics/InputService.cpp
        src/Physics/InputService.h)

# Find OpenGL
# Prefer modern GLVND over legacy libGL
if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Define where your .proto file is
set(PROTO_SRC "${CMAKE_SOURCE_DIR}/proto/simulation.proto")
set(PROTO_OUT_DIR "${CMAKE_SOURCE_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_OUT_DIR})

# Generate C++ files from .proto
set(PROTO_GEN_SRCS "${PROTO_OUT_DIR}/simulation.pb.cc")
set(PROTO_GEN_HDRS "${PROTO_OUT_DIR}/simulation.pb.h")
set(GRPC_GEN_SRCS "${PROTO_OUT_DIR}/simulation.grpc.pb.cc")
set(GRPC_GEN_HDRS "${PROTO_OUT_DIR}/simulation.grpc.pb.h")

add_custom_command(
        OUTPUT "${PROTO_GEN_SRCS}" "${PROTO_GEN_HDRS}" "${GRPC_GEN_SRCS}" "${GRPC_GEN_HDRS}"
        COMMAND protobuf::protoc
        ARGS --cpp_out "${PROTO_OUT_DIR}"
        --grpc_out "${PROTO_OUT_DIR}"
        --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
        -I "${CMAKE_SOURCE_DIR}/proto"
        "${PROTO_SRC}"
        DEPENDS "${PROTO_SRC}"
)

# Create executable
add_executable(Voxel ${SOURCE_FILES} ${GLAD_SRC} ${PROTO_GEN_SRCS} ${GRPC_GEN_SRCS}
        src/World/Chunk.h
        src/World/Chunk.cpp
        src/Renderer/Renderer.h
        src/Renderer/Renderer.cpp
        src/Renderer/VertexBuffer.h
        src/Renderer/VertexBuffer.cpp
        src/Renderer/IndexBuffer.cpp
        src/Renderer/IndexBuffer.h
        src/Renderer/VertexArray.h
        src/Renderer/VertexArray.cpp
        src/Renderer/VertexBufferLayout.h
        src/Renderer/VertexBufferLayout.cpp)

# Link necessary libraries
target_link_libraries(Voxel
    glfw
    ${OPENGL_gl_LIBRARY}  # this is crucial on Linux
    dl                    # dynamic linker (often needed with OpenGL)
    gRPC::grpc++
    protobuf::libprotobuf
)

# Ensure the compiler can find the header files
target_include_directories(Voxel PRIVATE ${INCLUDE_DIR} ${PROTO_OUT_DIR})

# Copy resources and shaders
file(COPY resources DESTINATION ${BUILD_DIR})
file(COPY shaders DESTINATION ${BUILD_DIR})

file(GLOB SHADERS shaders/*.glsl)
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
        DEPENDS ${SHADERS}
)
add_dependencies(Voxel copy_shaders)

