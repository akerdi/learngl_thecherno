cmake_minimum_required(VERSION 2.8)

project(Learngl)
set(Learngl_VERSION_MAJOR 1)
set(Learngl_VERSION_MINOR 0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 引入头文件
include_directories("/usr/local/Cellar/glfw/3.3.4/include")
include_directories("/usr/local/Cellar/glew/2.2.0_1/include")

aux_source_directory(. DIR_SRCS)

add_executable(Learngl ${DIR_SRCS})

# 导入glew lib
# 两种方法都可以
# 方法一，使用find_package, 然后直接link GLEW::GLEW
# find_package(GLEW REQUIRED)
# target_link_libraries(Learngl GLEW::GLEW)
# 方法二，直接link libglew, 然后补足缺失的opengl
link_directories("/usr/local/Cellar/glew/2.2.0_1/lib")
target_link_libraries(Learngl GLEW)
target_link_libraries(Learngl "-framework OpenGL")

# 导入glfw lib
link_directories("/usr/local/Cellar/glfw/3.3.4/lib")
target_link_libraries(Learngl glfw)
