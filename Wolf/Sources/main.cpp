/*#include <map>
#include <mutex>
#include <vector>
#include <utility>
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cxxopts.hpp>*/
//wolfengine
/*#include <wolf/renderer/shader.hpp>
#include <signalslot.h>
*/
extern "C"{
#include <glad/glad.h>
}
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
/*
//dge
#include "dge/singleton.h"
#include "dge/dgetypes.h"
#include "dge/colors.h"
#include "dge/rendermanager.h"
#include "dge/windowsmanager.h"
#include "dge/camera.h"
#include "dge/mesh.h"
#include "dge/texture.h"
#include "dge/platform/platform.h"
#include "dge/inputmanager.h"
#include "dge/application.h"

#include "dge/Applications/TriangleApplication.h"*/
#include "dge/Applications/CubeApplication.h"


int main ([[maybe_unused]] int argc, [[maybe_unused]] const char **argv){

    auto myApplication = CubeApplication();
    myApplication.Exec();

    //Shutdown

    auto& platform = DGE::PlatformGLFW::GetInstanceReference();
    platform.ShutDown();
    std::cout << "-- FINISHED OK --" << std::endl;
    return 0;
}
