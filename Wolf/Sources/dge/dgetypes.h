#pragma once
namespace DGE {

    using iSize = glm::ivec2;
    using iLocation2 = glm::ivec2;
    using iViewport = std::pair<iLocation2, iSize>;
    using fColor4 = glm::vec4;
    using fColor3 = glm::vec3;
    using fLocation = glm::vec3;
    using xTextureID = unsigned int;
    




    enum class EventType{
        EVENT_PRESSED,
        EVENT_RELEASED
    };
    enum class KeyState{
        STATE_DOWN,
        STATE_UP,
        STAKE_UNKNOWN
    };

    enum class Keys{
        K_A,
        K_S,
        K_D,
        K_W,
        K_UP,
        K_DWN,
        K_LFT,
        K_RGT,
        K_SPC,
        K_LCTL,
        K_RCTL,
        K_LALT,
        K_RALT,
        K_ESC
    };

}
