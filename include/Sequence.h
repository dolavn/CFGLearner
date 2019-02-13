#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include <vector>
#include <ostream>
#include <sstream>
#include <string>
enum Strand { SENSE, ANTI_SENSE };

class Sequence {
public:
	explicit Sequence(std::vector<int> seq) :seq(std::move(seq)) {
	}

	Sequence(const Sequence& other) :seq(other.seq) {
	}

	Sequence() :seq() {

	}

	~Sequence()=default;

	inline int& operator[](int ind) { return seq[ind]; }

	inline int getValue(int ind) const {return seq[ind];}

	inline int getLength() const { return (int)(seq.size()); }

	std::vector<int> subseq(int, int) const;

    std::string toString() const{
	    std::stringstream sstream;
	    sstream << "[";
	    for(int elem: seq){
	        sstream << elem;
	        sstream << " , ";
	    }
	    sstream << "]";
	    return sstream.str();
	}

private:
	std::vector<int> seq;
	std::vector<Strand> strandDir;
};

std::ostream& operator<<(std::ostream& stream, const Sequence& seq);

#endif
