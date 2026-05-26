cmake_minimum_required(VERSION 3.15)
project(Disk) # 1. Proje adı "Disk" olarak güncellendi

# C++ 20 standardını zorunlu yapıyoruz
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. GLFW Kurulumu (Pencere Yönetimi)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 2. ImGui Kurulumu (UI Framework)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.5
)
FetchContent_MakeAvailable(imgui)

# 3. Proje Dosyalarını Tanımlama
# Executable hedef adı "Disk" yapıldı
add_executable(Disk 
    "src/main.cpp" 
    "src/engine.h" 
    "src/engine.cpp" 
    "src/ui.h" 
    "src/ui.cpp"
    "src/scanner.h"
    "src/scanner.cpp"
)

# ImGui kaynak dosyalarını "Disk" hedefine bağlıyoruz
target_sources(Disk PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Dosya Yollarını Bağlama (Include Paths)
target_include_directories(Disk PRIVATE 
    "src"
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
)

# Grafik kütüphanelerini ve GLFW'yi "Disk" hedefine bağlıyoruz
target_link_libraries(Disk PRIVATE glfw opengl32)