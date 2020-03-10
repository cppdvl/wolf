#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <wolf/maputils.hpp>
#include <wolf/import/_3dformats/objfileparser.hpp>


#include <imgui.h>  
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <filesystem.hpp>

#include <shader_m.hpp>
#include <camera.hpp>
#include <stb_image.h>


#include <string>
#include <vector>
#include <iostream>


/*************************************************************/
/** General Settings : Apply always when using GLFW+OpenGL  **/
/*************************************************************/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// opengl GLSL version
// const std::string glsl_version {"#version 330"};

/********************************************************/
/***************** Application Settings *****************/
/********************************************************/

unsigned int loadTexture(const char *path, int vertical_inversion = 0);

bool blinn = false;
bool blinnKeyPressed = false;

std::string blinnString {"Blinn - Phong"};
std::string phongString {"Phong"};
char* guiText{nullptr};
// camera
Camera camera(glm::vec3(0.0f, 1.0f, 8.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Gui State
bool guiMode = false;
bool guiModeKeyPressed = false;

void killPrimitivePlane(unsigned int&, unsigned int&);
void primitivePlane(unsigned int&, unsigned int&); 
void fuhrerCube(
    std::vector<unsigned int>&vaovector, 
    std::vector<unsigned int>&vbovector, 
    std::vector<unsigned int>&vertexcount,
    std::vector<std::map<std::string, glm::vec3>>& matdictionaryvector,
    std::vector<std::string>& texturenamevector);

std::vector<unsigned int> fuhrerCubeVAO{};
std::vector<unsigned int> fuhrerCubeVBO{};
std::vector<unsigned int> fuhrerCubeVertexCount{};
std::vector<std::map<std::string, glm::vec3>> fuhrerCubeMaterial{};
std::vector<std::string> fuhrerCubeTextures{};

void setupGuiContext(GLFWwindow* pWind, const std::string glsl_version);
void drawGui();
void drawGuiExtended(
    char* guiText,
    bool* pUseLight,
    float* pClearColor);
void renderGui();
void renderGuiOpenGL();
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup Dear ImGui Context: tell whos the window (GLFW) and what's the GLSL version.
    setupGuiContext(window, "#version 330");


    // build and compile shaders
    // -------------------------
    Shader shader("lighting.vs", "wavefront.fs");

    // plane & fuhrer
    // -------------------------
    unsigned int planeVAO, planeVBO;
    primitivePlane(planeVAO, planeVBO);
    fuhrerCube(
        fuhrerCubeVAO, 
        fuhrerCubeVBO, 
        fuhrerCubeVertexCount, 
        fuhrerCubeMaterial, 
        fuhrerCubeTextures);

    for (auto&fct : fuhrerCubeTextures) {
        std::cout << " Texture: " << (fct.empty()?"none" : fct) << std::endl;
    }

    // load textures
    // -------------
    unsigned int floorTexture = loadTexture(FileSystem::getPath("Resources/Textures/wood.png").c_str());
    //unsigned int fuhrerTexture = loadTexture(FileSystem::getPath("../Build/Glitter/fuhrer.png").c_str(), 1);
    unsigned int fuhrerTexture = loadTexture(FileSystem::getPath("Resources/Textures/Atlases/walls.png").c_str(), 1);
    std::cout << "Fuhrer Texture: " << fuhrerTexture << std::endl;
    // shader configuration
    // --------------------
    shader.use();
    
    // lighting info
    // -------------
    glm::vec3 lightPos(-3.0f, 3.0f, 3.0f);
    

    // Clear Color in the GUI
    // ----------------------
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool useLight{true};
    guiText = blinn ? blinnString.data() : phongString.data();


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        
        // gui
        // -----
        
        drawGui();
        drawGuiExtended(
            guiText,
            &useLight,
            (float*)&clear_color);
        renderGui();

        
        // render
        // ------
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // draw objects
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        shader.setVec3("viewPos", camera.Position);

        shader.setVec3("lightPos", lightPos);
        shader.setBool("useLight", useLight);

        shader.setBool("blinn", blinn);

        // floor

        // fuhrercube
        const auto fuhrerCubeVaoCount = fuhrerCubeVAO.size();
        auto fuhrerCubeVaoPtr = fuhrerCubeVAO.data();
        auto fuhrerCubeVboPtr = fuhrerCubeVBO.data();
        auto fuhrerCubeVertexCountPtr = fuhrerCubeVertexCount.data();

        glBindVertexArray(planeVAO);
        shader.setFloat("textureFactor", 1.0f);

        shader.setInt("colselector_x", 0);
        shader.setInt("colselector_y", 0);
        shader.setFloat("sprite_texture_width", 1.0f);
        shader.setFloat("sprite_texture_height", 1.0f);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        

        
        glBindTexture(GL_TEXTURE_2D, fuhrerTexture);
        for (auto fuhrerCubeIndx = 0; fuhrerCubeIndx < fuhrerCubeVaoCount; ++fuhrerCubeIndx){
            
            auto vao = fuhrerCubeVaoPtr[fuhrerCubeIndx];
            auto vertexCount = fuhrerCubeVertexCountPtr[fuhrerCubeIndx];

            glBindVertexArray(vao);
            auto k_d = fuhrerCubeMaterial[fuhrerCubeIndx]["kd"];
            shader.setVec3("kd", k_d);
            shader.setFloat("textureFactor", fuhrerCubeTextures[fuhrerCubeIndx].empty() ? 0.0f : 1.0f);

            shader.setInt("colselector_x", 5);
            shader.setInt("colselector_y", 13);
            shader.setFloat("sprite_texture_width", 0.166666666);
            shader.setFloat("sprite_texture_height", 0.0526315);

            
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        
        }
        
        //std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;

        
        renderGuiOpenGL();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    killPrimitivePlane(planeVAO, planeVBO);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (!guiMode){

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed) 
    {
        blinn = !blinn;
        blinnKeyPressed = true;
        guiText = blinn ? blinnString.data() : phongString.data();

    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) 
    {
        blinnKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !guiModeKeyPressed) 
    {
        guiMode = !guiMode;
        guiModeKeyPressed = true;
        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, guiMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) 
    {
        guiModeKeyPressed = false;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if (!guiMode){
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path, int vertical_inversion)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(vertical_inversion);   
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
