/*
 * File: Main.cpp
 * Author: Arthur Laks
 *
 * This is the driver of the LFSR program.  It asks the user for an input file and a file with
 * the key, and encrypts or decrypts the file based on the key.  In encryption mode, the
 * initialization vector is generated randomly and inserted at the beginning of the output file.

 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <ctime>
#include <cstdlib>
#include "KeyStream.h"
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::hex;
using std::string;

using LSFR::KeyStream;
int main(){
	string input_filename,key_filename,output_filename;
	cout << "Enter the name of the input file: ";
	cin >> input_filename;
	cout << "Enter the name of the output file: ";
	cin >> output_filename;
	cout << "Enter the name of the file with the key (a binary file with 64 bits): ";
	cin >> key_filename;

	std::ifstream input_file(input_filename.c_str(),std::ios::binary|std::ios::in);
	std::ofstream output_file(output_filename.c_str(),std::ios::binary|std::ios::out);
	std::ifstream key_file(key_filename.c_str(),std::ios::binary|std::ios::in);

	KeyStream::Bit_Sequence key;
	key_file.read(reinterpret_cast<char*>(&key),sizeof(key));

	cout << "Is the file being encrypted or decrypted (type \'e\' for encryption and \'d\' "
			"for decryption): ";
	char mode;
	cin >> mode;
	KeyStream::Bit_Sequence iv;

	//If the file is being decrypted, read the iv from the file.  If it is being encrypted,
	//generate it randomly and append it to the beginning of the file.
	if(mode == 'd'){
		input_file.read(reinterpret_cast<char*>(&iv),sizeof(KeyStream::Bit_Sequence));

	}else{
		std::srand(std::time(NULL));
		iv = std::rand();
		output_file.write(reinterpret_cast<char*>(&iv),sizeof(KeyStream::Bit_Sequence));//and assign iv to it.
	}
	KeyStream key_stream(iv,key);

	while(input_file){
		//Extract a block of key from the keystream, read a block from the input file, xor
		//them, and write the result to the output file.
		char current_key;
		key_stream >> current_key;


		char current_input;
		input_file.read(reinterpret_cast<char*>(&current_input),sizeof(char));

		KeyStream::Bit_Sequence current_output = current_key ^ current_input;
		output_file.write(reinterpret_cast<char*>(&current_output),sizeof(char));
	}





	return 0;
}

