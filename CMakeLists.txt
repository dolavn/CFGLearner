cmake_minimum_required(VERSION 3.12)
project(CFGLearner)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)
find_package(PythonLibs 3 REQUIRED)
include_directories(include pybind11/include ${PYTHON_INCLUDE_DIRS})
link_libraries(${PYTHON_LIBRARIES})
add_subdirectory(pybind11)

SET (SRCS src/ParseTree.cpp
          src/TreeConstructor.cpp
          src/Sequence.cpp
          src/TreeComparator.cpp
          src/MultiLinearMap.cpp
          src/IndexArray.cpp
          src/MultiplicityTreeAcceptor.cpp
          src/SimpleMultiplicityTeacher.cpp
          src/TreesIterator.cpp
          src/FunctionalMultiplicityTeacher.cpp
          src/Learner.cpp
          src/ComparatorTeacher.cpp
          src/Logger.cpp
          src/DifferenceMultiplicityTeacher.cpp
          src/ProbabilityTeacher.cpp
          src/DuplicationsGenerator.cpp
          src/PartitionsIterator.cpp
          src/ConstructorGenerator.cpp
          src/ColinearHankelMatrix.cpp
          src/PythonModule.cpp
        )


add_library(mainProg
            ${SRCS})

link_libraries(mainProg)
pybind11_add_module(CFGLearner src/PythonModule.cpp)
