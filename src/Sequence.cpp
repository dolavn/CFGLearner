#include "Sequence.h"
#include <iostream>

using namespace std;

ostream& operator<<(ostream& stream, const Sequence& seq){
    stream << seq.toString();
    return stream;
}

vector<int> Sequence::subseq(int begin, int end) const{
    return vector<int>(seq.begin()+begin, seq.begin()+end);
}

int main(int argc, char** argv){
    std::cout << "Hello world!" << std::endl;
    return 0;
}
