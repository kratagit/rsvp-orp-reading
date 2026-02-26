set(CMAKE_SYSTEM_NAME Windows)

# Kompilatory MinGW
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Gdzie szukać bibliotek
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# Szukaj programów w środowisku hosta (Linux), a bibliotek w środowisku docelowym (Windows)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)