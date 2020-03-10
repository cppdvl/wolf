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

void drawGui(){

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();  
}

// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        
void drawGuiExtended(
    char* guiText,
    bool* pUseLight,
    float* pClearColor,
    int* pColSelector,
    int* pRowSelector){
    bool check1{true};
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.


    ImGui::Text(guiText);               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Use Illumination", pUseLight);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Check 1", &check1);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", pClearColor); // Edit 3 floats representing a color
    ImGui::SliderInt("Grid X", pColSelector, 0, 5);
    ImGui::SameLine();
    ImGui::SliderInt("Grid Y", pRowSelector, 0, 18);    
    if (ImGui::Button("Button")) counter++; // Buttons return true when clicked (most widgets return true when edited/activated)
                
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

}

void renderGui(){
    //Update Imgui Render but for ImGui itself?
    ImGui::Render();

}
void renderGuiOpenGL(){
    //ImGui Render in the Opengl context
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
}
