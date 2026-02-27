cmake_minimum_required(VERSION 3.14)
# Enable ASM language support
project(rsvp_reading C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -masm=intel") # Use Intel syntax

include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

# Add rsvp_logic.s to the executable
add_executable(main main.c rsvp_logic.s)
target_link_libraries(main PRIVATE raylib)

# Copy text file to build directory after build
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/tekst.txt"
        "${CMAKE_CURRENT_BINARY_DIR}/tekst.txt"
)