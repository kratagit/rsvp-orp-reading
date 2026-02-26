cmake_minimum_required(VERSION 3.14)
project(rsvp_reading C)

set(CMAKE_C_STANDARD 99)

include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG master   # <--- ZMIANA TUTAJ
)
FetchContent_MakeAvailable(raylib)

add_executable(main main.c)
target_link_libraries(main PRIVATE raylib)

# Kopiowanie pliku tekstowego do folderu budowania po każdej kompilacji
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/tekst.txt"
        "${CMAKE_CURRENT_BINARY_DIR}/tekst.txt"
)