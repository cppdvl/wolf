#pragma once
namespace DGE {
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
    
}
