cmake_minimum_required(VERSION 3.12)
project(CFGLearner)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(tests)
add_library(mainProg
        src/ParseTree.cpp
        src/TreeConstructor.cpp
        src/Sequence.cpp)
include_directories(include)
add_executable(CFGLearner
        src/ParseTree.cpp
        src/TreeConstructor.cpp
        src/Sequence.cpp
        include/TreeAcceptor.h include/Teacher.h include/Learner.h)
