#include "imGuiMain.hpp"
#include <ng-log/logging.h>
#include <IconsFontAwesome5.h>

namespace Anito3D {

    ImGuiMain::ImGuiMain() : showSettings(false), selectedModel(""), selectedScene(""),
        selectedRendererIndex(0), selectedModelIndex(0), selectedSceneIndex(0),
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
    }

    void ImGuiMain::resetSelections() {
        selectedRendererIndex = 0;
        selectedModelIndex = 0;
        selectedSceneIndex = 0;
        selectedModel = "";
        selectedScene = "";
        rayTracingEnabled = false;
        pbrEnabled = false;
        giEnabled = false;
        showSettings = false;
        LOG(INFO) << "Main menu selections reset";
    }

    int ImGuiMain::renderMainMenu() {
        int selectedRenderer = 0; // 0 = none, 1 = BGFX, 2 = Diligent, etc., -1 = "None"

        // Set window size and position
        ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
        ImGui::Begin("Anito 3D Benchmark", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

        // Header with "logo" (text-based with icon)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
        ImGui::Text(ICON_FA_CUBE " Anito 3D Benchmark Sandbox");
        ImGui::PopStyleColor();
        ImGui::Text("Version 1.0.0 | Powered by Vulkan");
        ImGui::Separator();

        // Two-column layout for selections
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, 350);

        // Renderer selection
        ImGui::Text(ICON_FA_DESKTOP " Renderer");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
        std::vector<const char*> rendererItems(renderers.size());
        for (size_t i = 0; i < renderers.size(); ++i) {
            rendererItems[i] = renderers[i].c_str();
        }
        ImGui::SetNextItemWidth(300);
        if (ImGui::Combo("##Renderer", &selectedRendererIndex, rendererItems.data(), rendererItems.size())) {
            LOG(INFO) << "Renderer selected: " << renderers[selectedRendererIndex];
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Select rendering engine (only BGFX implemented)");
            ImGui::EndTooltip();
        }
        ImGui::Text("Selected: %s", renderers[selectedRendererIndex].c_str());

        // Model selection
        ImGui::Text(ICON_FA_CUBE " Model");
        std::vector<const char*> modelItems(models.size());
        for (size_t i = 0; i < models.size(); ++i) {
            modelItems[i] = models[i].c_str();
        }
        ImGui::SetNextItemWidth(300);
        if (ImGui::Combo("##Model", &selectedModelIndex, modelItems.data(), modelItems.size())) {
            selectedModel = models[selectedModelIndex];
            LOG(INFO) << "Model selected: " << selectedModel;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Select 3D model for benchmarking");
            ImGui::EndTooltip();
        }
        ImGui::Text("Selected: %s", selectedModel.empty() ? "None" : selectedModel.c_str());

        // Scene selection
        ImGui::Text(ICON_FA_IMAGE " Scene");
        std::vector<const char*> sceneItems(scenes.size());
        for (size_t i = 0; i < scenes.size(); ++i) {
            sceneItems[i] = scenes[i].c_str();
        }
        ImGui::SetNextItemWidth(300);
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

        // Settings and actions
        ImGui::Text(ICON_FA_COG " Settings");
        if (ImGui::Button(ICON_FA_WRENCH " Configure", ImVec2(150, 40))) {
            showSettings = !showSettings;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Toggle benchmark settings");
            ImGui::EndTooltip();
        }

        // Launch button (disabled if "None" selected)
        bool canLaunch = selectedRendererIndex > 0; // Only BGFX implemented
        if (!canLaunch) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        if (ImGui::Button(ICON_FA_PLAY " Launch", ImVec2(150, 40)) && canLaunch) {
            selectedRenderer = selectedRendererIndex; // 1 = BGFX, 2 = Diligent, etc.
            LOG(INFO) << "Launching renderer: " << renderers[selectedRendererIndex];
        }
        if (ImGui::IsItemHovered() && !canLaunch) {
            ImGui::BeginTooltip();
            ImGui::Text("Select a renderer to launch");
            ImGui::EndTooltip();
        }
        if (!canLaunch) ImGui::PopStyleVar();

        // Reset button
        if (ImGui::Button(ICON_FA_REDO " Reset", ImVec2(150, 40))) {
            resetSelections();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Reset all selections");
            ImGui::EndTooltip();
        }

        ImGui::Columns(1);
        ImGui::PopStyleVar();

        // Settings window (modal popup)
        if (showSettings) {
            ImGui::OpenPopup("Benchmark Settings");
            ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Appearing);
            if (ImGui::BeginPopupModal("Benchmark Settings", &showSettings, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(ICON_FA_COGS " Benchmark Options");
                ImGui::Separator();
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
                ImGui::Separator();
                if (ImGui::Button(ICON_FA_TIMES " Close", ImVec2(120, 30))) {
                    showSettings = false;
                }
                ImGui::EndPopup();
            }
        }

        // Status bar
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), ICON_FA_INFO_CIRCLE " Status: Ready | Renderer: %s | Model: %s | Scene: %s",
            renderers[selectedRendererIndex].c_str(),
            selectedModel.empty() ? "None" : selectedModel.c_str(),
            selectedScene.empty() ? "None" : selectedScene.c_str());

        ImGui::End();

        // Return -1 if "None" is selected
        if (selectedRendererIndex == 0 && selectedRenderer != 0) {
            selectedRenderer = -1;
        }

        return selectedRenderer;
    }

}