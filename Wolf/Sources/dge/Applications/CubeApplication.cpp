//
// Created by User on 6/27/2021.
//

extern "C"{
#include <glad/glad.h>
}
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

#include "CubeApplication.h"



void CubeApplication::FinishAppExecution()
{

    bRunning = false;
    aWindowManagerRef.CloseWindow(mAppWindow);

}

void CubeApplication::UpdateEvents()
{

    if (anInputManagerRef.GetKeyState(mAppWindow, DGE::Keys::K_ESC) == DGE::KeyState::STATE_DOWN)
    {
        OnEscapePressed.emit();
    }

}

void CubeApplication::Init()
{

    mDescription = std::string {"A Simple Cubes - Camera Navigation Example."};
    auto startWindowLocation = DGE::iLocation2 {100,200};
    mAppWindow = aWindowManagerRef.SpawnWindow(startWindowLocation, mScreenSize, mDescription);
    bInitialized = true;

}

void CubeApplication::Load()
{

    //Create a Scene with a Mesh-Texture-Cube mesh, and a Camera.
    aCubeMesh = DGE::MeshVertexNormalTextureCube().VAO;




    //Create Shader.
    aShaderPtr = std::make_unique<Wolf::Renderer::Shader>("plain.vs", "plain.fs");
    if (!aShaderPtr)
    {
        bInitialized = false;
        return;
    }

    aShaderPtr->use();
    aShaderPtr->setMat4("projection", mProjection);
    aShaderPtr->setBool("blinn", bUsePhongShading);
    aShaderPtr->setVec3("lightPos", kLightPosition);

    spdlog::info("{:s}{:d} Finished loding the scene", __FILE__, __LINE__);

    mRadius = glm::length(mCameraLocation);

    OnEscapePressed.connect_member(this, &CubeApplication::FinishAppExecution);


}

void CubeApplication::MainLoop()
{
    aWindowManagerRef.GrabWindow(mAppWindow);
    aRenderManagerRef.SetBackgroundColor(DGE::COLOR::kBlue);

    //Invoke Shader
    aShaderPtr->use();

    //Update Camera
    auto timeValue = glfwGetTime();
    mCameraLocation = DGE::fLocation {
            mRadius * cos(timeValue + glm::pi<float>() / 4.0f ),
            3.0f * sin(timeValue),
            mRadius * sin(timeValue + glm::pi<float>() / 4.0f )
    };
    mCamera.Update();

    //Update Shader with camera Position and Orientation
    aShaderPtr->setVec3("viewPos", mCameraLocation);
    aShaderPtr->setMat4("view", mCameraTrans);


    glBindVertexArray(aCubeMesh);

    /* Draw as many boxes there are in the model */
    for (auto index = 0; index < aBoxModel.size(); ++index)
    {
        aShaderPtr->setVec3("color", aBoxColor[index]);
        aShaderPtr->setMat4("model", aBoxModel[index]);
        glDrawArrays(GL_TRIANGLES,0,36);
    }

    aWindowManagerRef.UpdateWindow(mAppWindow);

}

void CubeApplication::ShutDown() {
    aWindowManagerRef.ShutdownWindow(mAppWindow);
}




