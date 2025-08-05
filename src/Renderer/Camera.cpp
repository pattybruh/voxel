//
// Created by patrick on 8/4/25.
//

#include "Camera.h"

Camera::Camera(glm::vec3 pos) :
    m_position(pos),
    m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_up(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_pitch(0.0f),
    m_yaw(-90.0f),
    m_speed(10.0f),
    m_sens(0.1f)
{
}

void Camera::update(GLFWwindow *window, float delta_t, double mouse_x, double mouse_y) {
    float velocity = m_speed * delta_t;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_position += m_front * velocity;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_position -= glm::normalize(glm::cross(m_front, m_up))*velocity;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_position -= m_front * velocity;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_position += glm::normalize(glm::cross(m_front, m_up))*velocity;
    }

    glm::vec3 direction;
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    float xoffset = (mouse_x - m_last_x)*m_sens;
    float yoffset = (m_last_y - mouse_y)*m_sens;
    m_last_x = mouse_x;
    m_last_y = mouse_y;
    m_yaw += xoffset;
    m_pitch += yoffset;
    if(m_pitch > 89.0f) {
        m_pitch = 89.0f;
    }
    if(m_pitch < -89.0f) {
        m_pitch = -89.0f;
    }
    m_front = glm::normalize(direction);
}

glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(m_position, m_position+m_front, m_up);
}
