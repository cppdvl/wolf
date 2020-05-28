


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <wolf/glfw/glfw.hpp>
#include <wolf/utils/maputils.hpp>
#include <wolf/renderer/renderer.hpp>
#include <wolf/renderer/instance.hpp>


#include <wolf/renderer/shader.hpp>
#include <wolf/renderer/camera.hpp>
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
int window_size [2] = {1280, 720};

// Wolf Renderer
std::vector <unsigned int> foo {1,2,3};




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

// cursor
double cursorPos[2] = {1.0, 5.0};


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Gui State
bool guiMode = false;
bool guiModeKeyPressed = false;

//void killPrimitivePlane(unsigned int&, unsigned int&);
//void primitivePlane(unsigned int&, unsigned int&); 
/*void fuhrerCube(
    std::vector<unsigned int>&vaovector, 
    std::vector<unsigned int>&vbovector, 
    std::vector<unsigned int>&vertexcount,
    std::vector<std::map<std::string, glm::vec3>>& matdictionaryvector,
    std::vector<std::string>& texturenamevector);*/

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
    float* pClearColor,
    int* pColSelector,
    int* pRowSelector,
    double* pCursorPos,
    int* pWindowSize);
void renderGui();
void renderGuiOpenGL();
int main()
{   
    auto glfwConfiguration = Wolf::OGLUtil::GLFWInitConfiguration{};
    auto pWindow = Wolf::OGLUtil::GLFWInit(glfwConfiguration);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    //{
    //    std::cout << "Failed to initialize GLAD" << std::endl;
    //    return -1;
    //}

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup Dear ImGui Context: tell whos the window (GLFW) and what's the GLSL version.
    setupGuiContext(pWindow, "#version 330");

    // @TODO: This should be gone for good. This must initialize from a specific Json File. 
    // build and compile shaders:
    // -------------------------
    Wolf::Renderer::Shader shader("lighting.vs", "wavefront.fs");

    // TODO: This should be done by a load scene function.
    // -------------------------
    unsigned int planeVAO, planeVBO;
    /*primitivePlane(planeVAO, planeVBO);
    fuhrerCube(
        fuhrerCubeVAO, 
        fuhrerCubeVBO, 
        fuhrerCubeVertexCount, 
        fuhrerCubeMaterial, 
        fuhrerCubeTextures);*/

    /*for (auto&fct : fuhrerCubeTextures) {
        std::cout << " Texture: " << (fct.empty()?"none" : fct) << std::endl;
    }*/

    //auto rndr = WolfRenderer(shader.ID,fuhrerCubeVAO);
    auto shdrdomain = Wolf::Renderer::ShaderDomain<Wolf::Renderer::Shader, Wolf::Renderer::Instance<glm::vec4>>{shader};

    //auto fwCube = Wolf::Renderer::FWVAO_T<Wolf::Renderer::Instance<glm::vec4>>{fuhrerCubeVAO};
    //auto anObject = Wolf::Renderer::Instance<glm::vec4>();
    //anObject.tMatrix = glm::vec4{1.0f};
    //fwCube.push_back(anObject);

    //auto fwFloor = Wolf::Renderer::FWVAO_T<Wolf::Renderer::Instance<glm::vec4>>{planeVAO};
    //auto anotherObject = Wolf::Renderer::Instance<glm::vec4>();
    //anotherObject.tMatrix = glm::vec4{1.0f};
    //fwFloor.push_back(anotherObject);

    //shdrdomain.push_back(fwCube);
    //shdrdomain.push_back(fwFloor);

    
    // load textures
    // -------------
    auto woodPath = std::filesystem::absolute("Textures/wood.png").string();
    unsigned int floorTexture = loadTexture(woodPath.c_str());
    auto wallsPath = std::filesystem::absolute("Textures/Atlases/wolf/walls.png").string();
    unsigned int fuhrerTexture = loadTexture(wallsPath.c_str(), 1);
    std::cout << "Fuhrer Texture: " << fuhrerTexture << std::endl;
    // shader configuration
    // --------------------
    shader.use();
    
    // lighting info
    // -------------
    glm::vec3 lightPos(-3.0f, 3.0f, 3.0f);
    

    // Clear Color in the GUI
    // ----------------------
    //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool useLight{true};
    guiText = blinn ? blinnString.data() : phongString.data();
    int colSelector{5};
    int rowSelector{13};


    
    // render loop
    // -----------
    
    while (!glfwWindowShouldClose(pWindow))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(pWindow);

        
        // gui
        // -----
        
        //drawGui();
        /*drawGuiExtended(
            guiText,
            &useLight,
            (float*)&clear_color,
            &colSelector,
            &rowSelector,
            cursorPos,
            window_size
            );
        renderGui();*/

        
        // render
        // ------
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // draw objects
        //auto shdrPtr = shdrdomain.CommonDataPtr();
        //shdrPtr->use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        //shdrPtr->setMat4("projection", projection);
        //shdrPtr->setMat4("view", view);
        // set light uniforms
        //shdrPtr->setVec3("viewPos", camera.Position);

        //shdrPtr->setVec3("lightPos", lightPos);
        //shdrPtr->setBool("useLight", useLight);

        //shdrPtr->setBool("blinn", blinn);

        // floor

        // fuhrercube
        const auto fuhrerCubeVaoCount = fuhrerCubeVAO.size();
        auto fuhrerCubeVaoPtr = fuhrerCubeVAO.data();
        //auto fuhrerCubeVboPtr = fuhrerCubeVBO.data();
        auto fuhrerCubeVertexCountPtr = fuhrerCubeVertexCount.data();

        //for (auto& fwvao : shdrdomain){

            /*for (auto& vao : fwvao.commonData) {

                glBindVertexArray(vao);

                for (auto& instance : fwvao){

                    if (vao == planeVAO){

                        shader.setFloat("textureFactor", 1.0f);
                        shader.setInt("colselector_x", 0);
                        shader.setInt("colselector_y", 0);
                        shader.setFloat("sprite_texture_width", 1.0f);
                        shader.setFloat("sprite_texture_height", 1.0f);
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, floorTexture);
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                        glBindTexture(GL_TEXTURE_2D, fuhrerTexture);

                    } else {


                    }

                }

            }*/

        //}
        /*
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
        */
        for (auto fuhrerCubeIndx = static_cast<decltype(fuhrerCubeVaoCount)>(0); fuhrerCubeIndx < fuhrerCubeVaoCount; ++fuhrerCubeIndx){
            
            auto vao = fuhrerCubeVaoPtr[fuhrerCubeIndx];
            auto vertexCount = fuhrerCubeVertexCountPtr[fuhrerCubeIndx];

            glBindVertexArray(vao);
            auto k_d = fuhrerCubeMaterial[fuhrerCubeIndx]["kd"];
            //shdrPtr->setVec3("kd", k_d);
            //shdrPtr->setFloat("textureFactor", fuhrerCubeTextures[fuhrerCubeIndx].empty() ? 0.0f : 1.0f);

            //shdrPtr->setInt("colselector_x", colSelector);
            //shdrPtr->setInt("colselector_y", rowSelector);
            //shdrPtr->setFloat("sprite_texture_width", 0.166666666);
            //shdrPtr->setFloat("sprite_texture_height", 0.0526315);

            
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        
        }
        
        //std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;

        
        renderGuiOpenGL();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }

    //killPrimitivePlane(planeVAO, planeVBO);
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
    
    glfwGetCursorPos(window, &cursorPos[0], &cursorPos[1]);   

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* , int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    window_size[0] = width;
    window_size[1] = height;
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow*, double xpos, double ypos)
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
void scroll_callback(GLFWwindow*, double, double yoffset)
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
        else{ 
            std::cout << "Fatal error...." << std::endl;
            std::exit(-1);
        }

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
