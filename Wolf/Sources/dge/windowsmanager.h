namespace DGE {



    


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
}
