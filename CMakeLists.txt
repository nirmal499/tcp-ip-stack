cmake_minimum_required(VERSION 3.22.1)

project(tcp_ip_stack VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIBRARY_NAME "tcpip_lib")
set(EXECUTABLE_NAME "executable")

option(COMPILE_EXECUTABLE "Whether to compile the executable" ON)

add_subdirectory(src)

include(FetchContent)
FetchContent_Declare(
  cli
  GIT_REPOSITORY https://github.com/daniele77/cli.git
  GIT_TAG v2.1.0
)
FetchContent_MakeAvailable(cli)

# add_executable(main-project)
# target_link_libraries(main-project PRIVATE cli::cli)

if (COMPILE_EXECUTABLE)
	add_subdirectory(app)
else()
	# This message will be shown when we configure our project not when we build our project
	message("Without exe compiling")
endif()

