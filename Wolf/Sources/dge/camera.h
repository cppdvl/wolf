namespace DGE{
    /******************************************************************************************************************/
    /** -- Camera Mouse/Keyboard -- **********************************************************************************/
    /* An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL out of keyboard and mouse movement.
     */
    constexpr float DEFAULT_YAW         = -0.0f;
    constexpr float DEFAULT_PITCH_      =  0.0f;    // The _ it's because there's another DEFAULT_PITCH var in GDI.h
    constexpr float DEFAULT_ZOOM        =  45.0f;
    class ICamera
    {
    public:
        enum class Camera_Movement {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT
        };
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        float Yaw {DGE::DEFAULT_YAW};
        float Pitch {DGE::DEFAULT_PITCH_};
        float Zoom {DGE::DEFAULT_ZOOM};
        float Fov {45.0f};
        float AspectRatio {1.333333f};
        float ZFar{100.0f};
        float ZNear{0.1f};

        ICamera(
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
            float yaw = DGE::DEFAULT_YAW,
            float pitch = DGE::DEFAULT_PITCH_)
        : 
        Front(glm::vec3(0.0f, 0.0f, -1.0f))
        {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        ICamera(
            float posX, 
            float posY, 
            float posZ, 
            float upX, 
            float upY, 
            float upZ, 
            float yaw, 
            float pitch)
        : 
        Front(glm::vec3(0.0f, 0.0f, -1.0f))
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }
        glm::mat4 GetViewMatrix()
        {
            return glm::mat4 {1.0f};
            return glm::lookAt(Position, Position + Front, Up);
        }
        glm::mat4 GetProjectionMatrix()
        {
            return glm::perspective(glm::radians(Fov), AspectRatio, ZNear, ZFar);
        }

    protected:
        void updateCameraVectors()
        {
            /* calculate the new 
             * Front vector */
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            /* also re-calculate 
             * the Right and Up vector */
            Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up    = glm::normalize(glm::cross(Right, Front));
        }
    };
    
    const float MKBSPEED    =  2.5f;
    const float MSENSITIVITY =  0.1f;
    class Camera_MouseKeyboard : public ICamera
    {
    public:
        float MovementSpeed{ DGE::MKBSPEED };
        float MouseSensitivity{ DGE::MSENSITIVITY };

        Camera_MouseKeyboard(
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float yaw = DGE::DEFAULT_YAW,
            float pitch = DGE::DEFAULT_PITCH_)
        :
        ICamera(position, up, yaw, pitch)
        {

        }

        Camera_MouseKeyboard(
            float posX,
            float posY,
            float posZ,
            float upX,
            float upY,
            float upZ,
            float yaw,
            float pitch)
        :
        ICamera(posX, posY, posZ, upX, upY, upZ, yaw, pitch)
        {

        }

        void ProcessKeyboard(DGE::ICamera::Camera_Movement direction, float deltaTime)
        {
            /* processes input received from any keyboard-like input system.
             * Accepts input parameter in the form of camera defined ENUM
             * (to abstract it from windowing systems) */
            float displacement = MovementSpeed * deltaTime;
            if (direction == DGE::ICamera::Camera_Movement::FORWARD)
                Position += Front * displacement;
            if (direction == DGE::ICamera::Camera_Movement::BACKWARD)
                Position -= Front * displacement;
            if (direction == DGE::ICamera::Camera_Movement::LEFT)
                Position -= Right * displacement;
            if (direction == DGE::ICamera::Camera_Movement::RIGHT)
                Position += Right * displacement;
        }

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            /* processes input received from a mouse input system. Expects the offset value in
             * both the x and y direction. */
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;
            Yaw   += xoffset;
            Pitch += yoffset;

            /* make sure that when pitch is out of bounds, screen doesn't get flipped */
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            updateCameraVectors();
        }

        void ProcessMouseScroll(float yoffset)
        {
            /* processes input received from a
             * mouse scroll-wheel event. Only
             * requires input on the vertical
             * wheel-axis */
            Zoom -= (float)yoffset;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }

    };
}