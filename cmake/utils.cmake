#Assimp
function(import_assimp)
    message(STATUS "=== [Assimp] Importing Assimp Start ===")

    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v6.0.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(assimp)
    set_target_properties(assimp PROPERTIES EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

    message(STATUS "=== [Assimp] Importing Assimp Done ===")

endfunction()

function(link_assimp)
    message(STATUS "=== [Assimp] Linking AssimpStart  ===")

    target_include_directories(${SandboxMainExecutable} PRIVATE ${assimp_SOURCE_DIR}/include)
    target_link_libraries(${SandboxMainExecutable} PUBLIC assimp)

    message(STATUS "=== [Assimp] Linking Assimp Done ===")
endfunction()

# TheBig2
function(import_big2)
    message(STATUS "=== [BGFX] Importing BIG2 Stack Start ===")

    set (BGFX_RENDERER_SRC "${PROJ_SRC_PATH}/bgfx" CACHE PATH "BGFX Files SRC" FORCE)

    FetchContent_Declare(
        big2
        GIT_REPOSITORY https://github.com/Paper-Cranes-Ltd/big2-stack.git
        GIT_TAG v0.0.9
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(big2)


    message(STATUS "=== [BGFX] Importing BIG2 Stack Done ===")
endfunction()

function(link_bgfx_big2)
    message(STATUS "=== [BGFX] Linking BIG2 BGFX Start ===")

    target_link_libraries(${SandboxMainExecutable} PUBLIC BIG2)
    add_shaders_directory("${PROJ_ASSETS_PATH}/shaders/bgfx" BGFX_SHADERS_TARGET_NAME)

    target_link_libraries(${SandboxMainExecutable} PUBLIC "${BGFX_SHADERS_TARGET_NAME}")

    message(STATUS "=== [BGFX] Linking BIG2 BGFX Done ===")
endfunction()


# NG-Log Logger
function(import_nglog)
    message(STATUS "=== [NG-Log] Importing NG-Log Start ===")    

    FetchContent_Declare(
        ng-log
        GIT_REPOSITORY https://github.com/ng-log/ng-log.git
        GIT_TAG v0.8.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(ng-log)
    set_target_properties(ng-log PROPERTIES EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

    message(STATUS "=== [NG-Log] Importing NG-Log Done ===")    
endfunction()

function(link_nglog)
    message(STATUS "=== [NG-Log] Linking NG-Log Start ===")    

    target_link_libraries (${SandboxMainExecutable} PUBLIC ng-log)
    set(ANITO3DSANDBOX_LOG_PATH "${CMAKE_CURRENT_BINARY_DIR}/Anito3DSandboxLogs")

    target_compile_definitions (${SandboxMainExecutable} PUBLIC ANITO3DSANDBOX_LOG_PATH=\"${ANITO3DSANDBOX_LOG_PATH}\")
    file(MAKE_DIRECTORY "${ANITO3DSANDBOX_LOG_PATH}")

    message(STATUS "=== [NG-Log] Linking NG-Log Done ===")    
endfunction()