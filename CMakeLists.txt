cmake_minimum_required(VERSION 3.12)
project(CFGLearner)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)

find_package(PythonLibs REQUIRED)

add_subdirectory(tests)

SET (SRCS src/ParseTree.cpp
          src/TreeConstructor.cpp
          src/Sequence.cpp
          src/TreeAcceptor.cpp
          src/SimpleTeacher.cpp
          src/Learner.cpp
          src/CFG.cpp
          src/PythonModule.cpp)

add_library(mainProg
        ${SRCS})
include_directories(include venv/include/site/python3.6/ ${PYTHON_INCLUDE_DIRS})
add_executable(CFGLearner
        ${SRCS})
