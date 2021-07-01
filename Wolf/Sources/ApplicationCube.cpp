//
// Created by User on 6/4/2021.
//
//dge

#include <memory>
#include <string>

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <signalslot.h>
#include <wolf/renderer/shader.hpp>

extern "C"{
    #include <glad/glad.h>
}
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <GLFW/glfw3native.h>
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
#include "dge/application.h"

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

    float mRadius {0.0f};
    GTech::Signal<> OnEscapePressed;

protected:

    void FinishAppExecution(){
        bRunning = false;
        aWindowManagerRef.CloseWindow(mAppWindow);

    }

    virtual void UpdateEvents() override {


        if (anInputManagerRef.GetKeyState(mAppWindow, DGE::Keys ::K_ESC) == DGE::KeyState::STATE_DOWN)
            OnEscapePressed.emit();

    }


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

        mRadius = glm::length(mCamera.mPosition);

        OnEscapePressed.connect_member(this, &MyApplicationCube::FinishAppExecution);

    }


    void MainLoop() override {
        //Process

        //if (anInputManagerRef.GetKeyState(mAppWindow, DGE::Keys ::K_ESC) == DGE::KeyState::STATE_DOWN) FinishAppExecution();

        aWindowManagerRef.GrabWindow(mAppWindow);
        // Paint Background Fill Color & Update Window
        aRenderManagerRef.SetBackgroundColor(DGE::COLOR::kBlue);

        //Select Shader to Use and Configure it.
        aShaderPtr->use();
        //UPDATECAMERA()
        auto timeValue = glfwGetTime();
        mCamera.SetPositionAndTargetAndThenUpdate(
                glm::vec3{
                        mRadius*cos(timeValue + glm::pi<float>() / 4.0f),
                        3.0f * sin(timeValue),
                        mRadius*sin(timeValue + glm::pi<float>() / 4.0f)
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