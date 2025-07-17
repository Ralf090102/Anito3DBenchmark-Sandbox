# Add subdirectories
add_subdirectory(core)
add_subdirectory(bgfx)
add_subdirectory(ogre3D)
add_subdirectory(diligentEngine)

add_executable (${SandboxMainExecutable} ${CMAKE_CURRENT_SOURCE_DIR}/Anito3DBenchmark-Sandbox.cpp)

# Link dependencies to the executable
link_assimp()
link_bgfx_big2()
link_nglog()

target_link_libraries(${SandboxMainExecutable} PRIVATE
    Anito3DCore
)

# Include directories for the executable
target_include_directories(${SandboxMainExecutable} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_SOURCE_DIR}/src/core/objects
    ${CMAKE_SOURCE_DIR}/src/core/vulkan
    ${CMAKE_SOURCE_DIR}/src/bgfx/include
    ${CMAKE_SOURCE_DIR}/src/ogre3D/include
    ${CMAKE_SOURCE_DIR}/src/diligentEngine/include
)