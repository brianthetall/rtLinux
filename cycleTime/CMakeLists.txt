# CMakeLists.txt
cmake_minimum_required(VERSION 3.12)

# --------------------------------------------------
# 1. Project name and language
# --------------------------------------------------
project(CppRtInfinite LANGUAGES CXX)

# --------------------------------------------------
# 2. User must specify the target architecture
# --------------------------------------------------
# Usage examples:
#   cmake -DARCH=ARM64 ..
#   cmake -DARCH=X64 ..
if(NOT DEFINED ARCH)
    message(FATAL_ERROR "ARCH not defined. Pass -DARCH=ARM64 or -DARCH=X64")
endif()

# Validate the value
if(NOT ARCH MATCHES "^(ARM64|X64)$")
    message(FATAL_ERROR "ARCH must be either ARM64 or X64")
endif()

# --------------------------------------------------
# 3. Source files and output directory
# --------------------------------------------------
set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/rtInfinite.cpp   # same file compiled by build.sh [3]
)

# --------------------------------------------------
# 3. Create the executable target
# --------------------------------------------------
add_executable(cppRtInfinite ${SRC_FILES})

# Put the binary in ../bin just like the shell script
set_target_properties(cppRtInfinite
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin
)

# --------------------------------------------------
# 4. Architecture‑specific compile definitions
# --------------------------------------------------
if(ARCH STREQUAL "ARM64")
    target_compile_definitions(cppRtInfinite PRIVATE __ARM64__)
elseif(ARCH STREQUAL "X64")
    target_compile_definitions(cppRtInfinite PRIVATE __X64__)
endif()

# --------------------------------------------------
# 5. Include directories
# --------------------------------------------------
# If x64Timer/armTimer.h is located in the same folder as CMakeLists,
# we can expose it with an include directory.
target_include_directories(cppRtInfinite PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --------------------------------------------------
# 6. Link required libraries
# --------------------------------------------------
# The original build script linked pthread and math libraries [3]
target_link_libraries(cppRtInfinite PRIVATE pthread m)

# --------------------------------------------------
# 7. Optional: install rules
# --------------------------------------------------
install(TARGETS cppRtInfinite
        RUNTIME DESTINATION bin)
