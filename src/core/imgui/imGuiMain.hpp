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
        std::string getSelectedModel() const { return selectedModel; }
        std::string getSelectedScene() const { return selectedScene; }
        bool isRayTracingEnabled() const { return rayTracingEnabled; }
        bool isPbrEnabled() const { return pbrEnabled; }
        bool isGiEnabled() const { return giEnabled; }

        void resetSelections();

    private:
        bool showSettings;
        std::string selectedModel;
        std::string selectedScene;

        int selectedRendererIndex;
        int selectedModelIndex;
        int selectedSceneIndex;

        bool rayTracingEnabled;
        bool pbrEnabled;
        bool giEnabled;

        std::vector<std::string> renderers;
        std::vector<std::string> models;
        std::vector<std::string> scenes;
    };

}