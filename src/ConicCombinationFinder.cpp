#include "ConicCombinationFinder.h"
#include "soplex.h"
#include <iostream> //todo: delete

using namespace std; //todo: delete
using namespace soplex;

ConicCombinationFinder::ConicCombinationFinder(arma::mat mat):mat(std::move(mat)),sol(),status(UNSOLVED){

}

void ConicCombinationFinder::solve(int col) {
    SoPlex mysoplex;
    mysoplex.setIntParam(SoPlex::VERBOSITY, SoPlex::VERBOSITY_ERROR);
    mysoplex.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MINIMIZE);
    DSVector dummycol(0);
    for (int i = 0; i < mat.n_cols-1; ++i) {
        mysoplex.addColReal(LPCol(1.0, dummycol, infinity, 0));
    }
    for (int i = 0; i < mat.n_rows; ++i) {
        DSVector row1((int) (mat.n_cols) - 1);
        int ind = 0;
        for (int j = 0; j < mat.n_cols; ++j) {
            if (j == col) { continue; }
            row1.add(ind++, mat(i, j));
        }
        mysoplex.addRowReal(LPRow(mat(i, col), row1, mat(i, col)));
    }
    mysoplex.writeFileReal("dump.lp", NULL, NULL, NULL);
    SPxSolver::Status stat;
    DVector prim((int) (mat.n_cols) - 1);
    stat = mysoplex.optimize();
    if (stat == SPxSolver::INFEASIBLE) {
        status = NO_SOL;
    } else {
        status = SOLVED;
        sol = arma::vec(mat.n_cols - 1);
        for (int i = 0; i < mat.n_cols - 1; ++i) {
            sol(i) = prim[i];
        }
        //cout << prim << endl;
        //std::cout << "Objective value is " << mysoplex.objValueReal() << ".\n";
    }
}

ConicCombinationFinder::Status ConicCombinationFinder::getStatus() const{
    return status;
}