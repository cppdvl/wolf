#include <map>
#include <mutex>
#include <vector>
#include <utility>
#include <iostream>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern "C"{
#include <glad/glad.h>
}
#include <GLFW/glfw3.h>


namespace DGE{
    /******************************************************************************************************************/
    template <typename T>
    class Singleton{
    inline static T* mt{nullptr};
    public:
        inline static T& GetInstanceReference()
        {
            if (!mt){
                mt = new T();
            }
            //static T t = T{};
            return *mt;
        }
    };

    using iSize = glm::ivec2;
    using iLocation = glm::ivec2;
    using iViewport = std::pair<iLocation, iSize>;
    using fColor4 = glm::vec4;
    /******************************************************************************************************************/
    /* -- Rendering Manager -- ************************************************************************************** */
    static std::once_flag sRenderCtxtUniqueInitialization{};
    /*
     * Rendering Context -- OpenGl.
     *
     * The rendering context is explicitly Vulkan Or OpenGl Or Metal etc.
     * This context IS NOT visible by the Application. It's the Render Manager the one who's aware of it.
     *
     */
    class IRenderContext {
    protected:
        inline static bool bIsStarted{false};
    public:

        virtual void Init() = 0;
        virtual void Shutdown() = 0;
    };

    class RenderContextOpenGL : public DGE::Singleton<RenderContextOpenGL>, public IRenderContext {
    public:
        virtual void Init() override {
            std::call_once(DGE::sRenderCtxtUniqueInitialization, [](){
                if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
                    spdlog::critical("Glad opengl function pointers lib, {:s}:{:d} it's fucked.", __FILE__, __LINE__);
                    std::exit(-1);
                }
                bIsStarted = true;
            });
        }
        virtual void Shutdown() override {

        }
    };
    class WindowManager;
    class RenderManager : public DGE::Singleton<RenderManager>{
        friend class DGE::Singleton<RenderManager>;
        friend class DGE::WindowManager;
    public:
        enum class RenderDriver{
            DRIVER_OPENGL,
            DRIVER_VULKAN
        };
        inline static RenderDriver sRenderDriver{RenderDriver::DRIVER_OPENGL};
    private:
        IRenderContext* mRenderContext {nullptr};
        RenderManager()
        {
            if (!mRenderContext){

                if (sRenderDriver == RenderDriver::DRIVER_OPENGL){

                    spdlog::info("{:s}:{:d} -- RenderManager::RenderManager(), driver = RenderDriver::DRIVER_OPENGL ", __FILE__, __LINE__);
                    mRenderContext = &RenderContextOpenGL::GetInstanceReference();
                }
            }
            else {

                spdlog::critical("{:s}:{:d} -- RenderManager::RenderManager() driver not found... ", __FILE__, __LINE__);
                std::exit(-1);
            }
        }
        void _SpawnRenderContext(){
            mRenderContext->Init();
        }
    public:
        void SetBackgroundColor(DGE::fColor4 color4){
            // TODO: MOVE THE SPECIFIC FUNCTIONALITY OF THIS FUNCTION TO DGE::RenderContextOpenGL!!!!!!!!!!!
            glClearColor(color4.x, color4.y, color4.z, color4.z);
            glClear(GL_COLOR_BUFFER_BIT);
        }

    };

    /******************************************************************************************************************/
    /* -- Windows Manager -- *************************************************************************************** */
    static std::once_flag sPlaftormUniqueInitialization{};
    static void sWindowResizeCallback(void*, int w, int h){
        glViewport(0, 0, w, h);
    }

/*
 * Platform -- Exposed Interface.
 *
 * This is the exposed interface to:
 *  WindowManager,
 *  AudioManager,
 *  InputController Manager,
 *  Sys/OS/I/O Manager.
 *
 * Window Manager: will ask the Platform Interface for things such as windows, the resolution of those etc.
 * --------------
 * The Audio Manager: will ask for sound resources to the Platform Interface.
 * -----------------
 * The InputController Manager: will ask the Platform interface, for the controllers the inputs from the
 * ---------------------------
 * user (Mouse, Joystick, Keyboard).
 *
 * The Sys/OS/I/O/Manager: will ask for resources such as files, sockets, pipes, threads etc.
 * ----------------------
 */
    class IPlatform {
    protected:
        inline static bool bIsStarted{false};
    public:
        using WindowID = uint64_t;
        virtual void Init() = 0;
        virtual void ShutDown() = 0;
        virtual WindowID CreatePlatformWindow(int x, int y, int width, int height, const char* windowTitle) = 0;
        virtual WindowID DestroyPlatformWindow(WindowID) = 0;
        virtual void SetActiveWindow(WindowID) = 0;
        virtual void RefreshWindow(WindowID) = 0;


        virtual void InstallWindowResizeCallback(IPlatform::WindowID wID, void(*)(void*, int, int)) = 0;
    public:
    };
    class PlatformGLFW : public DGE::Singleton<PlatformGLFW>, public IPlatform {
    private:
    public:
        virtual void Init() override {
            std::call_once(sPlaftormUniqueInitialization, []()-> void {
                IPlatform::bIsStarted = true;
                spdlog::info("{:s}:{:d} -- PlatformGLFW::Init()", __FILE__, __LINE__);
                glfwInit();
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            });
        }
        virtual void ShutDown() override {
            spdlog::info("{:s}:{:d} -- PlatformGLFW::ShutDown()", __FILE__, __LINE__);
            glfwTerminate();
        }
        virtual WindowID CreatePlatformWindow(int x, int y, int width, int height, const char* windowTitle) override {
            if (IPlatform::bIsStarted == false) Init();
            auto ptrWindow = glfwCreateWindow(width, height, windowTitle, nullptr, nullptr);
            if (!ptrWindow){
                spdlog::critical("{:s}:{:d} -- PlatformGLFW::CreatePlatformWindow() glfwCreateWindow failed", __FILE__, __LINE__);
                return 0;
            }

            glfwSetWindowPos(ptrWindow, x, y);
            auto windowID = (IPlatform::WindowID)ptrWindow;
            spdlog::info("{:s}:{:d} -- PlatformGLFW::CreatePlatformWindow() Window Title and MemoryID => '{:s}' : [@0x{:x}]",  __FILE__, __LINE__, windowTitle, (IPlatform::WindowID)ptrWindow);
            return windowID;
        }
        virtual WindowID DestroyPlatformWindow(WindowID wID) override {
            auto pGLFWwindow = reinterpret_cast<GLFWwindow*>((void*)wID);
            glfwDestroyWindow(pGLFWwindow);
            return wID;
        }
        virtual void InstallWindowResizeCallback(IPlatform::WindowID wID, void(*windowResizeCb)(void*, int, int)) override {
            glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>((void*)wID), (GLFWframebuffersizefun)windowResizeCb);
        }
        virtual void SetActiveWindow(WindowID wID) override {
            auto ptrWindow = reinterpret_cast<GLFWwindow*>((void*)wID);
            glfwMakeContextCurrent(ptrWindow);
        }
        virtual void RefreshWindow(WindowID wID) override {
            glfwSwapBuffers((GLFWwindow*)wID);
        }

    };



/*
 *  WindowManager -- Anything with the suffix manager is exposed to the application. And May Use more sophisticated Expressions.
 */
    static std::vector<IPlatform::WindowID> sWindowIDs{};
    static std::map<IPlatform::WindowID, std::vector<DGE::iViewport>> smWindowID_vViewports{};

    class WindowManager : public DGE::Singleton<WindowManager>{
        friend class DGE::Singleton<WindowManager>;
        friend class DGE::RenderManager;
    public:
        enum class WindowDriver
        {
            DRIVER_SDL2,
            DRIVER_GLFW3
        };
        inline static WindowDriver sWindowDriver{WindowDriver::DRIVER_GLFW3};

    private:
        IPlatform::WindowID mActiveWindow{};
        IPlatform* mPlatform{nullptr};

        WindowManager(){
            if (!mPlatform){
                if (sWindowDriver == WindowDriver::DRIVER_GLFW3){
                    spdlog::info("{:s}:{:d} -- WindowManager(), driver = WindowDriver::DRIVER_GLFW3 ", __FILE__, __LINE__);
                    mPlatform = &PlatformGLFW::GetInstanceReference();
                }
                else {
                    spdlog::critical("{:s}:{:d} -- WindowManager driver not found... ", __FILE__, __LINE__);
                    std::exit(-1);
                }
            }
        }

    public:
        IPlatform::WindowID SpawnWindow(DGE::iLocation& windowLocation, DGE::iSize& windowSize, std::string windowTitle, void(*frameBufferResizeCb)(void*, int, int) = DGE::sWindowResizeCallback){

            auto windowID = mPlatform->CreatePlatformWindow(windowLocation.x, windowLocation.y, windowSize.x, windowSize.y, windowTitle.c_str());
            if (!windowID) {
                spdlog::critical ("{:s}:{:d} -- WindowManager couldn't Spawn the Window....", __FILE__, __LINE__);
                std::exit(-1);
            }
            mPlatform->InstallWindowResizeCallback(windowID, frameBufferResizeCb);
            mPlatform->SetActiveWindow(windowID);
            mActiveWindow = windowID;

            auto renderManager = DGE::RenderManager::GetInstanceReference();
            renderManager._SpawnRenderContext();
            sWindowIDs.emplace_back(windowID);
            return windowID;
        }

        IPlatform::WindowID ShutdownWindow(IPlatform::WindowID& windowID){

            if (!windowID) return 0;
            auto windowIDDestroyed = mPlatform->DestroyPlatformWindow(windowID);
            spdlog::info("{:s}:{:d} -- WindowManager::ShutdownWindow() [@0x{:x}]", __FILE__, __LINE__, windowIDDestroyed);

            for(auto windowIDIt = sWindowIDs.begin(); windowIDIt != sWindowIDs.end();)
            {
                if (*windowIDIt != windowID) ++windowIDIt;
                else sWindowIDs.erase(windowIDIt);
            }

            return windowIDDestroyed;
        }

        void GrabWindow(IPlatform::WindowID& windowID){

            mPlatform->SetActiveWindow(windowID);
            mActiveWindow = windowID;
        }

        void UpdateWindow(IPlatform::WindowID& windowID){

            mPlatform->RefreshWindow(windowID);
        }
    };

    /******************************************************************************************************************/

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods



    /******************************************************************************************************************/
    /** -- Camera Mouse/Keyboard -- **********************************************************************************/
    /* An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL out of keyboard and mouse movement.
     */
    const float YAW         = -90.0f;
    const float PITCH       =  0.0f;
    const float ZOOM        =  45.0f;
    class Camera
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
        float Yaw;
        float Pitch;
        float Zoom;

        Camera(
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
            float yaw = DGE::YAW, 
            float pitch = DGE::PITCH) 
        : 
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        Zoom(ZOOM)
        {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }
        Camera(
            float posX, 
            float posY, 
            float posZ, 
            float upX, 
            float upY, 
            float upZ, 
            float yaw, 
            float pitch)
        : 
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        Zoom(ZOOM)
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }
        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
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
    class Camera_MouseKeyboard : public Camera
    {
    public:
        float MovementSpeed{ DGE::MKBSPEED };
        float MouseSensitivity{ DGE::MSENSITIVITY };
        
        Camera_MouseKeyboard(
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
            float yaw = YAW, 
            float pitch = PITCH)
        : 
        Camera(position, up, yaw, pitch)
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
        Camera(posX, posY, posZ, upX, upY, upZ, yaw, pitch)
        {

        }

        void ProcessKeyboard(DGE::Camera::Camera_Movement direction, float deltaTime)
        {
            /* processes input received from any keyboard-like input system. 
             * Accepts input parameter in the form of camera defined ENUM 
             * (to abstract it from windowing systems) */
            float displacement = MovementSpeed * deltaTime;
            if (direction == DGE::Camera::Camera_Movement::FORWARD)
                Position += Front * displacement;
            if (direction == DGE::Camera::Camera_Movement::BACKWARD)
                Position -= Front * displacement;
            if (direction == DGE::Camera::Camera_Movement::LEFT)
                Position -= Right * displacement;
            if (direction == DGE::Camera::Camera_Movement::RIGHT)
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

    class MeshTextureCube {
        std::vector<float> mVertexData {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };
        unsigned int VBO;
    public:
        unsigned int VAO;
        MeshTextureCube(){
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mVertexData.size(), mVertexData.data() , GL_STATIC_DRAW);
            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // texture coord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
    };
}


int main ([[maybe_unused]] int argc, [[maybe_unused]] const char **argv){

    //Start the window Manager
    auto windowManager = DGE::WindowManager::GetInstanceReference();
    auto renderManager = DGE::RenderManager::GetInstanceReference();
    //Configure Windows
    auto startWindowLocation_0 = DGE::iLocation(100, 200);
    auto startWindowLocation_1 = DGE::iLocation(200, 100);
    auto windowSize_0 = DGE::iSize(640, 480);
    auto windowSize_1 = DGE::iSize(480, 640);
    auto windowID_0 = windowManager.SpawnWindow(startWindowLocation_0, windowSize_0, "I had the time of my life");
    auto windowID_1 = windowManager.SpawnWindow(startWindowLocation_1, windowSize_1, "The time of my life, I had");

    //Process
    auto mWindow_1 = reinterpret_cast<GLFWwindow*>((void*)windowID_1);
    while (glfwWindowShouldClose(mWindow_1) == false) {
        if (glfwGetKey(mWindow_1, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow_1, true);

        windowManager.GrabWindow(windowID_1);{
            // Paint Background Fill Color & Update Window
            renderManager.SetBackgroundColor(DGE::fColor4(0.25f, 0.25f, 0.25f, 1.0f));
            windowManager.UpdateWindow(windowID_1);
        }
        windowManager.GrabWindow(windowID_0);{
            renderManager.SetBackgroundColor(DGE::fColor4(73.0f/255.0f, 139.0f/255.0f, 245.0f/255.0f, 1.0f));
            windowManager.UpdateWindow(windowID_0);
        }

        glfwPollEvents();
    }

    //Shutdown
    windowManager.ShutdownWindow(windowID_0);
    windowManager.ShutdownWindow(windowID_1);

    auto& platform = DGE::PlatformGLFW::GetInstanceReference();
    platform.ShutDown();
    std::cout << "-- FINISHED OK --" << std::endl;
    return 0;
}
