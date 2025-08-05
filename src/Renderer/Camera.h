//
// Created by patrick on 8/4/25.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera {
private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    float m_pitch;
    float m_yaw;
    float m_speed;
    float m_ms;
    float m_sens;
    float m_last_x;
    float m_last_y;

public:
    Camera(glm::vec3 pos);

    void update(GLFWwindow* window, float delta_t, double mouse_x, double mouse_y);
    glm::mat4 get_view_matrix() const;
};



#endif //CAMERA_H
