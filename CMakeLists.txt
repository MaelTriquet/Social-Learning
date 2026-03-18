cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Output directories (bin and obj in project root)
# --------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj)

# --------------------------------------------------
# SFML via FetchContent
# --------------------------------------------------

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_MakeAvailable(SFML)

# --------------------------------------------------
# Eigen (header-only)
# --------------------------------------------------

find_package(Eigen3 REQUIRED NO_MODULE)

# --------------------------------------------------
# Collect all source files
# --------------------------------------------------

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/*.hpp
)

# --------------------------------------------------
# Create executable
# --------------------------------------------------

add_executable(main
    ${SRC_FILES}
    ${HEADER_FILES}
)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(main
    PRIVATE
        SFML::Graphics
        Eigen3::Eigen
)

# --------------------------------------------------
# Force object files into /obj
# --------------------------------------------------

set_target_properties(main PROPERTIES
    OBJECT_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj
)

# Optional (for VS users, harmless on Linux)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT main)

# Enable AddressSanitizer in Debug builds
target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>
)

target_link_options(main PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address>
)
