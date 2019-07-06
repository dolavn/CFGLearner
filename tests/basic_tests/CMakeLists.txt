project(CFGLearner)
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
                MultiplicityTreeAcceptorTest.cpp)

target_link_libraries(runBasicTests gtest gtest_main)
target_link_libraries(runBasicTests mainProg)