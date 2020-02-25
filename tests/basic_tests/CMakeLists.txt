project(CFGLearner)
find_package(PythonLibs REQUIRED)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})



add_executable(runBasicTests
                AcceptorTest.cpp
                TreeTest.cpp
                SimpleTeacherTest.cpp
                LearnerTest.cpp
                CFGTest.cpp
                FrequencyTeacherTest.cpp
                DifferenceTeacherTest.cpp
                FunctionalTeacherTest.cpp
                TreeComparatorTest.cpp
                IndexArrayTest.cpp
                MultiLinearMapTest.cpp
                MultiplicityTreeAcceptorTest.cpp
                MultiplicityTeacherTest.cpp
                HankelMatrixTest.cpp
                TreesIteratorTest.cpp
                ConicCombinationFinderTest.cpp
                PositiveHankelMatrixTest.cpp
                DifferenceMultiplicityTeacherTest.cpp
                TestHelperFunctions.cpp
                TreeConstructorTest.cpp
                ProbabilityTeacherTest.cpp)


include_directories(include ${PYTHON_INCLUDE_DIRS})
target_link_libraries(runBasicTests ${PYTHON_LIBRARIES})
target_link_libraries(runBasicTests gtest gtest_main)
target_link_libraries(runBasicTests mainProg)