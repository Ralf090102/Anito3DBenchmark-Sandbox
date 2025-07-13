# Add subdirectories
add_subdirectory(core)
add_subdirectory(bgfx)
add_subdirectory(ogre3D)
add_subdirectory(diligentEngine)

set(SandboxMainExecutable Anito3DBenchmarkSandbox)
add_executable (${SandboxMainExecutable} "Anito3DBenchmark-Sandbox.cpp")

# Link dependencies to the executable
target_link_libraries(${SandboxMainExecutable} PRIVATE
	assimp::assimp
    big2
)

# Include directories for the executable
target_include_directories(${SandboxMainExecutable} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_SOURCE_DIR}/src/bgfx/include
    ${CMAKE_SOURCE_DIR}/src/ogre3D/include
    ${CMAKE_SOURCE_DIR}/src/diligentEngine/include
)