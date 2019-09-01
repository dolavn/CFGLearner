//
// Created by dolavn on 8/29/19.
//

#ifndef CFGLEARNER_CONICCOMBINATIONFINDER_H
#define CFGLEARNER_CONICCOMBINATIONFINDER_H

#include <armadillo>


class ConicCombinationFinder{
public:
    ConicCombinationFinder(arma::mat);

    void solve(int);

    enum Status{
        UNSOLVED,
        NO_SOL,
        SOLVED
    };

    Status getStatus() const;
    arma::vec getSolution() const;
private:
    arma::mat mat;
    arma::vec sol;
    Status status;
};

#endif //CFGLEARNER_CONICCOMBINATIONFINDER_H
