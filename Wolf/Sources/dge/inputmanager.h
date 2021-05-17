#pragma once
#include "platform/platform.h"
namespace DGE {
    class InputManager : public DGE::Singleton<InputManager> {
        friend class DGE::Singleton<InputManager>;
        DGE::IPlatform* mPlatform{nullptr};

    private:
        InputManager() {
            if (!mPlatform) {
                if (WindowManager::sWindowDriver == WindowManager::WindowDriver::DRIVER_GLFW3){
                    mPlatform = &PlatformGLFW::GetInstanceReference();
                }
                else {
                    std::exit(-1);
                }
            }
        }

    public:
        using Trigger = std::pair<Keys, EventType>;

        inline DGE::KeyState GetKeyState(DGE::IPlatform::WindowID wid, Keys t) {
            return mPlatform -> GetKeyState(wid,t);
        }

    };


}