cmake_minimum_required(VERSION 3.20)
project(PlanetaryTerrainGeneration)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies -----------------------------------------------------------------

include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        origin/master
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLEW
FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG        origin/master
)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ONLY_LIBS               ON  CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glew)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        origin/master
)
FetchContent_MakeAvailable(glm)

# FastNoiseLite
FetchContent_Declare(
    FastNoiseLite
    GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite.git
    GIT_TAG        origin/master
)
FetchContent_MakeAvailable(FastNoiseLite)

# nlohmann/json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.9
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw opengl32)

# stb
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# Sources ----------------------------------------------------------------------

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/PlanetaryTerrainGeneration")

add_executable(${PROJECT_NAME}
    ${SRC_DIR}/Main.cpp
    ${SRC_DIR}/Camera.cpp
    ${SRC_DIR}/Renderer.cpp
    ${SRC_DIR}/Planet.cpp
    ${SRC_DIR}/CubeFace.cpp
    ${SRC_DIR}/PostProcessor.cpp
    ${SRC_DIR}/PaletteManager.cpp
    ${SRC_DIR}/PaletteGenerator.cpp
    ${SRC_DIR}/Randomiser.cpp
    ${SRC_DIR}/SurfaceGenerator.cpp
    ${SRC_DIR}/GUI.cpp
)

if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}
    ${fastnoiselite_SOURCE_DIR}/Cpp
    ${stb_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    libglew_static
    glm::glm
    imgui
    opengl32
    nlohmann_json::nlohmann_json
)

# Shaders ----------------------------------------------------------------------

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SRC_DIR}/main.frag"
        "${SRC_DIR}/main.vert"
	"${SRC_DIR}/pixelation.frag"
        "${SRC_DIR}/pixelation.vert"
	"${SRC_DIR}/background.frag"
        "${SRC_DIR}/background.vert"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copying shaders..."
)

# Assets -----------------------------------------------------------------------

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SRC_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying assets..."
)

# Release flags ----------------------------------------------------------------

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:/O2 /GL>
)
target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:/LTCG>
)