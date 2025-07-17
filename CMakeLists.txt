cmake_minimum_required (VERSION 3.20)
project("Anito3DBenchmarkSandbox" LANGUAGES CXX)
set(SandboxMainExecutable Anito3DBenchmarkSandbox)

# ==== Compiler settings ====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==== MSVC: Hot Reload & Debug Info ====
if (MSVC)
	if (POLICY CMP0141)
		cmake_policy(SET CMP0141 NEW)
	endif()

	# Use Edit and Continue in Debug, PDB otherwise
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
	"$<IF:$<CONFIG:Debug,RelWithDebInfo>,EditAndContinue,ProgramDatabase>")
endif()

# ==== Build type ====
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ==== Project paths ====
set(PROJ_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "Project SRC" FORCE)
set(PROJ_ASSETS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets" CACHE PATH "Project ASSETS" FORCE)
set(PROJ_TESTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tests" CACHE PATH "Project TESTS" FORCE)
set(PROJ_EXTERNAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external" CACHE PATH "Project EXTERNAL" FORCE)

add_compile_definitions(PROJ_SRC_DIR="${CMAKE_SOURCE_DIR}/src")
add_compile_definitions(PROJ_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets")
add_compile_definitions(PROJ_TESTS_DIR="${CMAKE_SOURCE_DIR}/tests")

# ==== Dependency Management ====
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/external)
file(MAKE_DIRECTORY ${FETCHCONTENT_BASE_DIR})

include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/vulkan.cmake)

# Assimp is used for loading 3D models
import_assimp()

# Big2 is a C++ library for 3D graphics and physics simulation
# BGFX, GLM, GLFW, IMGUI, SPDLOG
import_big2()

# NG-Log is a lightweight logging library
import_nglog()

# Import Vulkan and vk-bootstrap for Vulkan API support
import_vulkan()
import_vkbootstrap()

# Import IconFontCppHeaders
import_IconFontCppHeaders()

# ==== Output directories ====
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ==== Add source directories ====
add_subdirectory(src)
add_subdirectory(tests)

# ==== Platform-specific options ====
if(UNIX)
    add_compile_definitions(PLATFORM_UNIX)
elseif(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
endif()