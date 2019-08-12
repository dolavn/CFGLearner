
#ifndef CFGLEARNER_UTILITY_H
#define CFGLEARNER_UTILITY_H

#define ABS(x) ((x)>=0?(x):-(x))
#define MIN(x,y) ((x<=y)?(x):(y))
#define MAX(x,y) ((x>=y)?(x):(y))
#define SAFE_DELETE(ptr)  if(ptr){delete(ptr);ptr=nullptr;}

#endif //CFGLEARNER_UTILITY_H
