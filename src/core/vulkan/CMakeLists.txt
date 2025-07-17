cmake_minimum_required(VERSION 3.20)
project(Anito3DVulkan LANGUAGES CXX)

create_vulkan_library()

target_link_libraries(Anito3DVulkan PUBLIC
    Vulkan::Vulkan
    vk-bootstrap
    imgui
    glfw
    Anito3DImGui
    ng-log
)