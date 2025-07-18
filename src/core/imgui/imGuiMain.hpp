#pragma once
#include <imgui.h>
#include <string>
#include <vector>

namespace Anito3D {

    class ImGuiMain {
    public:
        ImGuiMain();
        ~ImGuiMain() = default;

        // Render the main menu, returns selected renderer (0 = none, 1 = BGFX, 2 = Diligent, 3 = Falcor, etc.)
        int renderMainMenu();

        // Getters for selected model, scene, and benchmark settings
        std::string getSelectedModel() const;
        std::string getSelectedScene() const { return selectedScene; }
        std::string getSelectedResolution() const { return resolutions[selectedResolutionIndex]; }
        bool isRayTracingEnabled() const { return rayTracingEnabled; }
        bool isPbrEnabled() const { return pbrEnabled; }
        bool isGiEnabled() const { return giEnabled; }
        const std::vector<std::string>& getSelectedModelPaths() const { return selectedModels; }

        void resetSelections();
        void applySettings();

    private:
        bool showSettings;
        std::string selectedScene;
        std::vector<std::string> selectedModels;

        int selectedRendererIndex;
        int selectedSceneIndex;
        int selectedResolutionIndex;
        std::vector<int> selectedModelIndices;

        bool rayTracingEnabled;
        bool pbrEnabled;
        bool giEnabled;

        std::vector<std::string> renderers;
        std::vector<std::string> models;
        std::vector<std::string> scenes;
        std::vector<std::string> resolutions;
    };

}