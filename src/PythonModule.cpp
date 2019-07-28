#include <iostream>
#include <sstream>
#include "Teacher.h"
#include "Learner.h"
#include "TreeAcceptor.h"
#include "CFG.h"
#include "TreeComparator.h"
#include "Definitions.h"
#include "Matrix.h"
#include <algorithm>
#include <vector>
#include <stack>
#include <armadillo>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;


#define BRACKET_OPEN (-1)
#define BRACKET_CLOSE (-2)
#define IS_DIGIT(A) ((A)>='0' && (A)<='9')

using namespace std;

typedef vector<int> intVec;


static int test_mat(){
    arma::mat testMat(2, 2);
    testMat(0, 0) = 1; testMat(0, 1) = 0;
    testMat(1, 0) = 1; testMat(1, 1) = 2;
    int rank = arma::rank(testMat);
    return rank;
}

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

PYBIND11_MODULE(CFGLearner, m) {
    py::class_<Teacher> teacher(m, "Teacher");
    py::class_<SimpleTeacher> simpleTeacher(m, "SimpleTeacher",teacher);
    simpleTeacher.def(py::init<>());
    simpleTeacher.def("addPositiveExample",[](SimpleTeacher& t,py::object nltkTree){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        t.addPositiveExample(*tree);
        delete(tree);
    });
    simpleTeacher.def("addNegativeExample",[](SimpleTeacher& t,py::object nltkTree){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        t.addNegativeExample(*tree);
        delete(tree);
    });
    simpleTeacher.def("__repr__",[](const SimpleTeacher& t){
        stringstream stream;
        stream << "<SimpleTeacher with ";
        stream << t.getPosNum() << " positive examples and ";
        stream << t.getNegNum() << " negative examples>";
        return stream.str();
    });
    py::class_<FrequencyTeacher> frequencyTeacher(m, "FrequencyTeacher",teacher);
    frequencyTeacher.def(py::init<int,float>(),py::arg("minCount")=0, py::arg("minFreq")=0.5f);
    frequencyTeacher.def("addPositiveExample",[](FrequencyTeacher& t,py::object nltkTree){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        t.addPositiveExample(*tree);
        delete(tree);
    });
    frequencyTeacher.def("addNegativeExample",[](FrequencyTeacher& t,py::object nltkTree){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        t.addNegativeExample(*tree);
        delete(tree);
    });
    frequencyTeacher.def("addPositiveExamples",[](FrequencyTeacher& t,py::object nltkTree, int num){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        t.addPositiveExamples(*tree, num);
        delete(tree);
    });
    frequencyTeacher.def("addNegativeExamples",[](FrequencyTeacher& t,py::object nltkTree, int num){
        if(!checkType(nltkTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        ParseTree* tree = parseTree(str);
        t.addNegativeExamples(*tree, num);
        delete(tree);
    });
    frequencyTeacher.def("__repr__",[](const FrequencyTeacher& t){
        stringstream stream;
        stream << "<FrequencyTeacher with minCount=";
        stream << t.getMinCount() << " and minFreq=";
        stream << t.getMinFreq() << ">";
        return stream.str();
    });
    py::class_<DifferenceTeacher> differenceTeacher(m, "DifferenceTeacher",teacher);
    differenceTeacher.def(py::init<int>(),py::arg("maxDiff")=0);
    differenceTeacher.def("addPositiveExample",[](DifferenceTeacher& t,py::object nltkTree, py::object weightsTree){
        if(!checkType(nltkTree) || !checkType(weightsTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        string weightsStr = py::str(weightsTree);
        ParseTree* tree = parseTree(str);
        ParseTree* weightParseTree = parseTree(weightsStr);
        tree->applyWeights(*weightParseTree);
        t.addPositiveExample(*tree);
        delete(tree);
        delete(weightParseTree);
    });

    differenceTeacher.def("addNegativeExample",[](DifferenceTeacher& t,py::object nltkTree, py::object weightsTree){
        if(!checkType(nltkTree) || !checkType(weightsTree)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str = py::str(nltkTree);
        string weightsStr = py::str(weightsTree);
        ParseTree* tree = parseTree(str);
        ParseTree* weightParseTree = parseTree(weightsStr);
        tree->applyWeights(*weightParseTree);
        t.addNegativeExample(*tree);
        delete(tree);
        delete(weightParseTree);
    });
    py::class_<TreeComparator> treeComparator(m, "TreeComparator");
    treeComparator.def(py::init<int, int, int>());
    treeComparator.def("compare", [](TreeComparator& c, py::object nltkTree1, py::object nltkTree2){
        if(!checkType(nltkTree1) || !checkType(nltkTree2)){
            throw std::invalid_argument("Must give an nltk tree");
        }
        string str1 = py::str(nltkTree1);
        string str2 = py::str(nltkTree2);
        ParseTree* tree1 = parseTree(str1);
        ParseTree* tree2 = parseTree(str2);
        int score = c.compare(*tree1, *tree2);
        delete(tree1);
        delete(tree2);
        return score;
    });
    differenceTeacher.def("setTreeComparator", [](DifferenceTeacher& t, TreeComparator& c){
        t.setTreeComparator(c);
    });
    m.def("learn",[](const Teacher& t, std::unordered_map<int,string> map) {
        py::gil_scoped_release release;
        TreeAcceptor acc = learn(t);
        CFG c(acc,map);
        py::gil_scoped_acquire acquire;
        py::object nltk = py::module::import("nltk");
        py::object nltkCFG = nltk.attr("CFG");
        py::object nltkCFGFromString = nltkCFG.attr("fromstring");
        return nltkCFGFromString(c.getRepr());
    });
    m.def("test_arma",[]() {
        return test_mat();
    });
    m.doc() = "A module used to learn context free grammars from structural data.";
}