cmake_minimum_required(VERSION 3.16)
project(cuda_test CUDA)

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xptxas -O3,-v")

add_executable(cuda_test main.cu cuda_bfs.h)

set_target_properties(
        cuda_test
        PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)


if (MSVC)
    if (NOT (MSVC_VERSION LESS 1900))  # 1900 is Visual Studio 2015.
        # Suppress warning C4819 for none-English environment.
        # see https://github.com/google/googletest/pull/1606#issuecomment-401526516
        #        set(cxx_base_flags "${cxx_base_flags} /wd4819")
        #        set(cxx_base_flags "${cxx_base_flags} -utf-8")
        #        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4819")
    endif ()
endif ()

if (APPLE)
    # We need to add the path to the driver (libcuda.dylib) as an rpath,
    # so that the static cuda runtime can find it at runtime.
    set_property(TARGET cuda_test
            PROPERTY
            BUILD_RPATH ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})
endif ()