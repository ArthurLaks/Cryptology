/*
 * File: KeyStream.h
 * Author: Arthur Laks

 * This file contains the definition of the KeyStream class and the implementation of its
 * templated methods.

 */

#include <deque>
#include <vector>
#include <iostream>
#include <bitset>
#include <sstream>
#include <algorithm>
#ifndef KEYSTREAM_H_
#define KEYSTREAM_H_

namespace LSFR {
class KeyStream {
public:
	//This type represents a single bit.  The only values it should store are 0 and 1.
	typedef char Bit;
	//This is the type of the iv and the key.
	typedef unsigned long int Bit_Sequence;

	KeyStream(Bit_Sequence iv,Bit_Sequence key);
	//Extracts a byte-worth of stream.
	KeyStream & operator >>(char&);

	//Turns a numeric value into a container of bits.
	template<typename Iterator>
	static void unpack(Bit_Sequence packed,Iterator start){
		for(unsigned int counter = 0;counter < sizeof(Bit_Sequence) * 8;++counter){
			//1 << counter is the bitmask.  It is anded with packed, which masks out all of the
			//other bits.  The result is shifted back by counter, so that the resulting char will
			//be either 0 or 1.

			//Cast 1 to an unsigned int in order to force the gcc to treat the shift as a
			//logical shift and not an arithmetic shift.
			*start++ = (((static_cast<Bit_Sequence>(1) << counter) & packed) >> counter);

		}
	}

	//Converts a container of bits into a numeric value
	template<typename Iterator>
	static Bit_Sequence pack(Iterator begin){
		Bit_Sequence retval = 0;
		for(unsigned long counter = 0;counter < sizeof(Bit_Sequence) * 8;++counter){
			//Set the nth bit of retval to the nth element in the container, shifted n to the
			//left, so that it would be either 0 or 1.
			retval |= *(begin + counter) << counter;
		}
		return retval;
	}
private:
	//The shift register is a deque in order to allow bits to be added to one end and removed
	//from the other.
	std::deque<Bit> shift_register;
	std::vector<Bit> key;
};

} /* namespace LSFR */
#endif /* KEYSTREAM_H_ */
