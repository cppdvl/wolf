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

#include "FPSDemoApplication.h"


static float lastime{0.0f};
void FPSDemoApplication::FPSKinematic(float timeaccel)
{

    //Update Position of Camera and where is looking at by calculating the integral of speed
    float nowtime{0.0f};
    float deltatime = nowtime - lastime;
    deltatime *= timeaccel;

    auto speed = 1.0f;
    [[maybe_unused]] auto deltaposition = deltatime * speed;



}

void FPSDemoApplication::MainLoop()
{
    aWindowManagerRef.GrabWindow(mAppWindow);
    aRenderManagerRef.SetBackgroundColor(DGE::COLOR::BlueViolet);

    //InvokeShader
    levelShader->use();





}

void FPSDemoApplication::ShutDown()
{
    aWindowManagerRef.ShutdownWindow(mAppWindow);
}
