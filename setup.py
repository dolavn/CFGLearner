from setuptools import setup, Extension

setup(name="CFGLearner",
      version="0.1",
      ext_modules=[Extension("CFGLearner", ["src/Learner.cpp", "src/PythonModule.cpp",
                                        "src/ParseTree.cpp", "src/Sequence.cpp",
                                        "src/SimpleTeacher.cpp", "src/TreeAcceptor.cpp",
                                        "src/TreeConstructor.cpp"], extra_compile_args=['-std=c++11'],
                                        include_dirs=["include"])]
      );
