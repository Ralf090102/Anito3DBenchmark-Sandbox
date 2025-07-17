function(import_vulkan)
    message(STATUS "=== [Vulkan] Importing Vulkan Start ===")

    find_package(Vulkan REQUIRED)

    message(STATUS "=== [Vulkan] Importing Vulkan Done ===")
endfunction()

function(import_vkbootstrap)
	message(STATUS "=== [vk-bootstrap] Importing vk-bootstrap Start ===")

	FetchContent_Declare(
        vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
        GIT_TAG v1.4.321
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(vk_bootstrap)
    set_target_properties(vk-bootstrap PROPERTIES POSITION_INDEPENDENT_CODE ON)

	message(STATUS "=== [vk-bootstrap] Importing vk-bootstrap Done ===")
endfunction()

function(import_vma)
	message(STATUS "=== [VMA] Importing VMA Start ===")

    FetchContent_Declare(
        VMA
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG v3.3.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(VMA)

	message(STATUS "=== [VMA] Importing VMA Done ===")
endfunction()

function(create_vulkan_library)
    message(STATUS "=== [Vulkan] Creating Vulkan Library Start ===")

    add_library(Anito3DVulkan STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMain.cpp
        ${PROJ_EXTERNAL_PATH}/imgui-src/imgui.cpp
        ${PROJ_EXTERNAL_PATH}/imgui-src/imgui_draw.cpp
        ${PROJ_EXTERNAL_PATH}/imgui-src/imgui_widgets.cpp
        ${PROJ_EXTERNAL_PATH}/imgui-src/imgui_tables.cpp
        ${PROJ_EXTERNAL_PATH}/imgui-src/backends/imgui_impl_glfw.cpp
        ${PROJ_EXTERNAL_PATH}/imgui-src/backends/imgui_impl_vulkan.cpp
    )
    target_include_directories(Anito3DVulkan PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${vk_bootstrap_SOURCE_DIR}
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )

    message(STATUS "=== [Vulkan] Creating Vulkan Library Done ===")
endfunction()