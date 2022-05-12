cmake_minimum_required(VERSION 3.14)
project(dia LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-g")
add_link_options("-L/usr/local/lib")
add_link_options( -lsfml-graphics -lsfml-window -lsfml-system)
include_directories(/usr/local/include)
add_executable(${PROJECT_NAME}
	src/CreateMode.cpp
	src/Defaults.cpp
	src/utils.cpp
	src/SetParser.cpp
	src/Drawer.cpp
	src/TextEdit.cpp
	src/EditMode.cpp
	src/CommandLine.cpp
	src/LoadSave.cpp
	src/main.cpp
	src/Selection.cpp
)
