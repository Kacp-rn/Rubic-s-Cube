#pragma once

#include "../glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // * Initializes camera with position vector and euler angles
    // ? @param position - camera position (default: origin), @param up - world up vector, @param yaw - rotation around Y axis, @param pitch - rotation around X axis
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // * Initializes camera with individual position and up vector components
    // ? @param posX,posY,posZ - camera position components, @param upX,upY,upZ - world up components, @param yaw - Y rotation, @param pitch - X rotation
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // * Returns view matrix calculated using Euler Angles and LookAt transform
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // * Processes keyboard input and updates camera position based on direction and delta time
    // ? @param direction - movement direction enum (FORWARD/BACKWARD/LEFT/RIGHT), @param deltaTime - time since last frame
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // * Processes mouse movement and updates camera rotation (Yaw and Pitch) accordingly
    // ? @param xoffset - mouse X movement offset, @param yoffset - mouse Y movement offset, @param constrainPitch - limit pitch to prevent screen flip
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // * Adjusts camera zoom level based on mouse scroll wheel input
    // ? @param yoffset - scroll wheel offset
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    // * Sets camera position directly
    void SetPosition(const glm::vec3& position)
    {
        Position = position;
        updateCameraVectors();
    }

    // * Sets camera yaw and pitch directly, then updates direction vectors
    void SetOrientation(float yaw, float pitch)
    {
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // * Orients the camera to look at a target point from its current position
    void LookAt(const glm::vec3& target)
    {
        glm::vec3 direction = glm::normalize(target - Position);
        Pitch = glm::degrees(asin(glm::clamp(direction.y, -1.0f, 1.0f)));
        Yaw = glm::degrees(atan2(direction.z, direction.x));
        updateCameraVectors();
    }

private:
    // * Recalculates camera direction vectors (Front, Right, Up) from updated Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};