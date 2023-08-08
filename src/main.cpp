#include "utils/Model.h"
#include "utils/Skybox.h"
#include <filesystem>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


namespace fs = std::filesystem;

const unsigned int windowWidth = 1200;
const unsigned int windowHeight = 800;

Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 3.0f));

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    camera.width = width;
    camera.height = height;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window =
        glfwCreateWindow(windowWidth, windowHeight, "Monnte", nullptr, nullptr);

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // init ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();
    // ==========================================

    std::string parentDir = fs::current_path().parent_path().string() + "/" + "project";
    std::string texPath = "/assets/textures/";
    std::string shaderPath = "/assets/shaders/";
    std::string modelPath = "/assets/models/";
    std::string skyboxPath = "/assets/skybox/clouds/";

    std::vector<std::string> facesCubemap = {
        parentDir + skyboxPath + "px.png",
        parentDir + skyboxPath + "nx.png",
        parentDir + skyboxPath + "py.png",
        parentDir + skyboxPath + "ny.png",
        parentDir + skyboxPath + "pz.png",
        parentDir + skyboxPath + "nz.png"};

    // ==========================================

    Shader shaderProgram((parentDir + shaderPath + "pbr.vert").c_str(), (parentDir + shaderPath + "pbr.frag").c_str());
    Shader skyboxShader((parentDir + shaderPath + "skybox.vert").c_str(), (parentDir + shaderPath + "skybox.frag").c_str());

    // 4 light sources for the scene in 4 corners
    std::vector<glm::vec3> lightColors = {
        glm::vec3(150.0f, 150.0f, 150.0f),
        glm::vec3(150.0f, 150.0f, 150.0f)

    };
    std::vector<glm::vec3> lightPositions = {
        glm::vec3(-0.7f, 4.0f, -5.0f),
        glm::vec3(-0.7f, 4.0f, 5.0f)

    };

    // glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    shaderProgram.Activate();
    for (int i = 0; i < lightColors.size(); i++)
    {
        std::string lightColorName = "lightColors[" + std::to_string(i) + "]";
        std::string lightPosName = "lightPositions[" + std::to_string(i) + "]";
        glUniform3f(glGetUniformLocation(shaderProgram.ID, lightColorName.c_str()), lightColors[i].x, lightColors[i].y, lightColors[i].z);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, lightPosName.c_str()), lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
    }

    skyboxShader.Activate();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

    Skybox skybox(facesCubemap);

    // Model sword((parentDir + modelPath + "sword/scene.gltf").c_str());
    // Model toy((parentDir + modelPath + "toy_car/scene.gltf").c_str());
    // Model bmw((parentDir + modelPath + "bmw/scene.gltf").c_str());
    // Model snake((parentDir + modelPath + "snake/scene.gltf").c_str());
    // Model bmw2((parentDir + modelPath + "bmw2/scene.gltf").c_str());

    // Model car((parentDir + modelPath + "car/scene.gltf").c_str());
    // Model room((parentDir + modelPath + "room/scene.gltf").c_str());
    // Model bmw1((parentDir + modelPath + "bmw1/scene.gltf").c_str());
    // Model cat((parentDir + modelPath + "cat/scene.gltf").c_str());
    // Model floor((parentDir + modelPath + "cobble_floor/patterned_cobblestone_4k.gltf").c_str());

    // ==========================================

    // ==========================================

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!ImGui::GetIO().WantCaptureMouse)
            camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);
        skybox.Draw(skyboxShader, camera);

                // imgui hello world
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        

        // sword.Draw(shaderProgram, camera);
        // room.Draw(shaderProgram, camera);
        // toy.Draw(shaderProgram, camera);
        // snake.Draw(shaderProgram, camera);
        // bmw.Draw(shaderProgram, camera);
        // bmw2.Draw(shaderProgram, camera);
        // bmw1.Draw(shaderProgram, camera);
        // car.Draw(shaderProgram, camera);
        // floor.Draw(shaderProgram, camera);
        // cat.Draw(shaderProgram, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ==========================================
    shaderProgram.Delete();

    // ==========================================

    glfwTerminate();
    return 0;
}