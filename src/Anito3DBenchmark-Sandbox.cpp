#include <iostream>
#include <filesystem>
#include <ng-log/logging.h>

#include "MeshEntity.hpp"

int main(int argc, char* argv[]) {
	// Set up logging directory and file
    std::filesystem::create_directories(ANITO3DSANDBOX_LOG_PATH);
    std::string logFile = std::string(ANITO3DSANDBOX_LOG_PATH) + "/Anito3DLog";

	// Initialize logging
    nglog::InitializeLogging(argv[0]);
    nglog::SetLogDestination(nglog::NGLOG_INFO, logFile.c_str());

	std::cout << "Hello, Anito3D Benchmark Sandbox!" << std::endl;
    LOG(INFO) << "Starting Anito3DBenchmark-Sandbox";
    
    // Use absolute path
    std::filesystem::path modelPath = std::filesystem::path(PROJ_ASSETS_DIR) / "models/3D/sphere.obj";
    Anito3D::MeshEntity meshEntity;
    if (meshEntity.LoadMesh(modelPath.string())) {
        LOG(INFO) << "Successfully loaded mesh with " << meshEntity.GetMeshData().vertices.size() << " vertices";
    }
    else {
        LOG(ERROR) << "Failed to load mesh";
        return 1;
    }

    return 0;
}