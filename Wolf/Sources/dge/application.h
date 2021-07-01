#pragma once

namespace DGE{

/******************************************************************************************************************/
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods

    class Application {
    public:

    protected:
        std::string mDescription{};
        DGE::IPlatform::WindowID mAppWindow;
        DGE::WindowManager& aWindowManagerRef{DGE::WindowManager::GetInstanceReference()};
        DGE::RenderManager& aRenderManagerRef{DGE::RenderManager::GetInstanceReference()};
        DGE::InputManager& anInputManagerRef{DGE::InputManager::GetInstanceReference()};

        bool bInitialized {false};
        bool bRunning{true};
        virtual void Init() = 0; //Initialize Windows, Platform, Rendering etc.
        virtual void Load() = 0; //Load Assets, Create Levels, Menus, Debug Dialogues etc.
        virtual void MainLoop() = 0;  // BeforeStart(), GetInput(), Simulate(), Render(), AfterFinish().
        virtual void ShutDown() = 0;  // Unload Assets, Levels, Dialogues etc.
        virtual const std::string& GetDescription(){ return mDescription; }
        inline virtual void UpdateEvents(){
            glfwPollEvents();
        }
    public:
        void Exec(){
            Init();
            if (!bInitialized) ShutDown();
            Load();
            if (!bInitialized) ShutDown();
            while (bRunning){
                MainLoop();
                Application::UpdateEvents();
                UpdateEvents();
            }
            ShutDown();
        }
    };

}