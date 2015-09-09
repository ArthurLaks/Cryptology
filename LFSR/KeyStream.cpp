/*
 * File: KeyStream.cpp
 * Author: Arthur Laks

 Contians the definitions of the non-templated methods fo the KeyStream class.

 */

#include "KeyStream.h"
#include <numeric>	//For inner_product
#include <iterator>

namespace LSFR {

	//Extracts a byte-worth of keystream.
	KeyStream& KeyStream::operator>>(char& result){
	  std::vector<Bit> unpacked(8);	//Store the result as a vector of bits before packing them
		//into a byte and assigning result to it.
		//For each bit of keystream to be generated

		//Uses the formula in the textbook on page 24.
		//For every bit to generate
		for(unsigned int counter = 0;counter < sizeof(char) * 8;++counter){
			unpacked[counter] = shift_register.front();	//Tap the bit in the front of the
			//register at the next bit in the stream.

			//The value of the new bit at the back of the register should be the xor of a
			//subset of the bits in the register.  That subset is the at the indices i for which
			//key[i] = 1.  This is implemented with the inner_product algorithm.
			shift_register.push_back(std::inner_product(key.begin(),key.end(),shift_register.begin(),0) % 2);
			shift_register.pop_front();
		}
		//Pack the vector of bits into a single byte.
		result = pack(unpacked.begin());
		return *this;
	}
	//Construct a KeyStream with the specified initialization vector and key.
	KeyStream::KeyStream(Bit_Sequence iv,Bit_Sequence key):
			shift_register(sizeof(Bit_Sequence) * 8),
			key(sizeof(Bit_Sequence) * 8){
		//Initialize the shift register to the iv.
		unpack(iv,shift_register.begin());
		unpack(key,this->key.begin());
	}


} /* namespace LSFR */
