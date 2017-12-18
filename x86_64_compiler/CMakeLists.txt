file(GLOB_RECURSE X86_64_COMPILER_SRC ${CMAKE_SOURCE_DIR} *.cpp)

add_library(${X86_64_COMPILER} STATIC
    ${X86_64_COMPILER_SRC})

target_include_directories(${X86_64_COMPILER}
    PUBLIC include)
