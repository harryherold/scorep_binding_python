project(scorep_bindings)

cmake_minimum_required(VERSION 3.8)

set (CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Scorep REQUIRED)
find_package(PythonInterp)
find_package(PythonLibs)

add_library(scorep_bindings MODULE src/scorep.cpp)
set_target_properties(scorep_bindings PROPERTIES PREFIX "")
target_include_directories(scorep_bindings PRIVATE ${PYTHON_INCLUDE_PATH})
target_include_directories(scorep_bindings PRIVATE ${SCOREP_INCLUDE_DIRS})
target_link_libraries(scorep_bindings PRIVATE ${SCOREP_LIBRARIES})
link_directories(${SCOREP_LINK_DIRS})

set(INSTALL_DIR "lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages")

install(DIRECTORY scorep DESTINATION ${INSTALL_DIR})
install(TARGETS scorep_bindings DESTINATION ${INSTALL_DIR}/scorep)
