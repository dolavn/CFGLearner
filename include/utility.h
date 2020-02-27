
#ifndef CFGLEARNER_UTILITY_H
#define CFGLEARNER_UTILITY_H

#define ABS(x) ((x)>=0?(x):-(x))
#define MIN(x,y) ((x<=y)?(x):(y))
#define MAX(x,y) ((x>=y)?(x):(y))
#define SAFE_DELETE(ptr)  if(ptr){delete(ptr);ptr=nullptr;}

#include <vector>
#include <functional>

template<typename T>
inline void clear_vec(std::vector<T*>& v){
    for(auto elem: v){
        SAFE_DELETE(elem)
    }
}

template<typename T>
inline void deep_copy_vec(const std::vector<T*>& orig, std::vector<T*>& dest){
    for(auto elem: orig){
        dest.push_back(new T(*elem));
    }
}


#endif //CFGLEARNER_UTILITY_H
