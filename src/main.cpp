#include "utils/Model.h"
#include "utils/Skybox.h"
#include "utils/Light.h"
#include "utils/Shadow.h"
#include <filesystem>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define MAX_LIGHTS 25 // don't forget to change the value in the shader
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f

namespace fs = std::filesystem;

const unsigned int windowWidth = 1200;
const unsigned int windowHeight = 800;

Camera camera(windowWidth, windowHeight, glm::vec3(35.0f, 16.0f, 40.0f), 45.0f, NEAR_PLANE, FAR_PLANE);

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
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

// transformations scale,rotate,translate
void DrawModelTransformations(Model *&currentModel)
{
    ImGui::Text("Model Transformations");
    if (ImGui::DragFloat3("Scale", glm::value_ptr(currentModel->scale), 0.1f))
    {
        currentModel->UpdateTransformMatrix();
    }
    if (ImGui::DragFloat3("Rotate", glm::value_ptr(currentModel->rotate), 0.1f))
    {
        currentModel->UpdateTransformMatrix();
    }
    if (ImGui::DragFloat3("Translate", glm::value_ptr(currentModel->translate), 0.1f))
    {
        currentModel->UpdateTransformMatrix();
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

void DrawLightsMenu(std::vector<Light> &lights, Shader &shaderProgram, Shader &lightShader)
{
    std::string header = "Lights (" + std::to_string(lights.size()) + " / " + std::to_string(MAX_LIGHTS) + ")";
    ImGui::Text(header.c_str());
    static int currentLightIndex = 0;
    std::vector<string> lightNames;

    lightNames.push_back("Light 1 | Shadow Light");
    for (int i = 1; i < lights.size(); i++)
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
        if (lights.size() > 2)
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
        glfwCreateWindow(windowWidth, windowHeight, "PGR Model Viewer", NULL, NULL);

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
            SkyboxPaths.push_back(entry.path().string());
            SkyboxNames.push_back(entry.path().filename().string());
        }
    }

    // ==========================================

    Shader shaderProgram((parentDir + shaderPath + "pbr.vert").c_str(), (parentDir + shaderPath + "pbr.frag").c_str());
    Shader skyboxShader((parentDir + shaderPath + "skybox.vert").c_str(), (parentDir + shaderPath + "skybox.frag").c_str());
    Shader shadowShader((parentDir + shaderPath + "shadow.vert").c_str(), (parentDir + shaderPath + "shadow.frag").c_str(), (parentDir + shaderPath + "shadow.geom").c_str());
    Shader lightShader((parentDir + shaderPath + "light.vert").c_str(), (parentDir + shaderPath + "light.frag").c_str());
    Shadow shadow;
    std::vector<Light> lights = {
        Light(glm::vec3(20.0f, 18.0f, 34.0f), glm::vec3(1.0f, 1.0f, 1.0f), 700.0f),
        Light(glm::vec3(5.0f, 17.0f, 5.0f), glm::vec3(1.0f, 1.0f, 1.0f), 3000.0f)};

    shaderProgram.Activate();
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), lights.size());
    glUniform1f(glGetUniformLocation(shaderProgram.ID, "farPlane"), FAR_PLANE);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "showShadows"), true);

    for (int i = 0; i < lights.size(); i++)
    {
        lights[i].SetUniform(shaderProgram, i);
    }

    skyboxShader.Activate();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

    lightShader.Activate();
    glUniform1f(glGetUniformLocation(lightShader.ID, "lightSize"), 2.0f);

    // default model and skybox
    std::string defaultSkybox = (parentDir + skyboxPath + "clouds");
    std::string defaultModel = (parentDir + modelPath + "bmw/scene.gltf");
    Skybox skybox(defaultSkybox.c_str());
    Model bmw(defaultModel.c_str());

    // default camera view to bwm model
    camera.Orientation = glm::vec3(-0.8f,-0.1f,-0.6f);

    // default translate, rotate, scale
    glm::vec3 translate = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(10.0f, 10.0f, 10.0f);

    bmw.translate = translate;
    bmw.rotate = rotate;
    bmw.scale = scale;
    bmw.UpdateTransformMatrix();

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
    // fps count
    int frameCount = 0;
    int fps = 0;
    bool showSkybox = true;
    bool showLights = false;
    bool showShadows = true;
    float lightSize = 2.0f;
    double lastTime = glfwGetTime();
    // ==========================================

    while (!glfwWindowShouldClose(window))
    {
        // fps counter
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - lastTime >= 1.0)
        {
            fps = frameCount;
            frameCount = 0;
            lastTime += 1.0;
        }

        // ==========================================
        shadow.Bind();
        shadow.setUniforms(shadowShader, lights[0], NEAR_PLANE, FAR_PLANE);
        currentModel->Draw(shadowShader, camera);
        shadow.Unbind();

        glViewport(0, 0, camera.width, camera.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // ==========================================
        // Camera and Skybox
        if (!ImGui::GetIO().WantCaptureMouse)
            camera.Inputs(window);

        camera.updateMatrix();
        if (showSkybox)
            currentSkybox->Draw(skyboxShader, camera);

        // ==========================================
        // Draw models
        shadow.Activate(shaderProgram, 4);
        currentModel->Draw(shaderProgram, camera);

        if (showLights)
            for (int i = 0; i < lights.size(); i++)
                lights[i].Draw(lightShader, camera);

        // ==========================================
        // ImGui

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Menu");
        ImGui::Text("FPS: %d", fps);
        ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("Camera Orientation: (%.2f, %.2f, %.2f)", camera.Orientation.x, camera.Orientation.y, camera.Orientation.z);
        ImGui::Separator();
        DrawModelCombo(currentModel, ModelNames, ModelPaths, &currentModelIndex);
        DrawModelTransformations(currentModel);
        if (ImGui::Checkbox("Show Shadows", &showShadows))
        {
            shaderProgram.Activate();
            glUniform1i(glGetUniformLocation(shaderProgram.ID, "showShadows"), showShadows);
        }
        ImGui::Separator();
        DrawSkyboxCombo(currentSkybox, SkyboxNames, SkyboxPaths, &currentSkyboxIndex);
        ImGui::Checkbox("Show Skybox", &showSkybox);
        ImGui::Separator();
        DrawLightsMenu(lights, shaderProgram, lightShader);
        ImGui::Checkbox("Show Lights Objects", &showLights);
        if (showLights)
        {
            if (ImGui::SliderFloat("Light Size", &lightSize, 0.1f, 25.0f))
            {
                lightShader.Activate();
                glUniform1f(glGetUniformLocation(lightShader.ID, "lightSize"), lightSize);
            }
        }
        ImGui::Separator();

        // camera speed slider
        ImGui::SliderFloat("Camera Speed", &camera.speed, 0.0f, 10.0f);
        ImGui::SliderFloat("Camera Sensitivity", &camera.sensitivity, 0.0f, 200.0f);
        ImGui::Separator();
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
