cmake_minimum_required (VERSION 3.8)
project("Anito3DBenchmarkSandbox" LANGUAGES CXX)

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

# ==== Output directories ====
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ==== Add source directories ====
add_subdirectory(src)

add_executable (Anito3DBenchmark-Sandbox "Anito3DBenchmark-Sandbox.cpp" "Anito3DBenchmark-Sandbox.h")

# ==== Platform-specific options ====
if (UNIX)
	target_compile_definitions(Anito3DBenchmark-Sandbox PRIVATE PLATFORM_UNIX)
elseif (WIN32)
	target_compile_definitions(Anito3DBenchmark-Sandbox PRIVATE PLATFORM_WINDOWS)
endif()