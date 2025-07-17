cmake_minimum_required(VERSION 3.20)

project(Anito3DCore LANGUAGES CXX)

# Add subdirectories
add_subdirectory(imgui)
add_subdirectory(vulkan)

# Core library (to be linked by bgfx, ogre3D, diligentEngine)
add_library(Anito3DCore STATIC
    objects/Entity.cpp
    objects/MeshData.cpp
    objects/MeshEntity.cpp
)

# Include directories
target_include_directories(Anito3DCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/objects
    ${assimp_SOURCE_DIR}/include
    ${FETCHCONTENT_BASE_DIR}/glm-src
)

# Link dependencies
target_link_libraries(Anito3DCore PUBLIC
    Anito3DVulkan
    assimp::assimp
)