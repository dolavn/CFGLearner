cmake_minimum_required(VERSION 3.12)
project(CFGLearner)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)
find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})
include_directories(soplex-4.0.2/src)
link_libraries(${ARMADILLO_LIBRARIES})
find_package(PythonLibs REQUIRED)
include_directories(include venv/include/site/python3.6/ /users/studs/bsc/2016/dolavn/CFGLearner/venv/include/site/python3.6 ${PYTHON_INCLUDE_DIRS})
link_libraries(${PYTHON_LIBRARIES})
add_subdirectory(tests)
add_subdirectory(soplex-4.0.2)
link_libraries(libsoplex)
add_subdirectory(pybind11)

SET (SRCS src/ParseTree.cpp
          src/TreeConstructor.cpp
          src/Sequence.cpp
          src/TreeAcceptor.cpp
          src/SimpleTeacher.cpp
          src/Learner.cpp
          src/CFG.cpp
          src/PythonModule.cpp
          src/FrequencyTeacher.cpp
          src/DifferenceTeacher.cpp
          src/TreeComparator.cpp
          src/MultiLinearMap.cpp
          src/IndexArray.cpp
          src/MultiplicityTreeAcceptor.cpp
          src/SimpleMultiplicityTeacher.cpp
          src/ObservationTable.cpp
          src/HankelMatrix.cpp
          src/TreesIterator.cpp
          src/FunctionalMultiplicityTeacher.cpp
          src/ConicCombinationFinder.cpp
          src/PositiveHankelMatrix.cpp
          src/Logger.cpp
          src/DifferenceMultiplicityTeacher.cpp
          src/ComparatorTeacher.cpp
          src/ProbabilityTeacher.cpp
          src/DuplicationsGenerator.cpp
          src/PartitionsIterator.cpp
          src/ConstructorGenerator.cpp
          src/ScalarHankelMatrix.cpp
        )


add_library(mainProg
            ${SRCS})

link_libraries(mainProg)
#add_executable(CFGLearnerExec
#               ${SRCS})
pybind11_add_module(CFGLearner src/PythonModule.cpp)