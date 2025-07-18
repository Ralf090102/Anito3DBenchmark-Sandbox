cmake_minimum_required(VERSION 3.20)
project(Anito3DImGui LANGUAGES CXX)

add_library(Anito3DImGui STATIC
    "ImGuiMain.cpp"
    "AnitoImGuiStyle.cpp"
)

target_include_directories(Anito3DImGui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${PROJ_EXTERNAL_PATH}/iconfontcppheaders-src
)

target_link_libraries(Anito3DImGui PUBLIC
    imgui
    ng-log
)