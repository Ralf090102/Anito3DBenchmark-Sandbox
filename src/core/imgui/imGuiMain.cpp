#include "ImGuiMain.hpp"
#include <ng-log/logging.h>
#include <IconsFontAwesome5.h>

namespace Anito3D {

    ImGuiMain::ImGuiMain() : selectedScene(""), selectedRendererIndex(0), selectedSceneIndex(0), selectedResolutionIndex(0),
        rayTracingEnabled(false), pbrEnabled(false), giEnabled(false) {

        // Initialize renderer options
        renderers = {
            "None",
            "BGFX",
            "Diligent Engine",
            "Falcor",
            "Ogre3D",
            "Cycles",
            "Intermediate Graphics Library (IGL)",
            "Godot Renderer",
            "Unreal Renderer"
        };

        // Initialize model options (placeholder paths, update as needed)
        models = {
            "None",
            "sphere.obj",
            "cube.obj",
            "bunny.obj"
        };

        selectedModelIndices = { 0 }; // Start with one dropdown
        selectedModels = { "" };      // Start with "None"

        // Initialize scene options
        scenes = {
            "None",
            "Crytek Sponza",
            "Amazon Lumberyard",
            "Bistro"
        };

        // Initialize resolution options
        resolutions = {
            "2560x1440",
            "1920x1080",
            "1920x1200",
            "1600x900",
            "1536x864",
            "1440x900",
            "1366x768",
            "1360x768",
            "1280x1024",
            "1280x800",
            "1280x720",
            "1024x768"
        };

        // Set initial resolution to 1280x720
        for (size_t i = 0; i < resolutions.size(); ++i) {
            if (resolutions[i] == "1280x720") {
                selectedResolutionIndex = static_cast<int>(i);
                break;
            }
        }
    }

    void ImGuiMain::resetSelections() {
        selectedRendererIndex = 0;
        selectedSceneIndex = 0;
        selectedModelIndices = { 0 };
        selectedModels = { "" };
        selectedScene = "";
        rayTracingEnabled = false;
        pbrEnabled = false;
        giEnabled = false;

        LOG(INFO) << "Main menu selections reset";
    }

    void ImGuiMain::applySettings() {
        LOG(INFO) << "Applied settings: Resolution=" << resolutions[selectedResolutionIndex]
            << ", RayTracing=" << (rayTracingEnabled ? "Enabled" : "Disabled")
            << ", PBR=" << (pbrEnabled ? "Enabled" : "Disabled")
            << ", GI=" << (giEnabled ? "Enabled" : "Disabled");
    }

    std::string ImGuiMain::getSelectedModel() const {
        if (selectedModels.size() > 1) {
            return "Multiple Models";
        }
        return selectedModels.empty() ? "None" : selectedModels[0];
    }

    void ImGuiMain::updateWindowResolution(GLFWwindow* window) {
        if (!window) {
            LOG(ERROR) << "Invalid GLFW window pointer";
            return;
        }

        std::string res = resolutions[selectedResolutionIndex];
        size_t xPos = res.find('x');
        if (xPos == std::string::npos) {
            LOG(ERROR) << "Invalid resolution format: " << res;
            return;
        }

        int width = std::stoi(res.substr(0, xPos));
        int height = std::stoi(res.substr(xPos + 1));
        glfwSetWindowSize(window, width, height);
        LOG(INFO) << "Window resized to: " << width << "x" << height;

        // Update ImGui display size
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
    }

    int ImGuiMain::renderMainMenu(GLFWwindow* window) {
        AnitoImGuiStyle anitoImGuiStyle;

        int selectedRenderer = 0; // -1 = None, 1 = BGFX, 2 = Diligent, etc.

        // Get GLFW window size
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        if (displaySize.x <= 0 || displaySize.y <= 0) {
            return -1;
        }

        // Calculate child sizes
        float footerHeight = displaySize.y * 0.25f;
        float mainChildHeight = displaySize.y - footerHeight;

        // Set ImGui window to full-screen
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always);
        ImGui::Begin("Anito 3D Benchmark", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        ImGui::BeginChild("MainContent", ImVec2(0, mainChildHeight), false);

        // Header
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(anitoImGuiStyle.getAccentGreen()));
        ImGui::SetCursorPosX((displaySize.x - ImGui::CalcTextSize(ICON_FA_CUBE " Anito 3D Benchmark").x) * 0.5f);
        ImGui::Text(ICON_FA_CUBE " Anito 3D Benchmark");
        ImGui::PopStyleColor();

        ImGui::SetCursorPosX((displaySize.x - ImGui::CalcTextSize("Version 1.0.0 | Powered by Vulkan").x) * 0.5f);
        ImGui::Text("Version 1.0.0 | Powered by Vulkan");

        // "X" button
        ImGui::SameLine(ImGui::GetWindowWidth() - 50);
        ImGui::SetCursorPosY(10);
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 50, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(220, 80, 80, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(180, 40, 40, 255));
        if (ImGui::Button(ICON_FA_TIMES)) {
            showExitPopup = true;
        }
        ImGui::PopStyleColor(3);
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Exit the application");
            ImGui::EndTooltip();
        }

        ImGui::Separator();

        // Two-column layout
        float padding = 15.0f;
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, displaySize.x * 0.5f);

        ImVec2 childSize = ImVec2(displaySize.x * 0.5f - 2 * padding, displaySize.y - ImGui::GetCursorPosY() - padding);
        ImGui::SetCursorPosX(padding);
        ImGui::BeginChild("Selections", childSize, false, ImGuiWindowFlags_NoBackground);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Renderer selection
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(anitoImGuiStyle.getAccentGreen()));
        ImGui::Text(ICON_FA_COGS " Renderer");
        ImGui::PopStyleColor();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
        std::vector<const char*> rendererItems(renderers.size());
        for (size_t i = 0; i < renderers.size(); ++i) {
            rendererItems[i] = renderers[i].c_str();
        }
        ImGui::SetNextItemWidth(displaySize.x * 0.4f);
        if (ImGui::Combo("##Renderer", &selectedRendererIndex, rendererItems.data(), rendererItems.size())) {
            LOG(INFO) << "Renderer selected: " << renderers[selectedRendererIndex];
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Select rendering engine (only BGFX implemented)");
            ImGui::EndTooltip();
        }

        ImGui::Dummy(ImVec2(0.0f, 15.0f));

        // Model selection (dynamic dropdowns)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(anitoImGuiStyle.getAccentGreen()));
        ImGui::Text(ICON_FA_CUBE " Models");
        ImGui::PopStyleColor();
        if (selectedModelIndices.size() != selectedModels.size()) {
            selectedModels.resize(selectedModelIndices.size(), "");
        }
        std::vector<const char*> modelItems(models.size());
        for (size_t i = 0; i < models.size(); ++i) {
            modelItems[i] = models[i].c_str();
        }
        float modelListMaxHeight = 3 * 60.0f;
        ImGui::BeginChild("ModelList", ImVec2(0, modelListMaxHeight), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

        if (selectedModelIndices.empty()) {
            selectedModelIndices.push_back(0);
            selectedModels.push_back("");
        }

        for (size_t i = 0; i < selectedModelIndices.size(); ++i) {
            ImGui::Dummy(ImVec2(0.0f, 8.0f));
            ImGui::PushID(static_cast<int>(i));
            ImGui::SetNextItemWidth(displaySize.x * 0.35f);
            if (ImGui::Combo("##Model", &selectedModelIndices[i], modelItems.data(), modelItems.size())) {
                selectedModels[i] = modelPath + models[selectedModelIndices[i]];
                LOG(INFO) << "Model " << i << " selected: " << selectedModels[i];
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Select 3D model for benchmarking");
                ImGui::EndTooltip();
            }

            if (i > 0) {
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                if (ImGui::Button(ICON_FA_TRASH, ImVec2(55, 40))) {
                    selectedModelIndices.erase(selectedModelIndices.begin() + i);
                    selectedModels.erase(selectedModels.begin() + i);
                    LOG(INFO) << "Removed model dropdown " << i;
                    ImGui::PopID();
                    continue;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Remove this model selection");
                    ImGui::EndTooltip();
                }
            }

            ImGui::PopID();
            ImGui::Spacing();
        }
        ImGui::EndChild();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::Button(ICON_FA_PLUS " Add Model", ImVec2(175, 40))) {
            selectedModelIndices.push_back(0);
            selectedModels.push_back("");
            LOG(INFO) << "Added new model dropdown";
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Add another model selection");
            ImGui::EndTooltip();
        }

        ImGui::Dummy(ImVec2(0.0f, 15.0f));

        // Scene selection
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(anitoImGuiStyle.getAccentGreen()));
        ImGui::Text(ICON_FA_IMAGE " Scene");
        ImGui::PopStyleColor();
        std::vector<const char*> sceneItems(scenes.size());
        for (size_t i = 0; i < scenes.size(); ++i) {
            sceneItems[i] = scenes[i].c_str();
        }
        ImGui::SetNextItemWidth(displaySize.x * 0.4f);
        if (ImGui::Combo("##Scene", &selectedSceneIndex, sceneItems.data(), sceneItems.size())) {
            selectedScene = scenes[selectedSceneIndex];
            LOG(INFO) << "Scene selected: " << selectedScene;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Select scene for benchmarking");
            ImGui::EndTooltip();
        }

        
        ImGui::PopStyleVar();
        ImGui::EndChild();

        ImGui::NextColumn();

        // Right column: Settings and Actions
        childSize = ImVec2(displaySize.x * 0.4f - 2 * padding, displaySize.y - ImGui::GetCursorPosY() - padding);
        ImGui::SetCursorPosX(displaySize.x * 0.6f + padding);
        ImGui::BeginChild("SettingsAndActions", childSize, false, ImGuiWindowFlags_NoBackground);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Settings
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(anitoImGuiStyle.getAccentGreen()));
        ImGui::Text(ICON_FA_COG " Settings");
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text(ICON_FA_DESKTOP " Window Resolution");
        std::vector<const char*> resolutionItems(resolutions.size());
        for (size_t i = 0; i < resolutions.size(); ++i) {
            resolutionItems[i] = resolutions[i].c_str();
        }
        ImGui::SetNextItemWidth(displaySize.x * 0.3f);
        if (ImGui::Combo("##Resolution", &selectedResolutionIndex, resolutionItems.data(), resolutionItems.size())) {
            LOG(INFO) << "Resolution selected: " << resolutions[selectedResolutionIndex];
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Select window resolution for benchmarking");
            ImGui::EndTooltip();
        }

        // Configurations sub-header
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text(ICON_FA_WRENCH " Configurations");
        ImGui::Checkbox("Enable Ray Tracing", &rayTracingEnabled);
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Enable real-time ray tracing (if supported)");
            ImGui::EndTooltip();
        }
        ImGui::Checkbox("Enable PBR", &pbrEnabled);
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Enable physically-based rendering");
            ImGui::EndTooltip();
        }
        ImGui::Checkbox("Enable Global Illumination", &giEnabled);
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Enable global illumination effects");
            ImGui::EndTooltip();
        }
        ImGui::EndChild();
        ImGui::EndChild();

        ImGui::Separator();

        ImGui::Dummy(ImVec2(0.0f, 3.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::Text(ICON_FA_INFO_CIRCLE " Status: Ready | Renderer: %s | Model: %s | Scene: %s | Resolution: %s",
            renderers[selectedRendererIndex].c_str(),
            getSelectedModel().c_str(),
            selectedScene.empty() ? "None" : selectedScene.c_str(),
            resolutions[selectedResolutionIndex].c_str());
        ImGui::PopStyleColor();

        // Launch and Reset
        ImGui::Dummy(ImVec2(0.0f, 25.0f));

        float buttonWidth = 200.0f;
        float spacing = 20.0f;
        float totalWidth = 2 * buttonWidth + spacing;
        float centerPosX = (displaySize.x - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(centerPosX);
        bool canLaunch = selectedRendererIndex > 0;
        if (!canLaunch) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        if (ImGui::Button(ICON_FA_PLAY " Launch", ImVec2(buttonWidth, 40)) && canLaunch) {
            applySettings();
            selectedRenderer = selectedRendererIndex;
            LOG(INFO) << "Launching renderer: " << renderers[selectedRendererIndex];
        }
        if (!canLaunch) ImGui::PopStyleVar();

        ImGui::SameLine();
        ImGui::SetCursorPosX(centerPosX + buttonWidth + spacing); // Right of Launch
        if (ImGui::Button(ICON_FA_REDO " Reset", ImVec2(buttonWidth, 40))) {
            resetSelections();
            LOG(INFO) << "Reset button clicked";
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Reset all selections");
            ImGui::EndTooltip();
        }

        // Exit confirmation popup
        ImGui::SetNextWindowPos(ImVec2(displaySize.x * 0.5f, displaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);

        if (showExitPopup) {
            ImGui::OpenPopup("Exit Confirmation");
            showExitPopup = false;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30)); // clean padding

        if (ImGui::BeginPopupModal("Exit Confirmation", nullptr, ImGuiWindowFlags_NoResize)) {

            // Center the text horizontally
            const char* text = "Exit the Application?";
            ImVec2 textSize = ImGui::CalcTextSize(text);
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
            ImGui::Text("%s", text);

            ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 25.0f));
            ImGui::Spacing(); ImGui::Spacing();

            // Center the buttons
            float buttonWidth = 120.0f;
            float spacing = 40.0f;
            float totalWidth = buttonWidth * 2 + spacing;
            float startX = (ImGui::GetWindowWidth() - totalWidth) * 0.5f;

            ImGui::SetCursorPosX(startX);
            if (ImGui::Button("Yes", ImVec2(buttonWidth, 50))) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                LOG(INFO) << "User confirmed application exit";
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine(0.0f, spacing);
            if (ImGui::Button("No", ImVec2(buttonWidth, 50))) {
                ImGui::CloseCurrentPopup();
                LOG(INFO) << "User canceled application exit";
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();
        ImGui::End();

        if (selectedRendererIndex == 0 && selectedRenderer != 0) {
            selectedRenderer = -1;
        }

        return selectedRenderer;
    }

}