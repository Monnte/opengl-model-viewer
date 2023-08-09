#include "utils/Model.h"
#include "utils/Skybox.h"
#include "utils/Light.h"
#include <filesystem>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define MAX_LIGHTS 25 // don't forget to change the value in the shader

namespace fs = std::filesystem;

const unsigned int windowWidth = 1200;
const unsigned int windowHeight = 800;

Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 1.0f, 2.0f), 45.0f, 0.1f, 100.0f);

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    camera.width = width;
    camera.height = height;
}

void DrawModelCombo(Model *&currentModel, std::vector<string> &modelNames, std::vector<string> &modelPaths, int *currentName)
{
    ImGui::Text("Select Model");
    if (ImGui::Combo(
            "##ModelCombo", currentName, [](void *data, int idx, const char **out_text)
            {
            *out_text = (*(std::vector<std::string> *)data)[idx].c_str();
            return true; },
            (void *)&modelNames, modelNames.size()))
    {
        currentModel = new Model(modelPaths[*currentName]);
    }
}

void DrawSkyboxCombo(Skybox *&currentSkybox, std::vector<string> &skyboxNames, std::vector<string> &skyboxPaths, int *currentName)
{
    ImGui::Text("Select Skybox");
    if (ImGui::Combo(
            "##SkyboxCombo", currentName, [](void *data, int idx, const char **out_text)
            {
            *out_text = (*(std::vector<std::string> *)data)[idx].c_str();
            return true; },
            (void *)&skyboxNames, skyboxNames.size()))
    {
        currentSkybox = new Skybox(skyboxPaths[*currentName]);
    }
}

void DrawLightsMenu(std::vector<Light> &lights, Shader &shaderProgram)
{
    std::string header = "Lights (" + std::to_string(lights.size()) + " / " + std::to_string(MAX_LIGHTS) + ")";
    ImGui::Text(header.c_str());
    static int currentLightIndex = 0;
    std::vector<string> lightNames;
    for (int i = 0; i < lights.size(); i++)
    {
        lightNames.push_back("Light " + std::to_string(i + 1));
    }

    ImGui::Combo(
        "##LightCombo", &currentLightIndex, [](void *data, int idx, const char **out_text)
        {
            *out_text = (*(std::vector<std::string> *)data)[idx].c_str();
            return true; },
        (void *)&lightNames, lightNames.size());

    if (ImGui::DragFloat3("Position", glm::value_ptr(lights[currentLightIndex].position), 0.1f))
    {
        lights[currentLightIndex].SetUniform(shaderProgram, currentLightIndex);
    }
    if (ImGui::ColorEdit3("Color", glm::value_ptr(lights[currentLightIndex].color)))
    {
        lights[currentLightIndex].SetUniform(shaderProgram, currentLightIndex);
    }
    if (ImGui::DragFloat("Intensity", &lights[currentLightIndex].intensity, 5.0f, 0.0f, 10000.0f))
    {
        lights[currentLightIndex].SetUniform(shaderProgram, currentLightIndex);
    }

    if (ImGui::Button("Add Light"))
    {
        if (lights.size() < MAX_LIGHTS)
        {
            lights.push_back(Light(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), 200.0f));
            lights[lights.size() - 1].SetUniform(shaderProgram, lights.size() - 1);
            glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), lights.size());
            currentLightIndex = lights.size() - 1;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Remove Light"))
    {
        if (lights.size() > 1)
        {
            lights.pop_back();
            glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), lights.size());
            currentLightIndex = lights.size() - 1;
        }
    }
}

int main()
{
    // init glfw and glad
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
    std::string skyboxPath = "/assets/skybox/";

    // get all folders in modelPath that contains gltf files
    std::vector<string> ModelPaths = {};
    std::vector<string> ModelNames = {};

    std::vector<string> SkyboxPaths = {};
    std::vector<string> SkyboxNames = {};

    for (const auto &entry : fs::directory_iterator(parentDir + modelPath))
    {
        if (entry.is_directory())
        {
            for (const auto &subEntry : fs::directory_iterator(entry.path()))
            {
                if (subEntry.path().extension() == ".gltf")
                {
                    ModelPaths.push_back(subEntry.path().string());
                    ModelNames.push_back(entry.path().filename().string());
                }
            }
        }
    }

    for (const auto &entry : fs::directory_iterator(parentDir + skyboxPath))
    {
        if (entry.is_directory())
        {
            // save path to skybox and folder name
            SkyboxPaths.push_back(entry.path().string());
            SkyboxNames.push_back(entry.path().filename().string());
        }
    }

    // ==========================================

    // std::vector<std::string> facesCubemap = {
    //     parentDir + skyboxPath + "clouds/px.png",
    //     parentDir + skyboxPath + "clouds/nx.png",
    //     parentDir + skyboxPath + "clouds/py.png",
    //     parentDir + skyboxPath + "clouds/ny.png",
    //     parentDir + skyboxPath + "clouds/pz.png",
    //     parentDir + skyboxPath + "clouds/nz.png"};

    // ==========================================

    Shader shaderProgram((parentDir + shaderPath + "pbr.vert").c_str(), (parentDir + shaderPath + "pbr.frag").c_str());
    Shader skyboxShader((parentDir + shaderPath + "skybox.vert").c_str(), (parentDir + shaderPath + "skybox.frag").c_str());

    std::vector<Light> lights = {
        Light(glm::vec3(-0.7f, 4.0f, -5.0f), glm::vec3(1.0f, 1.0f, 1.0f), 150.0f),
        Light(glm::vec3(-0.7f, 4.0f, 5.0f), glm::vec3(1.0f, 1.0f, 1.0f), 150.0f)};

    shaderProgram.Activate();
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), lights.size());
    for (int i = 0; i < lights.size(); i++)
    {
        lights[i].SetUniform(shaderProgram, i);
    }

    skyboxShader.Activate();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

    // default model and skybox
    std::string defaultSkybox = (parentDir + skyboxPath + "clouds");
    std::string defaultModel = (parentDir + modelPath + "bmw/scene.gltf");
    Skybox skybox(defaultSkybox.c_str());
    Model bmw(defaultModel.c_str());

    // ==========================================

    // ==========================================

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Model *currentModel = &bmw;
    Skybox *currentSkybox = &skybox;
    // find index of default model and skybox
    int currentModelIndex = std::find(ModelPaths.begin(), ModelPaths.end(), defaultModel) - ModelPaths.begin();
    int currentSkyboxIndex = std::find(SkyboxPaths.begin(), SkyboxPaths.end(), defaultSkybox) - SkyboxPaths.begin();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ==========================================
        // Camera and Skybox
        if (!ImGui::GetIO().WantCaptureMouse)
            camera.Inputs(window);

        camera.updateMatrix();
        currentSkybox->Draw(skyboxShader, camera);

        // ==========================================
        // Draw models

        currentModel->Draw(shaderProgram, camera);

        // ==========================================
        // ImGui

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Menu");

        // ==========================================
        DrawModelCombo(currentModel, ModelNames, ModelPaths, &currentModelIndex);
        DrawSkyboxCombo(currentSkybox, SkyboxNames, SkyboxPaths, &currentSkyboxIndex);
        DrawLightsMenu(lights, shaderProgram);
        // ==========================================

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ==========================================
    shaderProgram.Delete();

    // ==========================================

    glfwTerminate();
    return 0;
}
