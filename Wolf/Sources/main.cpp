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


#include <wingdi.h>

extern "C"{
#include <glad/glad.h>
}
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>


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

    /***** COLORS *****/
    namespace COLOR{
        [[maybe_unused]]static const fColor3 kGreen{0.203f, 0.921f, 0.658f};
        [[maybe_unused]]static const fColor3 KRed{0.921f, 0.203f, 0.658f};
        [[maybe_unused]]static const fColor3 kBlue{0.203f, 0.203f, 0.921f};
        [[maybe_unused]]static const fColor3 DimGray{0.329412f, 0.329412f, 0.329412f};
        [[maybe_unused]]static const fColor3 DimGrey{0.329412f, 0.329412f, 0.329412f};
        [[maybe_unused]]static const fColor3 Gray{0.752941f, 0.752941f, 0.752941f};
        [[maybe_unused]]static const fColor3 Grey{0.752941f, 0.752941f, 0.752941f};
        [[maybe_unused]]static const fColor3 LightGray{0.658824f, 0.658824f, 0.658824f};
        [[maybe_unused]]static const fColor3 LightGrey{0.658824f, 0.658824f, 0.658824f};
        [[maybe_unused]]static const fColor3 VLightGray{0.80f, 0.80f, 0.80f};
        [[maybe_unused]]static const fColor3 VLightGrey{0.80f, 0.80f, 0.80f};
        [[maybe_unused]]static const fColor3 Aquamarine{0.439216f, 0.858824f, 0.576471f};
        [[maybe_unused]]static const fColor3 BlueViolet{0.62352f, 0.372549f, 0.623529f};
        [[maybe_unused]]static const fColor3 Brown{0.647059f, 0.164706f, 0.164706f};
        [[maybe_unused]]static const fColor3 CadetBlue{0.372549f, 0.623529f, 0.623529f};
        [[maybe_unused]]static const fColor3 Coral{1.0f, 0.498039f, 0.0f};
        [[maybe_unused]]static const fColor3 CornflowerBlue{0.258824f, 0.258824f, 0.435294f};
        [[maybe_unused]]static const fColor3 DarkGreen{0.184314f, 0.309804f, 0.184314f};
        [[maybe_unused]]static const fColor3 DarkOliveGreen{0.309804f, 0.309804f, 0.184314f};
        [[maybe_unused]]static const fColor3 DarkOrchid{0.6f, 0.196078f, 0.8f};
        [[maybe_unused]]static const fColor3 DarkSlateBlue{0.419608f, 0.137255f, 0.556863f};
        [[maybe_unused]]static const fColor3 DarkSlateGray{0.184314f, 0.309804f, 0.309804f};
        [[maybe_unused]]static const fColor3 DarkSlateGrey{0.184314f, 0.309804f, 0.309804f};
        [[maybe_unused]]static const fColor3 DarkTurquoise{0.439216f, 0.576471f, 0.858824f};
        [[maybe_unused]]static const fColor3 Firebrick{0.556863f, 0.137255f, 0.137255f};
        [[maybe_unused]]static const fColor3 ForestGreen{0.137255f, 0.556863f, 0.137255f};
        [[maybe_unused]]static const fColor3 Gold{0.8f, 0.498039f, 0.196078f};
        [[maybe_unused]]static const fColor3 Goldenrod{0.858824f, 0.858824f, 0.439216f};
        [[maybe_unused]]static const fColor3 GreenYellow{0.576471f, 0.858824f, 0.439216f};
        [[maybe_unused]]static const fColor3 IndianRed{0.309804f, 0.184314f, 0.184314f};
        [[maybe_unused]]static const fColor3 Khaki{0.623529f, 0.623529f, 0.372549f};
        [[maybe_unused]]static const fColor3 LightBlue{0.74902f, 0.847059f, 0.847059f};
        [[maybe_unused]]static const fColor3 LightSteelBlue{0.560784f, 0.560784f, 0.737255f};
        [[maybe_unused]]static const fColor3 LimeGreen{0.196078f, 0.8f, 0.196078f};
        [[maybe_unused]]static const fColor3 Maroon{0.556863f, 0.137255f, 0.419608f};
        [[maybe_unused]]static const fColor3 MediumAquamarine{0.196078f, 0.8f, 0.6f};
        [[maybe_unused]]static const fColor3 MediumBlue{0.196078f, 0.196078f, 0.8f};
        [[maybe_unused]]static const fColor3 MediumForestGreen{0.419608f, 0.556863f, 0.137255f};
        [[maybe_unused]]static const fColor3 MediumGoldenrod{0.917647f, 0.917647f, 0.678431f};
        [[maybe_unused]]static const fColor3 MediumOrchid{0.576471f, 0.439216f, 0.858824f};
        [[maybe_unused]]static const fColor3 MediumSeaGreen{0.258824f, 0.435294f, 0.258824f};
        [[maybe_unused]]static const fColor3 MediumSlateBlue{0.498039f, 0.0f, 1.0f};
        [[maybe_unused]]static const fColor3 MediumSpringGreen{0.498039f, 1.0f, 0.0f};
        [[maybe_unused]]static const fColor3 MediumTurquoise{0.439216f, 0.858824f, 0.858824f};
        [[maybe_unused]]static const fColor3 MediumVioletRed{0.858824f, 0.439216f, 0.576471f};
        [[maybe_unused]]static const fColor3 MidnightBlue{0.184314f, 0.184314f, 0.309804f};
        [[maybe_unused]]static const fColor3 Navy{0.137255f, 0.137255f, 0.556863f};
        [[maybe_unused]]static const fColor3 NavyBlue{0.137255f, 0.137255f, 0.556863f};
        [[maybe_unused]]static const fColor3 Orange{1.0f, 0.5f, 0.0f};
        [[maybe_unused]]static const fColor3 OrangeRed{1.0f, 0.25f, 0.0f};
        [[maybe_unused]]static const fColor3 Orchid{0.858824f, 0.439216f, 0.858824f};
        [[maybe_unused]]static const fColor3 PaleGreen{0.560784f, 0.737255f, 0.560784f};
        [[maybe_unused]]static const fColor3 Pink{0.737255f, 0.560784f, 0.560784f};
        [[maybe_unused]]static const fColor3 Plum{0.917647f, 0.678431f, 0.917647f};
        [[maybe_unused]]static const fColor3 Salmon{0.435294f, 0.258824f, 0.258824f};
        [[maybe_unused]]static const fColor3 SeaGreen{0.137255f, 0.556863f, 0.419608f};
        [[maybe_unused]]static const fColor3 Sienna{0.556863f, 0.419608f, 0.137255f};
        [[maybe_unused]]static const fColor3 SkyBlue{0.196078f, 0.6f, 0.8f};
        [[maybe_unused]]static const fColor3 SlateBlue{0.0f, 0.498039f, 1.0f};
        [[maybe_unused]]static const fColor3 SpringGreen{0.0f, 1.0f ,0.498039f};
        [[maybe_unused]]static const fColor3 SteelBlue{0.137255f, 0.419608f, 0.556863f};
        [[maybe_unused]]static const fColor3 Tan{0.858824f, 0.576471f, 0.439216f};
        [[maybe_unused]]static const fColor3 Thistle{0.847059f, 0.74902f, 0.847059f};
        [[maybe_unused]]static const fColor3 Turquoise{0.678431f, 0.917647f, 0.917647f};
        [[maybe_unused]]static const fColor3 Violet{0.309804f, 0.184314f, 0.309804f};
        [[maybe_unused]]static const fColor3 VioletRed{0.8f, 0.196078f, 0.6f};
        [[maybe_unused]]static const fColor3 Wheat{0.847059f, 0.847059f, 0.74902f};
        [[maybe_unused]]static const fColor3 YellowGreen{0.6f, 0.8f, 0.196078f};
        [[maybe_unused]]static const fColor3 SummerSky{0.22f, 0.69f, 0.87f};
        [[maybe_unused]]static const fColor3 RichBlue{0.35f, 0.35f, 0.67f};
        [[maybe_unused]]static const fColor3 Brass{0.71f, 0.65f, 0.26f};
        [[maybe_unused]]static const fColor3 Copper{0.72f, 0.45f, 0.20f};
        [[maybe_unused]]static const fColor3 Bronze{0.55f, 0.47f, 0.14f};
        [[maybe_unused]]static const fColor3 Bronze2{0.65f, 0.49f, 0.24f};
        [[maybe_unused]]static const fColor3 Silver{0.90f, 0.91f, 0.98f};
        [[maybe_unused]]static const fColor3 BrightGold{0.85f, 0.85f, 0.10f};
        [[maybe_unused]]static const fColor3 OldGold{0.81f, 0.71f, 0.23f};
        [[maybe_unused]]static const fColor3 Feldspar{0.82f, 0.57f, 0.46f};
        [[maybe_unused]]static const fColor3 Quartz{0.85f, 0.85f, 0.95f};
        [[maybe_unused]]static const fColor3 NeonPink{1.00f, 0.43f, 0.78f};
        [[maybe_unused]]static const fColor3 DarkPurple{0.53f, 0.12f, 0.47f};
        [[maybe_unused]]static const fColor3 NeonBlue{0.30f, 0.30f, 1.00f};
        [[maybe_unused]]static const fColor3 CoolCopper{0.85f, 0.53f, 0.10f};
        [[maybe_unused]]static const fColor3 MandarinOrange{0.89f, 0.47f, 0.20f};
        [[maybe_unused]]static const fColor3 LightWood{0.91f, 0.76f, 0.65f};
        [[maybe_unused]]static const fColor3 MediumWood{0.65f, 0.50f, 0.39f};
        [[maybe_unused]]static const fColor3 DarkWood{0.52f, 0.37f, 0.26f};
        [[maybe_unused]]static const fColor3 SpicyPink{1.00f, 0.11f, 0.68f};
        [[maybe_unused]]static const fColor3 SemiSweetChoc{0.42f, 0.26f, 0.15f};
        [[maybe_unused]]static const fColor3 BakersChoc{0.36f, 0.20f, 0.09f};
        [[maybe_unused]]static const fColor3 Flesh{0.96f, 0.80f, 0.69f};
        [[maybe_unused]]static const fColor3 NewTan{0.92f, 0.78f, 0.62f};
        [[maybe_unused]]static const fColor3 NewMidnightBlue{0.00f, 0.00f, 0.61f};
        [[maybe_unused]]static const fColor3 VeryDarkBrown{0.35f, 0.16f, 0.14f};
        [[maybe_unused]]static const fColor3 DarkBrown{0.36f, 0.25f, 0.20f};
        [[maybe_unused]]static const fColor3 DarkTan{0.59f, 0.41f, 0.31f};
        [[maybe_unused]]static const fColor3 GreenCopper{0.32f, 0.49f, 0.46f};
        [[maybe_unused]]static const fColor3 DkGreenCopper{0.29f, 0.46f, 0.43f};
        [[maybe_unused]]static const fColor3 DustyRose{0.52f, 0.39f, 0.39f};
        [[maybe_unused]]static const fColor3 HuntersGreen{0.13f, 0.37f, 0.31f};
        [[maybe_unused]]static const fColor3 Scarlet{0.55f, 0.09f, 0.09f};

        [[maybe_unused]]static const fColor3 Med_Purple{0.73f, 0.16f, 0.96f};
        [[maybe_unused]]static const fColor3 Light_Purple{0.87f, 0.58f, 0.98f};
        [[maybe_unused]]static const fColor3 Very_Light_Purple{0.94f, 0.81f, 0.99f};
    }

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
        virtual void SetBackgroundColor(DGE::fColor4 color4) = 0;
    };

    class RenderContextOpenGL : public DGE::Singleton<RenderContextOpenGL>, public IRenderContext {
    public:
        virtual void Init() override {

            glEnable(GL_DEPTH_TEST );
        }
        virtual void Shutdown() override {

        }

        void SetBackgroundColor(DGE::fColor4 color4) override {
            glClearColor(color4.x, color4.y, color4.z, color4.z);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
            mRenderContext->SetBackgroundColor(color4);
        }
        void SetBackgroundColor(DGE::fColor3 color3) {
            mRenderContext->SetBackgroundColor(glm::vec4(color3, 1.0f));
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

        inline static GLFWwindow* firstWindowIDPTR{nullptr};
        inline static std::once_flag firstWindowID_flag{};

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
            auto windowID = (IPlatform::WindowID)ptrWindow;
            SetActiveWindow(windowID);
            glfwSetWindowPos(ptrWindow, x, y);
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
            static bool neh {true};
            glfwMakeContextCurrent((GLFWwindow*)wID);
            if (neh){
                neh = false;
                if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
                    spdlog::critical("Glad opengl function pointers lib, {:s}:{:d} it's fucked.", __FILE__, __LINE__);
                    std::exit(-1);
                } else {
                    spdlog::info("Glad opengl function pointers lib, {:s}:{:d} it's Ok.", __FILE__, __LINE__);
                }

            }
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
            mActiveWindow = windowID;
            if (!windowID) {
                spdlog::critical ("{:s}:{:d} -- WindowManager couldn't Spawn the Window....", __FILE__, __LINE__);
                std::exit(-1);
            }
            mPlatform->InstallWindowResizeCallback(windowID, frameBufferResizeCb);

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

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

        }
    };
    class MeshVertexNormalTextureCube : public Mesh {
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
    public:
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
        std::string mDescription{};

        DGE::WindowManager& aWindowManagerRef{DGE::WindowManager::GetInstanceReference()};
        DGE::RenderManager& aRenderManagerRef{DGE::RenderManager::GetInstanceReference()};

        bool bInitialized {false};
        virtual void Init() = 0; //Initialize Windows, Platform, Rendering etc.
        virtual void Load() = 0; //Load Assets, Create Levels, Menus, Debug Dialogues etc.
        virtual void MainLoop() = 0;  // BeforeStart(), GetInput(), Simulate(), Render(), AfterFinish().
        virtual void ShutDown() = 0;  // Unload Assets, Levels, Dialogues etc.
        virtual const std::string& GetDescription(){ return mDescription; }
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

    DGE::iSize mScreenSize{800,600};
    float mAspectRatio{(float)mScreenSize.x/(float)mScreenSize.y};
    float mFov{60.0f};
    DGE::IPlatform::WindowID mAppWindow;

    unsigned int aCubeMesh{};
    glm::mat4 aBoxModel{glm::mat4{1.0f}};

    std::unique_ptr<Wolf::Renderer::Shader> aShaderPtr{nullptr};

    const DGE::fLocation kLightPosition {3.0f, 3.0f, -3.0f};

    bool bUsePhongShading{true};

    glm::mat4 mProjection = glm::perspective(glm::radians(mFov), mAspectRatio, 0.1f, 100.0f);

protected:

    void Init() override {

        mDescription = std::string {"A Simple Cubes - Camera Navigation example."};
        auto startWindowLocation = DGE::iLocation2(100, 200);
        mAppWindow = aWindowManagerRef.SpawnWindow(startWindowLocation, mScreenSize, "Simple Cubes Application");

        bInitialized = true;
    }
    void Load() override {

        //Create a Scene with a Mesh-Texture-Cube mesh, and a Camera.
        aCubeMesh = DGE::MeshVertexNormalTextureCube().VAO;

        //Create a Shader.
        aShaderPtr = std::make_unique<Wolf::Renderer::Shader>("plain.vs", "plain.fs");
        if (!aShaderPtr){
            bInitialized = false;
            return;
        }
        aShaderPtr->use();
        aShaderPtr->setMat4("projection", mProjection);
        aShaderPtr->setBool("blinn", bUsePhongShading);
        aShaderPtr->setVec3("lightPos", kLightPosition);
        aShaderPtr->setVec3("color", DGE::COLOR::kGreen);

        spdlog::info("{:s}{:d} Finished loding the scene", __FILE__, __LINE__);
    }
    void MainLoop() override {
        //Process
        auto radius = glm::length(mCamera.mPosition);
        while (glfwWindowShouldClose((GLFWwindow*)mAppWindow) == false) {

            if (glfwGetKey((GLFWwindow*)mAppWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose((GLFWwindow*)mAppWindow, true);

            aWindowManagerRef.GrabWindow(mAppWindow);
            // Paint Background Fill Color & Update Window
            aRenderManagerRef.SetBackgroundColor(DGE::COLOR::kBlue);

            //Select Shader to Use and Configure it.
            aShaderPtr->use();
            //UPDATECAMERA()
            auto timeValue = glfwGetTime();
            mCamera.SetPositionAndTarget(
                    glm::vec3{
                        radius*cos(timeValue + glm::pi<float>() / 4.0f),
                        3.0f * sin(timeValue),
                        radius*sin(timeValue + glm::pi<float>() / 4.0f)
                        },
                        glm::vec3{0.0f, 0.0f, 0.0f}
                        );
            aShaderPtr->setVec3("viewPos", mCamera.mPosition);
            aShaderPtr->setMat4("view", mCamera.GetViewMatrix());
            //FOR EACH MODEL UPDATEMODEL()
            glBindVertexArray(aCubeMesh);
            //UPDATE MODEL()
            aShaderPtr->setMat4("model", aBoxModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            aWindowManagerRef.UpdateWindow(mAppWindow);

            glfwPollEvents();
        }

    }
    void ShutDown() override {

        aWindowManagerRef.ShutdownWindow(mAppWindow);
    }

public:

    class Camera {
        friend class MyApplicationCube;
        const glm::vec3 referenceUp{0.0f, 1.0f, 0.0f};
        glm::mat4 mTransform {1.0f};
        DGE::fLocation mPosition {3.0f, 3.0f, 3.0f};
        DGE::fLocation mFront{-mPosition};
        glm::vec3 mRight{};
        glm::vec3 mUp{referenceUp};
    public:

        glm::mat4& GetViewMatrix(){
            return mTransform;
        }

        void Update(){
            mTransform = glm::lookAt(mPosition, mPosition + mFront, mUp);
        }
        void SetPositionAndTarget(glm::vec3 position, glm::vec3 lookat){
            mPosition = position;

            mFront = lookat - position;
            auto camZAxis = -mFront;
            camZAxis = glm::normalize(camZAxis);

            mRight = glm::cross(referenceUp, camZAxis);
            mUp = glm::cross(camZAxis, mRight);
            Update();
        }
        Camera(glm::vec3 position, glm::vec3 lookat){
            SetPositionAndTarget(position, lookat);
        }
        Camera() { Update(); }
    };
    Camera mCamera{};

};

class MyApplicationTriangle : public DGE::Application{

    unsigned int SCR_WIDT{800};
    unsigned int SCR_HGHT{600};
    unsigned int aTriangleMesh{};
    std::unique_ptr<Wolf::Renderer::Shader> aShaderPtr{nullptr};
    DGE::IPlatform::WindowID windowID_0;
    DGE::IPlatform::WindowID windowID_1;
    
    GLFWwindow*window_0;
    GLFWwindow*window_1;


protected:
    void Init() override{

        mDescription = std::string {"This Application tries to render two triangles, in separate windows. It's not working."};

        auto startWindowLocation_0 = DGE::iLocation2(100, 200);
        auto startWindowLocation_1 = DGE::iLocation2(100+SCR_WIDT, 200);
        auto windowSize_0 = DGE::iSize(SCR_WIDT, SCR_HGHT);
        auto windowSize_1 = DGE::iSize(SCR_WIDT, SCR_HGHT);
        
        windowID_0 = aWindowManagerRef.SpawnWindow(startWindowLocation_0, windowSize_0, "My Triangle Window 0");
        windowID_1 = aWindowManagerRef.SpawnWindow(startWindowLocation_1, windowSize_1, "My Triangle Window 1");
        window_0 = (GLFWwindow*)windowID_0;
        window_1 = (GLFWwindow*)windowID_1;
        aWindowManagerRef.GrabWindow(windowID_0);
        aWindowManagerRef.GrabWindow(windowID_1);
        bInitialized = true;
    }
    void Load() override {

        //Create a Scene with a Triangle and a Camera.
        //Create a Triangle Mesh.
        aTriangleMesh = DGE::MeshVertexNormalTextureTriangle().VAO;
        //Create a Shader.
        aShaderPtr = std::make_unique<Wolf::Renderer::Shader>("plane.vs", "plane.fs");
        if (!aShaderPtr){
            bInitialized = false;
            return;
        }

    }
    void MainLoop() override {
        //Process
        while (!glfwWindowShouldClose(window_0))
        {
            if (glfwGetKey(window_0, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window_0, true);

            //Draw the Window 0
            aWindowManagerRef.GrabWindow(windowID_0);
            //Clear the Background
            aRenderManagerRef.SetBackgroundColor(DGE::fColor4(73.0f / 255.0f, 139.0f / 255.0f, 245.0f / 255.0f, 1.0f));

            //Activate the Shader.
            aShaderPtr->use();
            glBindVertexArray(aTriangleMesh);

            auto timeValue = glfwGetTime();

            aShaderPtr->setVec3("color", DGE::fColor3 {DGE::COLOR::kGreen.r, DGE::COLOR::kGreen.g * sin(timeValue) / 2.0f + 0.5f, DGE::COLOR::kGreen.b});

            //Flush to Screen.
            glDrawArrays(GL_TRIANGLES, 0, 3);
            aWindowManagerRef.UpdateWindow(windowID_0);
            aWindowManagerRef.GrabWindow(windowID_1);
            //Clear the Background
            aRenderManagerRef.SetBackgroundColor(DGE::fColor4(73.0f / 255.0f, 245.0f / 255.0f, 139.0f / 255.0f, 1.0f));
            aShaderPtr->use();
            glBindVertexArray(aTriangleMesh);
            aShaderPtr->setVec3("color", DGE::COLOR::KRed);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            aWindowManagerRef.UpdateWindow(windowID_1);
            glfwPollEvents();
        }

    }
    void ShutDown() override {
        aWindowManagerRef.ShutdownWindow(windowID_0);
    }

};


int main ([[maybe_unused]] int argc, [[maybe_unused]] const char **argv){

    auto myApplication = MyApplicationCube();
    myApplication.Exec();

    //Shutdown

    auto& platform = DGE::PlatformGLFW::GetInstanceReference();
    platform.ShutDown();
    std::cout << "-- FINISHED OK --" << std::endl;
    return 0;
}
