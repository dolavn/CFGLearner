//
// Created by dolavn on 8/7/19.
//

#ifndef CFGLEARNER_RANKEDCHAR_H
#define CFGLEARNER_RANKEDCHAR_H

struct rankedChar{
    int c;
    int rank;

    friend inline bool operator==(const rankedChar& lhs,const rankedChar& rhs){
        return lhs.c==rhs.c && lhs.rank==rhs.rank;
    }

    friend inline bool operator<(const rankedChar& lhs,const rankedChar& rhs){
        if(lhs.c==rhs.c){
            return lhs.rank<rhs.rank;
        }
        return lhs.c<rhs.c;
    }
};


#endif //CFGLEARNER_RANKEDCHAR_H
