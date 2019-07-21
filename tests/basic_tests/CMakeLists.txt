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
                TreeComparatorTest.cpp
                IndexArrayTest.cpp
                MultiLinearMapTest.cpp
                MultiplicityTreeAcceptorTest.cpp
                MatrixTest.cpp)


include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(runBasicTests ${PYTHON_LIBRARIES})
target_link_libraries(runBasicTests gtest gtest_main)
target_link_libraries(runBasicTests mainProg)