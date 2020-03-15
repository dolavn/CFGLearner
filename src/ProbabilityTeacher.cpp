#include "MultiplicityTeacher.h"
#include "TreeComparator.h"
#include "ParseTree.h"
#include "MultiplicityTreeAcceptor.h"
#include <functional>
#include "Logger.h"
#include "TreesGenerator.h"
#include "math.h"

using namespace std;

ProbabilityTeacher::ProbabilityTeacher(TreeComparator& cmp, double decayFactor, double epsilon):
ComparatorTeacher(cmp, epsilon),decayFactor(decayFactor),countingFunc(),generator(nullptr){

}

ProbabilityTeacher::ProbabilityTeacher(TreeComparator& cmp, double decayFactor, countingFunction countingFunc,
        double epsilon):ComparatorTeacher(cmp, epsilon), decayFactor(decayFactor),countingFunc(move(countingFunc)),
        generator(nullptr){
    if(decayFactor>=1 || decayFactor<=0){
        throw invalid_argument("Decay factor must be between 0 and 1");
    }
}

ProbabilityTeacher::ProbabilityTeacher(const ProbabilityTeacher& other):ComparatorTeacher(other),
decayFactor(other.decayFactor),countingFunc(other.countingFunc),generator(other.generator->clone()){

}

ProbabilityTeacher::ProbabilityTeacher(ProbabilityTeacher&& other):ComparatorTeacher(move(other)),
decayFactor(other.decayFactor),countingFunc(move(other.countingFunc)),generator(other.generator){
    other.generator = nullptr;
}

double ProbabilityTeacher::calcNewProb(const ParseTree& tree1, const ParseTree& tree2, TreeComparator& cmp) const{
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    float result = cmp.compare(tree2, tree1);
    int numTreesWithResult = countingFunc?countingFunc(result, tree2.getLeavesNum()):1;
    double p = tree2.getProb();
    p = p*std::pow(decayFactor, result);
    p = p / numTreesWithResult;
    return p;
}

double ProbabilityTeacher::membership(const ParseTree& tree) const{
    double ans = ComparatorTeacher::membership(tree);
    return ans;
}

void ProbabilityTeacher::setupConstructorGenerator(TreeConstructor& constructor, int maxLen, int treesNum){
    if(this->generator){
        delete(this->generator);
        this->generator = nullptr;
    }
    set<rankedChar> alphabet = getAlphabet();
    vector<int> alphabet_vec;
    for(auto sym: alphabet){
        if(sym.rank==0){alphabet_vec.push_back(sym.c);}
    }
    this->generator = new ConstructorGenerator(constructor, maxLen, treesNum, alphabet_vec);
}


void ProbabilityTeacher::setupDuplicationsGenerator(int depth){
    if(this->generator){
        delete(this->generator);
        this->generator = nullptr;
    }
    this->generator = new DuplicationsGenerator(trees, depth);
}

void ProbabilityTeacher::setTreesGenerator(const TreesGenerator& generator){
    if(this->generator){
        delete(this->generator);
        this->generator = nullptr;
    }
    this->generator = generator.clone();
}

ProbabilityTeacher::~ProbabilityTeacher(){
    if(this->generator){
        delete(this->generator);
        this->generator = nullptr;
    }
}

//TODO: Ugly design. Fix this.
ParseTree* ProbabilityTeacher::equivalence(const MultiplicityTreeAcceptor& acc) const{
    Logger& logger = Logger::getLogger();
    std::function<bool(const ParseTree&, bool)> testFunc = [&acc,this](const ParseTree& t1, bool cache){
        double val = acc.run(t1);
        double prob = t1.getProb();
        if(!cache){
            prob = prob*(1-decayFactor);
        }
        return std::abs(val-prob)>epsilon;
    };
    if(!this->generator){
        for(auto& currTree: trees){
            if(testFunc(*currTree, false)){
                return new ParseTree(*currTree);
            }
        }
        for(auto& currTree: cache){
            if(testFunc(*currTree, true)){
                return new ParseTree(*currTree);
            }
        }
    }else{
        Logger& logger = Logger::getLogger();
        logger.setLoggingLevel(Logger::LOG_DEBUG);
        logger << "equivalence" << logger.endline;
        TreesGenerator& genRef = *generator;
        genRef.reset();
        while(genRef.hasNext()){
            ParseTree curr = *genRef;
            curr.setProb(membership(curr));
            if(curr.getProb()>0){
                logger << "tree:" << curr << logger.endline;
                logger << "tree-prob:" << curr.getProb() << logger.endline;
                logger << "acceptor(tree):" << acc.run(curr) << logger.endline;
            }
            if(testFunc(curr, true)){
                logger << curr << logger.endline;
                return new ParseTree(curr);
            }
            ++genRef;
        }
        logger << "all good" << logger.endline;
    }
    return nullptr;
}
