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

# ==== Dependency Management ====
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/external)
file(MAKE_DIRECTORY ${FETCHCONTENT_BASE_DIR})

# Assimp is used for loading 3D models
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v6.0.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(assimp)
set_target_properties(assimp PROPERTIES EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)


# Big2 is a C++ library for 3D graphics and physics simulation
# BGFX, GLM, GLFW, IMGUI, SPDLOG
FetchContent_Declare(
    big2
    GIT_REPOSITORY https://github.com/Paper-Cranes-Ltd/big2-stack.git
    GIT_TAG v0.0.9
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(big2)


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