//
// Created by JG -- CPP, DEVIL on 6/27/2021.
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

class CubeApplication : public DGE::Application
{

    DGE::iSize mScreenSize{800, 600};
    float mAspectRatio{(float)mScreenSize.x/(float)mScreenSize.y};
    float mFov{60.0f};

    unsigned int aCubeMesh{};
    glm::mat4 aBoxModel{glm::mat4{1.0f}};

    std::unique_ptr<Wolf::Renderer::Shader> aShaderPtr{nullptr};

    const DGE::fLocation kLightPosition {3.0f, 3.0f, -3.0f};

    bool bUsePhongShading{true};

    glm::mat4 mProjection = glm::perspective(glm::radians(mFov), mAspectRatio, 0.1f, 100.0f);

    float mRadius {0.0f};
    GTech::Signal<> OnEscapePressed;

protected:

    void FinishAppExecution();

    void UpdateEvents() override;
    void Init() override;
    void Load() override;
    void MainLoop() override;
    void ShutDown() override;

    DGE::fLocation mCameraLocation{3.0f, 3.0f, 3.0f};
    DGE::fLocation mTargetLocation{0.0f, 0.0f, 0.0f};

    DGE::Camera mCamera{mCameraLocation, mTargetLocation};
    glm::mat4& mCameraTrans{mCamera.GetViewMatrix() };

};


