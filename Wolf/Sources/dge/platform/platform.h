#pragma once
namespace DGE {

    static std::once_flag sPlaftormUniqueInitialization{};
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
        virtual bool IsWindowClosing(WindowID) = 0;
        virtual void CloseWindow(WindowID) = 0;


        virtual DGE::KeyState GetKeyState(WindowID, Keys) = 0;


        virtual void InstallWindowResizeCallback(IPlatform::WindowID wID, void(*)(void*, int, int)) = 0;
    public:
    };

    class PlatformGLFW : public DGE::Singleton<PlatformGLFW>, public IPlatform {
    private:

        inline static GLFWwindow* firstWindowIDPTR{nullptr};
        inline static std::once_flag firstWindowID_flag{};
        inline static std::map<DGE::Keys, int> keyMap_DGE_GLFW{
            std::make_pair(DGE::Keys::K_A, GLFW_KEY_A),
            std::make_pair(DGE::Keys::K_S, GLFW_KEY_S),
            std::make_pair(DGE::Keys::K_D, GLFW_KEY_D),
            std::make_pair(DGE::Keys::K_W, GLFW_KEY_W),
            std::make_pair(DGE::Keys::K_UP, GLFW_KEY_UP),
            std::make_pair(DGE::Keys::K_DWN, GLFW_KEY_DOWN),
            std::make_pair(DGE::Keys::K_LFT, GLFW_KEY_LEFT),
            std::make_pair(DGE::Keys::K_RGT, GLFW_KEY_RIGHT),
            std::make_pair(DGE::Keys::K_SPC, GLFW_KEY_SPACE),
            std::make_pair(DGE::Keys::K_LCTL, GLFW_KEY_LEFT_CONTROL),
            std::make_pair(DGE::Keys::K_RCTL, GLFW_KEY_RIGHT_CONTROL),
            std::make_pair(DGE::Keys::K_LALT, GLFW_KEY_LEFT_ALT),
            std::make_pair(DGE::Keys::K_RALT, GLFW_KEY_RIGHT_ALT),
            std::make_pair(DGE::Keys::K_ESC, GLFW_KEY_ESCAPE)
        };

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
        virtual bool IsWindowClosing(WindowID wID) override {
            return glfwWindowShouldClose((GLFWwindow*) wID);
        }
        virtual void CloseWindow(WindowID wID) override {
            glfwSetWindowShouldClose((GLFWwindow*)wID, GLFW_TRUE);
        }
        virtual DGE::KeyState GetKeyState(WindowID wid, Keys t){
            auto result = glfwGetKey((GLFWwindow*)wid, keyMap_DGE_GLFW[t]);
            return GLFW_PRESS == result ? DGE::KeyState::STATE_DOWN : DGE::KeyState::STATE_UP; 
        }
    };
}