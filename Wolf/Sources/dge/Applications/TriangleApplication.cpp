//
// Created by JG -- CPP, DEVIL on 6/26/2021.
//

extern "C"{
#include <glad/glad.h>
}
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>


#include "TriangleApplication.h"


unsigned int SCR_WIDT{800};
unsigned int SCR_HGHT{600};
unsigned int aTriangleMesh{};

std::unique_ptr<Wolf::Renderer::Shader> aShaderPtr{nullptr};
DGE::IPlatform::WindowID windowID_0;
DGE::IPlatform::WindowID windowID_1;

GLFWwindow*window_0;
GLFWwindow*window_1;


void TriangleApplication::Init() {

    mDescription = std::string {"This Application tries to render two triangles, in separate windows. It's not working."};

    auto startWindowLocation_0 = DGE::iLocation2(100, 200);
    auto startWindowLocation_1 = DGE::iLocation2(100+SCR_WIDT, 200);
    auto windowSize_0 = DGE::iSize(SCR_WIDT, SCR_HGHT);
    auto windowSize_1 = DGE::iSize(SCR_WIDT, SCR_HGHT);

    windowID_0 = aWindowManagerRef.SpawnWindow(startWindowLocation_0, windowSize_0, "My Triangle Window 0");
    windowID_1 = aWindowManagerRef.SpawnWindow(startWindowLocation_1, windowSize_1, "My Triangle Window 1");
    window_0 = (GLFWwindow*)windowID_0;
    window_1 = (GLFWwindow*)windowID_1;
    aWindowManagerRef.GrabWindow(windowID_0);
    aWindowManagerRef.GrabWindow(windowID_1);
    bInitialized = true;
}
void TriangleApplication::Load() {

    //Create a Scene with a Triangle and a Camera.
    //Create a Triangle Mesh.
    aTriangleMesh = DGE::MeshVertexNormalTextureTriangle().VAO;
    //Create a Shader.
    aShaderPtr = std::make_unique<Wolf::Renderer::Shader>("plane.vs", "plane.fs");
    if (!aShaderPtr){
        bInitialized = false;
        return;
    }

}
void TriangleApplication::MainLoop() {
    //Process
    while (!glfwWindowShouldClose(window_0))
    {
        if (glfwGetKey(window_0, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window_0, true);

    //Draw the Window 0
        aWindowManagerRef.GrabWindow(windowID_0);
    //Clear the Background
        aRenderManagerRef.SetBackgroundColor(DGE::fColor4(73.0f / 255.0f, 139.0f / 255.0f, 245.0f / 255.0f, 1.0f));

    //Activate the Shader.
        aShaderPtr->use();
        glBindVertexArray(aTriangleMesh);

        auto timeValue = glfwGetTime();

        aShaderPtr->setVec3("color", DGE::fColor3 {DGE::COLOR::kGreen.r, DGE::COLOR::kGreen.g * sin(timeValue) / 2.0f + 0.5f, DGE::COLOR::kGreen.b});

    //Flush to Screen.
        glDrawArrays(GL_TRIANGLES, 0, 3);
        aWindowManagerRef.UpdateWindow(windowID_0);
        aWindowManagerRef.GrabWindow(windowID_1);
    //Clear the Background
        aRenderManagerRef.SetBackgroundColor(DGE::fColor4(73.0f / 255.0f, 245.0f / 255.0f, 139.0f / 255.0f, 1.0f));
        aShaderPtr->use();
        glBindVertexArray(aTriangleMesh);
        aShaderPtr->setVec3("color", DGE::COLOR::KRed);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        aWindowManagerRef.UpdateWindow(windowID_1);
        glfwPollEvents();
    }

}
void TriangleApplication::ShutDown() {
    aWindowManagerRef.ShutdownWindow(windowID_0);
}

