#include <iostream>
#include <sstream>
#include "TreeComparator.h"
#include "Definitions.h"
#include "MultiplicityTreeAcceptor.h"
#include "MultiplicityTeacher.h"
#include "Logger.h"
#include "TreeConstructor.h"
#include "ColinearHankelMatrix.h"
#include <algorithm>
#include <vector>
#include <stack>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;


#define BRACKET_OPEN (-1)
#define BRACKET_CLOSE (-2)
#define IS_DIGIT(A) ((A)>='0' && (A)<='9')

using namespace std;

typedef vector<int> intVec;

MultiplicityTreeAcceptor learnColin(const MultiplicityTeacher&, ColinearHankelMatrix&);

static bool checkType(py::object& obj){
    py::object nltk = py::module::import("nltk");
    py::object nltkTree = nltk.attr("Tree");
    return py::isinstance(obj,nltkTree);
}

ParseTree* parseTree(std::string& str){
    str = str.substr(1,str.size()-2);
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    vector<int> seq;
    for(unsigned int i=0;i<str.size();i++){
        char c = str[i];
        if(c=='('){
            seq.push_back(BRACKET_OPEN);
            continue;
        }
        if(c==')'){
            seq.push_back(BRACKET_CLOSE);
            continue;
        }
        if(c!=' '){
            unsigned int m=i+1;
            while(IS_DIGIT(str[m])){m++;}
            int a = std::atoi(str.substr(i,m-i).c_str());
            seq.push_back(a);
            i=m-1;
        }
    }
    ParseTree* ans = nullptr;
    stack<ParseTree*> s;
    ans = new ParseTree(seq[0]);
    s.push(ans);
    for(unsigned int i=1;i<seq.size();++i){
        if(s.empty()){ //error
            goto error;
        }
        ParseTree* curr = s.top();
        if(seq[i]==BRACKET_OPEN){
            auto tree = new ParseTree(0);
            curr->setPointer(tree,curr->getChildrenNum());
            s.push(tree);
            continue;
        }
        if(seq[i]==BRACKET_CLOSE){
            s.pop();
            continue;
        }
        curr->setData(seq[i]);
    }
    return ans;
    error:
    if(ans) {
        delete (ans);
        ans=nullptr;
    }
    return ans;
}

namespace PythonModule{
    Logger::LoggingLevel verbosity = Logger::LOG_ERRORS;
}

PYBIND11_MODULE(CFGLearner, m) {
    py::class_<TreeComparator> treeComparator(m, "TreeComparator");
    py::class_<TreeAligner> treeAligner(m, "TreeAligner",treeComparator);
    treeAligner.def(py::init<int, int, int>());
    py::class_<DuplicationComparator> duplicationComparator(m, "DuplicationComparator", treeComparator);
    duplicationComparator.def(py::init<>());
    py::class_<SwapComparator> swapComparator(m, "SwapComparator", treeComparator);
    swapComparator.def(py::init<>());
    treeComparator.def("compare", [](TreeComparator& c, py::object nltkTree1, py::object nltkTree2){
        if(!checkType(nltkTree1) || !checkType(nltkTree2)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str1 = py::str(nltkTree1);
        string str2 = py::str(nltkTree2);
        ParseTree* tree1 = parseTree(str1);
        ParseTree* tree2 = parseTree(str2);
        float score = c.compare(*tree1, *tree2);
        delete(tree1);
        delete(tree2);
        return score;
    });
    py::class_<TreeConstructor> treeConstructor(m, "TreeConstructor");
    treeConstructor.def(py::init<std::map<std::vector<int>,float>>());
    treeConstructor.def("construct_tree",[](TreeConstructor& c, std::vector<int> v){
        Sequence seq(v);
        c.createTree(seq);
        ParseTree tree = c.getTree();
        py::object nltk = py::module::import("nltk");
        py::object nltkTree = nltk.attr("Tree");
        py::object nltkTreeFromString = nltkTree.attr("fromstring");
        return nltkTreeFromString(tree.getNltkTreeStr());

    });
    treeConstructor.def("set_lambda", [](TreeConstructor& c, float lambda){
        c.setLambda(lambda);
    });
    treeConstructor.def("set_concat", [](TreeConstructor& c, bool concat){
        c.setConcat(concat);
    });
    py::class_<MultiplicityTeacher> multiplicityTeacher(m, "MultiplicityTeacher");
    py::class_<SimpleMultiplicityTeacher> simpleMultiplicityTeacher(m, "SimpleMultiplicityTeacher",
                                                                    multiplicityTeacher);
    simpleMultiplicityTeacher.def(py::init<double,double>(),py::arg("epsilon")=0.1, py::arg("default_val")=-1);
    simpleMultiplicityTeacher.def("addExample",[](SimpleMultiplicityTeacher& t,py::object nltkTree,
                                                  double val){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        tree->setProb(val);
        t.addExample(*tree);
        delete(tree);
    });
    py::class_<DifferenceMultiplicityTeacher> differenceMultiplicityTeacher(m, "DifferenceMultiplicityTeacher",
                                                                            multiplicityTeacher);
    differenceMultiplicityTeacher.def(py::init<TreeComparator&,int,double,double>());
    differenceMultiplicityTeacher.def("addExample",[](DifferenceMultiplicityTeacher& t, py::object nltkTree,
                                                      double val){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        tree->setProb(val);
        t.addExample(*tree);
        delete(tree);
    });
    py::class_<ProbabilityTeacher> probabilityTeacher(m, "ProbabilityTeacher", multiplicityTeacher);
    probabilityTeacher.def(py::init<TreeComparator&,double,double>());
    probabilityTeacher.def("addExample",[](ProbabilityTeacher& t, py::object nltkTree, double val){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        tree->setProb(val);
        t.addExample(*tree);
        delete(tree);
    });
    probabilityTeacher.def("setup_duplications_generator",[](ProbabilityTeacher& t, int depth){
        t.setupDuplicationsGenerator(depth);
    });
    probabilityTeacher.def("setup_constructor_generator",[](ProbabilityTeacher& t, TreeConstructor& c, int maxLen,
                                                            int numTrees){
        t.setupConstructorGenerator(c, maxLen, numTrees);
    });
    py::enum_<Logger::LoggingLevel >(m, "LoggingLevel")
            .value("LOG_ERRORS", Logger::LoggingLevel::LOG_ERRORS)
            .value("LOG_WARNING", Logger::LoggingLevel::LOG_WARNING)
            .value("LOG_DETAILS", Logger::LoggingLevel::LOG_DETAILS)
            .value("LOG_DEBUG", Logger::LoggingLevel::LOG_DEBUG)
            .export_values();
    m.def("set_verbose",[](Logger::LoggingLevel lvl){
        //Logger::LoggingLevel logArr[] = {Logger::LOG_ERRORS, Logger::LOG_WARNING, Logger::LOG_DEBUG};
        Logger& log = Logger::getLogger();
        log.setPrintLevel(lvl);
    });
    m.def("learnMultPos",[](const MultiplicityTeacher& t) {
        py::gil_scoped_release release;
        ColinearHankelMatrix h(t);
        MultiplicityTreeAcceptor acc = learnColin(t, h);
        if(h.getZeroVecInd()!=-1){
            cout << h.getZeroVecInd() << endl;
            return acc.getAcceptorWithoutState(h.getZeroVecInd());
        }
        return acc;
    });
    py::class_<MultiLinearMap> multiLinearMap(m, "MultiLinearMap");
    multiLinearMap.def("get_param",[](const MultiLinearMap& m, vector<int> v){
        return m.getParam(v);
    });
    multiLinearMap.def("get_dim",[](const MultiLinearMap& m){
        return m.getVDim();
    });
    multiLinearMap.def("get_input_num",[](const MultiLinearMap& m){
        return m.getParamNum();
    });
    multiLinearMap.def("__repr__",[](const MultiLinearMap& m){
        stringstream stream;
        stream << "<MultiLinearMap with output_dim=";
        stream << m.getVDim();
        stream << " input_num=" << m.getParamNum() << ">";
        return stream.str();
    });
    py::class_<MultiplicityTreeAcceptor> multiplicityTreeAcceptor(m, "MultiplicityTreeAcceptor");
    multiplicityTreeAcceptor.def("get_lambda",[](MultiplicityTreeAcceptor& acc){
        return acc.getLambda();
    });
    multiplicityTreeAcceptor.def("get_map_terminal",[](MultiplicityTreeAcceptor& acc, int c){
        rankedChar r{c, 0};
        return acc.getMap(r);
    });
    multiplicityTreeAcceptor.def("get_map_non_terminal",[](MultiplicityTreeAcceptor& acc, int rank){
        rankedChar r{0, rank};
        return acc.getMap(r);
    });
    multiplicityTreeAcceptor.def("get_alphabet",[](MultiplicityTreeAcceptor& acc, int rank){
        std::vector<rankedChar> alphabet = acc.getAlphabet(rank);
        std::vector<int> ans;
        for(auto& rc: alphabet){
            ans.push_back(rc.c);
        }
        return ans;
    });
    multiplicityTreeAcceptor.def("get_ranks",[](MultiplicityTreeAcceptor& acc){
        return acc.getRanks();
    });
    multiplicityTreeAcceptor.def("run",[](MultiplicityTreeAcceptor& acc, py::object nltkTree){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        double ans = acc.run(*tree);
        delete(tree);
        return ans;
    });
    multiplicityTreeAcceptor.def("print_desc",[](MultiplicityTreeAcceptor& acc){
        return acc.printDesc();
    });
    multiplicityTreeAcceptor.def("get_dimension", [](MultiplicityTreeAcceptor& acc){
        return acc.getDim();
    });
    m.doc() = "A module used to learn probabilistic context free grammars from structural data.";
}
