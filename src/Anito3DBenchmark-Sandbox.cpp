#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <ng-log/logging.h>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "vulkanMain.hpp"
#include "MeshEntity.hpp"
#include "ImGuiMain.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

void loadModel(const std::string& modelPath);
void loadModels(const std::vector<std::string>& modelPaths);
void glfwErrorCallback(int error, const char* description);

int main(int argc, char* argv[]) {
	// Set up logging directory and file
    std::filesystem::create_directories(ANITO3DSANDBOX_LOG_PATH);
    std::string logFile = std::string(ANITO3DSANDBOX_LOG_PATH) + "/Anito3DLog";

	// Initialize logging
    nglog::InitializeLogging(argv[0]);
    nglog::SetLogDestination(nglog::NGLOG_INFO, logFile.c_str());

	std::cout << "Hello, Anito3D Benchmark Sandbox!" << std::endl;
    LOG(INFO) << "Starting Anito3DBenchmark-Sandbox";

    // Initialize GLFW
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        LOG(ERROR) << "Failed to initialize GLFW";
        return 1;
    }

    while (true) {
        // Create main menu window
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(1280, 720, "Anito3D Benchmark Main Menu", nullptr, nullptr);
        if (!window) {
            LOG(ERROR) << "Failed to create main menu window";
            glfwTerminate();
            return 1;
        }
        LOG(INFO) << "Main menu window created successfully";

        // Initialize VulkanMain
        Anito3D::VulkanMain vulkanMain;
        try {
            if (!vulkanMain.init(window, 1280, 720)) {
                LOG(ERROR) << "Failed to initialize VulkanMain";
                glfwDestroyWindow(window);
                continue; // Retry main menu
            }
        }
        catch (const std::exception& e) {
            LOG(ERROR) << "VulkanMain initialization exception: " << e.what();
            glfwDestroyWindow(window);
            continue;
        }
        LOG(INFO) << "Vulkan initialized successfully";

        // Run main menu and get selected renderer
        Anito3D::ImGuiMain imguiMain; // Create ImGuiMain instance to access getters
        int selectedRenderer = 0;
        try {
            selectedRenderer = vulkanMain.runMainMenu(window);
        }
        catch (const std::exception& e) {
            LOG(ERROR) << "VulkanMain runMainMenu exception: " << e.what();
        }

        // Switch to selected renderer
        if (selectedRenderer == 1) { // BGFX
            loadModels(imguiMain.getSelectedModelPaths());
            // runBgfxRenderer(ImGui::GetCurrentContext());
            LOG(INFO) << "Returned from BGFX renderer";
        }
        else if (selectedRenderer == -1) { // None
            LOG(INFO) << "No renderer selected, returning to main menu";
        }
        else if (selectedRenderer == 0) { // Window closed
            break;
        }
        // Add cases for Diligent (2), Falcor (3), etc., later

        // Cleanup main menu
        vulkanMain.cleanup();
        glfwDestroyWindow(window);

    }


    glfwTerminate();
    LOG(INFO) << "Anito3DBenchmark-Sandbox terminated";

    return 0;
}

void loadModel(const std::string& modelPath) {
    Anito3D::MeshEntity meshEntity;
    if (meshEntity.LoadMesh(modelPath)) {
        LOG(INFO) << "Successfully loaded mesh with " << meshEntity.GetMeshData().vertices.size() << " vertices";
    }
    else {
        LOG(ERROR) << "Failed to load mesh from " << modelPath;
    }
}

void loadModels(const std::vector<std::string>& modelPaths) {
    for (const auto& modelPath : modelPaths) {
        loadModel(modelPath);
    }
}

void glfwErrorCallback(int error, const char* description) {
    LOG(ERROR) << "GLFW Error (" << error << "): " << description;
}
