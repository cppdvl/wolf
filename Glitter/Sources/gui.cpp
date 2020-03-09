#include <string>
#include <imgui.h>  
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void setupGuiContext(GLFWwindow* pwindow, const std::string glsl_version){
	// Setup Dear ImGui Context
    // ------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    // ----------------------
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // --------------------------------
    ImGui_ImplGlfw_InitForOpenGL(pwindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}