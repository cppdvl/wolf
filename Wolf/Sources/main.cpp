#include <map>
#include <mutex>
#include <vector>
#include <utility>
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>

//wolfengine
#include <wolf/renderer/shader.hpp>
#include <signalslot.h>

extern "C"{
#include <glad/glad.h>
}
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

//dge
#include "dge/singleton.h"
#include "dge/dgetypes.h"
#include "dge/colors.h"
#include "dge/rendermanager.h"
#include "dge/windowsmanager.h"
#include "dge/camera.h"
#include "dge/mesh.h"
#include "dge/texture.h"
#include "dge/inputmanager.h"
#include "dge/platform/platform.h"

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
            MainLoop();
            ShutDown();
        }
    };

}

class MyApplicationCube : public DGE::Application {

    DGE::iSize mScreenSize{800,600};
    float mAspectRatio{(float)mScreenSize.x/(float)mScreenSize.y};
    float mFov{60.0f};

    unsigned int aCubeMesh{};
    glm::mat4 aBoxModel{glm::mat4{1.0f}};

    std::unique_ptr<Wolf::Renderer::Shader> aShaderPtr{nullptr};

    const DGE::fLocation kLightPosition {3.0f, 3.0f, -3.0f};

    bool bUsePhongShading{true};

    glm::mat4 mProjection = glm::perspective(glm::radians(mFov), mAspectRatio, 0.1f, 100.0f);

protected:

    void Init() override {

        mDescription = std::string {"A Simple Cubes - Camera Navigation example."};
        auto startWindowLocation = DGE::iLocation2(100, 200);
        mAppWindow = aWindowManagerRef.SpawnWindow(startWindowLocation, mScreenSize, "Simple Cubes Application");

        bInitialized = true;
    }
    void Load() override {

        //Create a Scene with a Mesh-Texture-Cube mesh, and a Camera.
        aCubeMesh = DGE::MeshVertexNormalTextureCube().VAO;

        //Create a Shader.
        aShaderPtr = std::make_unique<Wolf::Renderer::Shader>("plain.vs", "plain.fs");
        if (!aShaderPtr){
            bInitialized = false;
            return;
        }
        aShaderPtr->use();
        aShaderPtr->setMat4("projection", mProjection);
        aShaderPtr->setBool("blinn", bUsePhongShading);
        aShaderPtr->setVec3("lightPos", kLightPosition);
        aShaderPtr->setVec3("color", DGE::COLOR::kGreen);

        spdlog::info("{:s}{:d} Finished loding the scene", __FILE__, __LINE__);

    }


    void MainLoop() override {
        //Process
        auto radius = glm::length(mCamera.mPosition);
        while (aWindowManagerRef.IsWindowClosing(mAppWindow) == false) {

            if (anInputManagerRef.GetKeyState(mAppWindow, DGE::Keys::K_ESC) == DGE::KeyState::STATE_DOWN) aWindowManagerRef.CloseWindow(mAppWindow);

            aWindowManagerRef.GrabWindow(mAppWindow);
            // Paint Background Fill Color & Update Window
            aRenderManagerRef.SetBackgroundColor(DGE::COLOR::kBlue);

            //Select Shader to Use and Configure it.
            aShaderPtr->use();
            //UPDATECAMERA()
            auto timeValue = glfwGetTime();
            mCamera.SetPositionAndTargetAndThenUpdate(
                glm::vec3{
                    radius*cos(timeValue + glm::pi<float>() / 4.0f),
                    3.0f * sin(timeValue),
                    radius*sin(timeValue + glm::pi<float>() / 4.0f)
                },
                glm::vec3{0.0f, 0.0f, 0.0f}
                );
            aShaderPtr->setVec3("viewPos", mCamera.mPosition);
            aShaderPtr->setMat4("view", mCamera.GetViewMatrix());
            //FOR EACH MODEL UPDATEMODEL()
            glBindVertexArray(aCubeMesh);
            //UPDATE MODEL()
            aShaderPtr->setMat4("model", aBoxModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            aWindowManagerRef.UpdateWindow(mAppWindow);

            UpdateEvents();
        }

    }
    void ShutDown() override {
        aWindowManagerRef.ShutdownWindow(mAppWindow);
    }

public:

    class Camera {
        friend class MyApplicationCube;
        const glm::vec3 referenceUp{0.0f, 1.0f, 0.0f};
        glm::mat4 mTransform {1.0f};
        DGE::fLocation mPosition {3.0f, 3.0f, 3.0f};
        DGE::fLocation mFront{-mPosition};
        glm::vec3 mRight{};
        glm::vec3 mUp{referenceUp};
    public:

        glm::mat4& GetViewMatrix(){
            return mTransform;
        }

        void Update(){
            mTransform = glm::lookAt(mPosition, mPosition + mFront, mUp);
        }
        void SetPositionAndTargetAndThenUpdate(glm::vec3 position, glm::vec3 lookat){
            mPosition = position;

            mFront = lookat - position;
            auto camZAxis = -mFront;
            camZAxis = glm::normalize(camZAxis);

            mRight = glm::cross(referenceUp, camZAxis);
            mUp = glm::cross(camZAxis, mRight);
            Update();
        }
        Camera(glm::vec3 position, glm::vec3 lookat){
            SetPositionAndTargetAndThenUpdate(position, lookat);
        }
        Camera() { Update(); }
    };
    Camera mCamera{};

};

class MyApplicationTriangle : public DGE::Application{

    unsigned int SCR_WIDT{800};
    unsigned int SCR_HGHT{600};
    unsigned int aTriangleMesh{};
    std::unique_ptr<Wolf::Renderer::Shader> aShaderPtr{nullptr};
    DGE::IPlatform::WindowID windowID_0;
    DGE::IPlatform::WindowID windowID_1;
    
    GLFWwindow*window_0;
    GLFWwindow*window_1;


protected:
    void Init() override{

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
    void Load() override {

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
    void MainLoop() override {
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
    void ShutDown() override {
        aWindowManagerRef.ShutdownWindow(windowID_0);
    }

};


int main ([[maybe_unused]] int argc, [[maybe_unused]] const char **argv){

    auto myApplication = MyApplicationCube();
    myApplication.Exec();

    //Shutdown

    auto& platform = DGE::PlatformGLFW::GetInstanceReference();
    platform.ShutDown();
    std::cout << "-- FINISHED OK --" << std::endl;
    return 0;
}
