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
            "models/3D/sphere.obj",
            "models/3D/cube.obj",
            "models/3D/bunny.obj"
        };

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
    }

    void ImGuiMain::resetSelections() {
        selectedRendererIndex = 0;
        selectedSceneIndex = 0;
        selectedResolutionIndex = 0;
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


    int ImGuiMain::renderMainMenu() {
        int selectedRenderer = 0; // -1 = None, 1 = BGFX, 2 = Diligent, etc.

        // Get GLFW window size
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        // Set ImGui window to full-screen
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always);
        ImGui::Begin("Anito 3D Benchmark", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        // Header
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
        ImGui::SetCursorPosX((displaySize.x - ImGui::CalcTextSize(ICON_FA_CUBE " Anito 3D Benchmark Sandbox").x) * 0.5f);
        ImGui::Text(ICON_FA_CUBE " Anito 3D Benchmark Sandbox");
        ImGui::PopStyleColor();
        ImGui::SetCursorPosX((displaySize.x - ImGui::CalcTextSize("Version 1.0.0 | Powered by Vulkan").x) * 0.5f);
        ImGui::Text("Version 1.0.0 | Powered by Vulkan");
        ImGui::Separator();

        // Two-column layout for selections
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, displaySize.x * 0.6f); // 60% for selections

        // Renderer selection
        ImGui::Text(ICON_FA_DESKTOP " Renderer");
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
        ImGui::Text("Selected: %s", renderers[selectedRendererIndex].c_str());

        // Model selection (dynamic dropdowns)
        ImGui::Text(ICON_FA_CUBE " Models");
        if (selectedModelIndices.size() != selectedModels.size()) {
            selectedModels.resize(selectedModelIndices.size(), "");
        }
        std::vector<const char*> modelItems(models.size());
        for (size_t i = 0; i < models.size(); ++i) {
            modelItems[i] = models[i].c_str();
        }
        for (size_t i = 0; i < selectedModelIndices.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            ImGui::SetNextItemWidth(displaySize.x * 0.35f);
            if (ImGui::Combo("##Model", &selectedModelIndices[i], modelItems.data(), modelItems.size())) {
                selectedModels[i] = models[selectedModelIndices[i]];
                LOG(INFO) << "Model " << i << " selected: " << selectedModels[i];
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Select 3D model for benchmarking");
                ImGui::EndTooltip();
            }
            if (i > 0) {
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove", ImVec2(80, 20))) {
                    selectedModelIndices.erase(selectedModelIndices.begin() + i);
                    selectedModels.erase(selectedModels.begin() + i);
                    LOG(INFO) << "Removed model dropdown " << i;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Remove this model selection");
                    ImGui::EndTooltip();
                }
            }
            ImGui::PopID();
        }
        if (ImGui::Button(ICON_FA_PLUS " Add Model", ImVec2(150, 30))) {
            selectedModelIndices.push_back(0);
            selectedModels.push_back("");
            LOG(INFO) << "Added new model dropdown";
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Add another model selection");
            ImGui::EndTooltip();
        }
        ImGui::Text("Selected: %s", getSelectedModel().c_str());

        // Scene selection
        ImGui::Text(ICON_FA_IMAGE " Scene");
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
        ImGui::Text("Selected: %s", selectedScene.empty() ? "None" : selectedScene.c_str());

        ImGui::NextColumn();

        // Right column: Collapsing headers
        ImGui::SetColumnWidth(1, displaySize.x * 0.4f); // 40% for headers

        // Settings header
        if (ImGui::CollapsingHeader(ICON_FA_COG " Settings")) {
            ImGui::TextWrapped("Configure benchmarking options and display settings.");
            ImGui::Separator();

            // Resolution dropdown
            ImGui::Text("Screen Resolution");
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
                ImGui::Text("Select screen resolution for benchmarking");
                ImGui::EndTooltip();
            }

            // Benchmark options
            ImGui::Text("Benchmark Options");
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

            // Apply button
            ImGui::Separator();
            ImGui::SetCursorPosX((ImGui::GetColumnWidth() - 150) * 0.5f);
            if (ImGui::Button(ICON_FA_CHECK " Apply", ImVec2(150, 30))) {
                applySettings();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Apply benchmark settings");
                ImGui::EndTooltip();
            }
        }

        // Reset header
        if (ImGui::CollapsingHeader(ICON_FA_REDO " Reset")) {
            ImGui::TextWrapped("Reset all selections to their default values.");
            ImGui::Separator();
            ImGui::Text("Current Configuration:");
            ImGui::BulletText("Renderer: %s", renderers[selectedRendererIndex].c_str());
            ImGui::BulletText("Model: %s", getSelectedModel().c_str());
            ImGui::BulletText("Scene: %s", selectedScene.empty() ? "None" : selectedScene.c_str());
            ImGui::BulletText("Resolution: %s", resolutions[selectedResolutionIndex].c_str());
            ImGui::BulletText("Ray Tracing: %s", rayTracingEnabled ? "Enabled" : "Disabled");
            ImGui::BulletText("PBR: %s", pbrEnabled ? "Enabled" : "Disabled");
            ImGui::BulletText("Global Illumination: %s", giEnabled ? "Enabled" : "Disabled");
            ImGui::Separator();
            ImGui::SetCursorPosX((ImGui::GetColumnWidth() - 150) * 0.5f);
            if (ImGui::Button(ICON_FA_REDO " Reset", ImVec2(150, 30))) {
                resetSelections();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Reset all selections");
                ImGui::EndTooltip();
            }
        }

        // Launch header
        if (ImGui::CollapsingHeader(ICON_FA_PLAY " Launch")) {
            ImGui::TextWrapped("Launch the selected renderer with the current configuration.");
            ImGui::Separator();
            ImGui::Text("Current Configuration:");
            ImGui::BulletText("Renderer: %s", renderers[selectedRendererIndex].c_str());
            ImGui::BulletText("Model: %s", getSelectedModel().c_str());
            ImGui::BulletText("Scene: %s", selectedScene.empty() ? "None" : selectedScene.c_str());
            ImGui::BulletText("Resolution: %s", resolutions[selectedResolutionIndex].c_str());
            ImGui::BulletText("Ray Tracing: %s", rayTracingEnabled ? "Enabled" : "Disabled");
            ImGui::BulletText("PBR: %s", pbrEnabled ? "Enabled" : "Disabled");
            ImGui::BulletText("Global Illumination: %s", giEnabled ? "Enabled" : "Disabled");
            ImGui::Separator();
            bool canLaunch = selectedRendererIndex > 0;
            ImGui::SetCursorPosX((ImGui::GetColumnWidth() - 150) * 0.5f);
            if (!canLaunch) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            if (ImGui::Button(ICON_FA_PLAY " Launch", ImVec2(150, 30)) && canLaunch) {
                selectedRenderer = selectedRendererIndex; // 1 = BGFX, 2 = Diligent, etc.
                LOG(INFO) << "Launching renderer: " << renderers[selectedRendererIndex];
            }
            if (ImGui::IsItemHovered() && !canLaunch) {
                ImGui::BeginTooltip();
                ImGui::Text("Select a renderer to launch");
                ImGui::EndTooltip();
            }
            if (!canLaunch) ImGui::PopStyleVar();
        }

        ImGui::Columns(1);
        ImGui::PopStyleVar();

        // Status bar (bottom of screen)
        ImGui::SetCursorPosY(displaySize.y - ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().WindowPadding.y);
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), ICON_FA_INFO_CIRCLE " Status: Ready | Renderer: %s | Model: %s | Scene: %s",
            renderers[selectedRendererIndex].c_str(),
            getSelectedModel().c_str(),
            selectedScene.empty() ? "None" : selectedScene.c_str());

        ImGui::End();

        // Return -1 if "None" renderer is selected
        if (selectedRendererIndex == 0 && selectedRenderer != 0) {
            selectedRenderer = -1;
        }

        return selectedRenderer;
    }

}