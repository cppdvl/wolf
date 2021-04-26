#include <map>
#include <mutex>
#include <vector>
#include <utility>
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>

#include <wolf/renderer/shader.hpp>


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
    using iLocation2 = glm::ivec2;
    using iViewport = std::pair<iLocation2, iSize>;
    using fColor4 = glm::vec4;
    using fColor3 = glm::vec3;
    using fLocation = glm::vec3;
    using xTextureID = unsigned int;
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
            glEnable(GL_DEPTH_TEST);
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
        void SetBackgroundColor(DGE::fColor3 color4){
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
        IPlatform::WindowID SpawnWindow(DGE::iLocation2& windowLocation, DGE::iSize& windowSize, std::string windowTitle, void(*frameBufferResizeCb)(void*, int, int) = DGE::sWindowResizeCallback){

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
    class Mesh {
    protected:
        unsigned int VBO;
    public:
        unsigned int VAO;
    };
    class MeshVertexNormalTextureTriangle : public Mesh {
        std::vector <float> mMeshData {
            -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
             0.0f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f
        };
    public:
        MeshVertexNormalTextureTriangle(){
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mMeshData.size(), mMeshData.data(), GL_STATIC_DRAW);

            //position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // normal attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            // texture coord attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }
    };
    class MeshVertexNormalTextureCube {
        std::vector<float> mMeshData {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
        };
        unsigned int VBO;
    public:
        unsigned int VAO;
        MeshVertexNormalTextureCube(){

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mMeshData.size(), mMeshData.data() , GL_STATIC_DRAW);
            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // normal attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            // texture coord attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }
    };

    class Texture{
    public:
        DGE::xTextureID textureId{};
        int mWidth, mHeight, mChannels;
        Texture(char* texturePath){
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            //Wrapping Parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            //Filtering Parametes
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            auto pData = stbi_load(texturePath, &mWidth, &mHeight, &mChannels, 0);
            if (!pData){
                spdlog::critical("{:s}:{:d} -- Texture::Texture() stbi_load failed", __FILE__, __LINE__);
                std::exit(-1);
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(pData);

        }
    };

    class Application {
    public:

    protected:
        DGE::WindowManager& aWindowManagerRef{DGE::WindowManager::GetInstanceReference()};
        DGE::RenderManager& aRenderManagerRef{DGE::RenderManager::GetInstanceReference()};

        bool bInitialized {false};
        virtual void Init() = 0; //Initialize Windows, Platform, Rendering etc.
        virtual void Load() = 0; //Load Assets, Create Levels, Menus, Debug Dialogues etc.
        virtual void MainLoop() = 0;  // BeforeStart(), GetInput(), Simulate(), Render(), AfterFinish().
        virtual void ShutDown() = 0;  // Unload Assets, Levels, Dialogues etc.
    public:
        void Exec(){
            Init();
            if (!bInitialized) ShutDown();
            Load();
            if (!bInitialized) ShutDown();
            MainLoop();
            ShutDown();
        }
    };

}

class MyApplicationCube : public DGE::Application {

    unsigned int aCubeMesh{};
    DGE::ICamera aCamera{};
    Wolf::Renderer::Shader* aShaderPtr{nullptr};
    const DGE::fColor3 kGreen{0.203f, 0.921f, 0.658f};
    const DGE::fLocation kLightPosition {3.0f, 3.0f, 3.0f};
    bool bUsePhongShading {true};
    glm::mat4 aBoxModel {glm::mat4(1.0f)};
    DGE::IPlatform::WindowID windowID_0, windowID_1;
protected:
    void Init() override {

        auto startWindowLocation_0 = DGE::iLocation2(100, 200);
        auto startWindowLocation_1 = DGE::iLocation2(200, 100);
        auto windowSize_0 = DGE::iSize(640, 480);
        auto windowSize_1 = DGE::iSize(480, 640);
        windowID_0 = aWindowManagerRef.SpawnWindow(startWindowLocation_0, windowSize_0, "I had the time of my life");
        windowID_1 = aWindowManagerRef.SpawnWindow(startWindowLocation_1, windowSize_1, "The time of my life, I had");
        bInitialized = true;

    }
    void Load() override {

        //Create a Scene with a Mesh-Texture-Cube mesh, and a Camera.
        aCubeMesh = DGE::MeshVertexNormalTextureCube().VAO;
        aCamera = static_cast<DGE::ICamera>(DGE::Camera_MouseKeyboard(DGE::fLocation(2.0f, 2.0f, 3.0f)));

        //Create a Shader.
        aShaderPtr = Wolf::Renderer::Shader::GetShaderByID(Wolf::Renderer::Shader("plain.vs", "plain.fs").ID);
        if (!aShaderPtr){
            bInitialized = false;
            return;
        }
        aShaderPtr->setBool("blinn", bUsePhongShading);
        aShaderPtr->setVec3("lightPos", kLightPosition);
        aShaderPtr->setVec3("color", kGreen);


    }
    void MainLoop() override {
        //Process
        auto mWindow_1 = reinterpret_cast<GLFWwindow*>((void*)windowID_1);
        while (glfwWindowShouldClose(mWindow_1) == false) {
            if (glfwGetKey(mWindow_1, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(mWindow_1, true);

            aWindowManagerRef.GrabWindow(windowID_1);{
                // Paint Background Fill Color & Update Window
                aRenderManagerRef.SetBackgroundColor(DGE::fColor4(0.25f, 0.25f, 0.25f, 1.0f));
                aWindowManagerRef.UpdateWindow(windowID_1);
            }
            aWindowManagerRef.GrabWindow(windowID_0);{
                aRenderManagerRef.SetBackgroundColor(DGE::fColor4(73.0f / 255.0f, 139.0f / 255.0f, 245.0f / 255.0f, 1.0f));
                //Select Shader to Use and Configure it.
                aShaderPtr->use();
                {
                    //Execute Vertex Shader
                    {
                        //UPDATECAMERA()
                        aShaderPtr->setMat4("projection", aCamera.GetProjectionMatrix());
                        aShaderPtr->setMat4("view", aCamera.GetViewMatrix());
                    }
                    {
                        //FOR EACH MODEL UPDATEMODEL()
                        glBindVertexArray(aCubeMesh);
                        //UPDATE MODEL()
                        aShaderPtr->setMat4("model", aBoxModel);
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                    }
                }
                aShaderPtr->setVec3("viewPos", aCamera.Position);
                aWindowManagerRef.UpdateWindow(windowID_0);
            }
            glfwPollEvents();
        }

    }
    void ShutDown() override {
        aWindowManagerRef.ShutdownWindow(windowID_0);
        aWindowManagerRef.ShutdownWindow(windowID_1);
    }



};

class MyAppplcationTriangle : public DGE::Application{

    unsigned int SCR_WIDT{800};
    unsigned int SCR_HGHT{600};
    unsigned int aTriangleMesh{};
    const DGE::fLocation kViewerPoistion {0.0f, 0.0f, 3.0f};
    Wolf::Renderer::Shader* aShaderPtr{nullptr};
    const DGE::fColor3 kGreen{0.203f, 0.921f, 0.658f};
    const DGE::fLocation kLightPosition {3.0f, 3.0f, 3.0f};
    bool bUsePhongShading {false};
    DGE::IPlatform::WindowID windowID_0;

    GLFWwindow*window;
    unsigned int shaderProgram;
    unsigned int VAO;
protected:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }
    void Init() override{

        /*auto startWindowLocation_0 = DGE::iLocation2(100, 200);
        auto windowSize_0 = DGE::iSize(SCR_WIDT, SCR_HGHT);
        windowID_0 = aWindowManagerRef.SpawnWindow(startWindowLocation_0, windowSize_0, "My Triangle Window 0");*/
        bInitialized = true;
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // glfw window creation
        // --------------------
        window = glfwCreateWindow(SCR_WIDT, SCR_HGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            bInitialized = false;
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, MyAppplcationTriangle::framebuffer_size_callback);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            bInitialized = false;
            return;
        }


    }
    void Load() override {
        //Create a Scene with a Mesh-Texture-Cube mesh, and a Camera.
        //aTriangleMesh = DGE::MeshVertexNormalTextureCube().VAO;
        aShaderPtr = Wolf::Renderer::Shader::GetShaderByID(Wolf::Renderer::Shader("plane.vs", "plane.fs").ID);
        if (!aShaderPtr){
            bInitialized = false;
            return;
        }
        aShaderPtr->use();
        aShaderPtr->setVec3("color", kGreen);

        //Create a Shader.
        /*const char *vertexShaderSource = "#version 330 core\n"
                                         "layout (location = 0) in vec3 aPos;\n"
                                         "void main()\n"
                                         "{\n"
                                         "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                         "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
                                           "out vec4 FragColor;\n"
                                           "void main()\n"
                                           "{\n"
                                           "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                           "}\n\0";
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);*/

        float vertices[] ={
                -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
                0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                0.0f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f
        };
        /*float vertices[] = {
                -0.5f, -0.5f, 0.0f, // left
                0.5f, -0.5f, 0.0f, // right
                0.0f,  0.5f, 0.0f  // top
        };*/

        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)3);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)6);
        glEnableVertexAttribArray(2);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

    }
    void MainLoop() override {
        //Process
        //aWindowManagerRef.GrabWindow(windowID_0);
        /*while (glfwWindowShouldClose((GLFWwindow*)windowID_0) == false) {
            if (glfwGetKey((GLFWwindow*)windowID_0, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose((GLFWwindow*)windowID_0, true);
            }

            {
                //aRenderManagerRef.SetBackgroundColor(DGE::fColor4(73.0f / 255.0f, 139.0f / 255.0f, 245.0f / 255.0f, 1.0f));
                //Select Shader to Use and Configure it.
                aShaderPtr->use();
                {
                        //FOR EACH MODEL UPDATEMODEL()
                        glBindVertexArray(aTriangleMesh);
                        glDrawArrays(GL_TRIANGLES, 0, 3);
                }
                aWindowManagerRef.UpdateWindow(windowID_0);
            }
            glfwPollEvents();
        }*/
        while (!glfwWindowShouldClose(window))
        {
            // input
            // -----
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

            // render
            // ------
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw our first triangle
            //glUseProgram(shaderProgram);
            aShaderPtr->use();
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            glDrawArrays(GL_TRIANGLES, 0, 3);
            // glBindVertexArray(0); // no need to unbind it every time

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }
    void ShutDown() override {
        aWindowManagerRef.ShutdownWindow(windowID_0);
    }

};


int main ([[maybe_unused]] int argc, [[maybe_unused]] const char **argv){

    auto myApplication = MyAppplcationTriangle();
    myApplication.Exec();

    //Shutdown

    auto& platform = DGE::PlatformGLFW::GetInstanceReference();
    platform.ShutDown();
    std::cout << "-- FINISHED OK --" << std::endl;
    return 0;
}
