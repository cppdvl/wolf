//
// Created by User on 6/27/2021.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <wolf/renderer/shader.hpp>
#include <signalslot.h>

#include "../singleton.h"
#include "../dgetypes.h"
#include "../colors.h"
#include "../rendermanager.h"
#include "../windowsmanager.h"
#include "../camera.h"
#include "../mesh.h"
#include "../platform/platform.h"
#include "../inputmanager.h"
#include "../application.h"

constexpr float ScreenWidthf = 800.0f;
constexpr float ScreenHeightf = 600.0f;
constexpr int ScreenWidth = 800;
constexpr int ScreenHeight = 600;

class FPSDemoApplication : public DGE::Application{


    inline static DGE::iSize mScreenSize{ScreenWidth, ScreenHeight};
    inline static float mAspectRatio { ScreenWidthf / ScreenHeightf };


    float mFov {60.0f};

    unsigned int aCubeMesh{};
    glm::mat4 aBoxModel{glm::mat4 {1.0f}};


    std::unique_ptr<Wolf::Renderer::Shader> spShader{nullptr};
    DGE::fLocation lightPosition {3.0f, 3.0f, -3.0f};

    bool bUsePhongShading{true};

    glm::mat4 mProjection = glm::perspective(glm::radians(mFov), mAspectRatio, 0.1f, 100.0f);


    GTech::Signal<> EscapePressed;
    GTech::Signal<> LeftKeyPressed;
    GTech::Signal<> RightKeyPressed;
    GTech::Signal<> UpKeyPressed;
    GTech::Signal<> DownKeyPressed;
    GTech::Signal<> AKeyPressed;
    GTech::Signal<> DKeyPressed;
    GTech::Signal<> WKeyPressed;
    GTech::Signal<> SKeyPressed;
    GTech::Signal<> SpaceKeyPressed;


    GTech::Signal<> TurnLeft_Pressed;
    GTech::Signal<> TurnRight_Pressed;
    GTech::Signal<> AdvanceForwards_Pressed;
    GTech::Signal<> AdvanceBackwards_Pressed;
    GTech::Signal<> FireAction_Pressed;
    GTech::Signal<> TurnLeft_Released;
    GTech::Signal<> TurnRight_Released;
    GTech::Signal<> AdvanceForwards_Released;
    GTech::Signal<> AdvanceBackwards_Released;
    GTech::Signal<> FireAction_Released;



protected:

    void OnEscapeSequence();
    void OnTurnLeftPressed();
    void OnTurnLeftReleased();
    void OnTurnRightPressed();
    void OnTurnRightReleased();
    void OnGoForwardPressed();
    void OnGoForwardReleased();
    void OnGoBackwardPressed();
    void OnGoBackwardReleased();
    void OnFirePressed();
    void OnFireReleased();

    virtual void UpdateEvents() override;
    virtual void Init() override;
    virtual void Load() override;
    virtual void MainLoop() override;
    virtual void ShutDown() override;

    void FPSKinematic(float timeaccel = 1.0f);



    DGE::fLocation mCameraPosition{3.0f, 3.0f, 3.0f};
    DGE::fLocation mCameraTargetLocation{0.0f, 0.0f, 0.0f};
    DGE::Camera mCamera{mCameraPosition, mCameraTargetLocation};
    glm::mat4& mCameraTrans{mCamera.GetViewMatrix()};


};


