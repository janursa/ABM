CMAKE_MINIMUM_REQUIRED(VERSION 3.13.1)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(TARGET "ABM")
PROJECT(${TARGET} VERSION 1.0 LANGUAGES CXX)
SET(ABMLIB "ABM")
SET(APP "appABM")
SET(BINDSMODULE "myBinds")
SET(FUZZYLITE "fuzzylite")
SET(FUZZYLIB "fuzzy")
SET(DIFFUSIONLIB "diffusion")
SET(APPDIFFUSION "appDiffusion")
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    include(CTest)
    find_package(Doxygen)
    # if(Doxygen_FOUND)
    #     add_subdirectory(docs)
    # else()
    #     message(STATUS "Doxygen not found, not building docs")
    # endif()
endif()

include(FetchContent)
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()
# find_package (Python COMPONENTS Interpreter Development)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(pybind11)
# set (PYBIND11_PYTHON_VERSION "3.7")
set (PYTHON_EXECUTABLE $"/Users/matin/miniconda3/envs/v37_conda/bin/python")

include(FetchContent)

add_subdirectory(../CppyABM cppyabm)
add_subdirectory(src)
#add_subdirectory(app)
add_subdirectory(binds)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    #add_subdirectory(tests)
endif()


