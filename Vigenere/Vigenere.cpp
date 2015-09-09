/*
 * Author: Arthur Laks
 * File: Vigenerre.cpp
 *
 *  Created on: Mar 4, 2015
 * This program attempts to cryptanalyze a Vigenere cipher by trying every keyword length until
 * 50 and using the length with the index of coincidence closest to 0.065.  Based on the
 * keyword length, it guesses the keyword based on the technique from the textbook and decrypts
 * the text using on that keyword.
 */
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>

using std::map;
using std::vector;
using std::string;
using std::cout;
using std::endl;

int shift_back(int,int);
//This array stores the frequencies of the letters A, B, etc in English writing.
double frequencies_in_english[] = {0.08167,0.01492,0.02782,0.04253,0.12702,0.02228,0.02015,0.06094,0.06966,
		0.00153,0.00772,0.04025,0.02406,0.06749,0.07507,0.01929,0.00095,0.05987,0.06327,0.09056,
		0.02758,0.00978,0.02360,0.00150,0.01974,0.00074};
//The name of the file to be cryptanlayzed should be the only parameter.
int main(int argc,char* args[]){
	if(argc < 2){
		std::cerr << "Usage: Vigenere filename" << endl;
		return 1;
	}
	//Read the cipher text from a text file.  This method eliminates all whitespace.
	std::ifstream file(args[1]);
	//The inner parentheses are to avoid the C++ most vexing parse.
	string  cipher_text(std::istream_iterator<char>(file),(std::istream_iterator<char>()));

	//Find the keyword length that makes the index of coincidence closest to 0.065.
	double closest_ioc = 100;	 //The distance from 0.065 of the ioc obtained from the best
	//keyword length that was tested.
	int best_length = 0;	//The best keyword length tested so far.
	string best_keyword;	//The keyword if the keyword length is best_length

	//For every possible keyword length between 2 and 50.
	for(int m = 2;m < 50;++m){
		//Divide the text into substrings, and calculate their index of coincidence.
		vector<string> substrings(m);
		for(unsigned int counter = 0;counter < cipher_text.length();++counter){
			substrings[counter % m].push_back(cipher_text[counter]);
		}

		//Calculate the ioc of each substring and find the letter of the keyword used for that
		//substring.  Add up the ioc's in order to calculate their average.

		double total_ioc = 0;
		string keyword;
		for(auto c_string:substrings){
			//Calculate the ioc of the substring.

			//Count the frequencies of each letter.
			map<char,int> frequencies;
			for(auto iter = c_string.begin();iter < c_string.end();++iter){
				++frequencies[*iter];
			}
			double ioc = 0;
			for(auto pair:frequencies){
				ioc += pair.second * (pair.second - 1);
			}
			ioc /= (c_string.size() * (c_string.size() - 1));

			total_ioc += ioc;

			//Find the keyword based on the formula from the textbook, page 35.
			//Find the value of g that will cause mg to be closest to 0.065
			double closest_approximation = 100;
			int best_guess;		//The offset from the beginning of the alphabet of the best
			//letter found so far.
			//For each possible value of g.
			for(int guess = 0;guess < 26;++guess){
				double mg = 0;
				for(int i = 0;i < 26;++i){
					mg += frequencies_in_english[i] * frequencies[((i + guess) % 26) + 'A'] / c_string.length();
				}
				double distance_from_norm = std::abs(mg - 0.065);
				if(distance_from_norm < closest_approximation){
					closest_approximation = distance_from_norm;
					best_guess = guess;
				}
			}
			//Add that letter to the keyword.
			keyword.push_back('A' + best_guess);
		}

		//Determine if this keyword length produces an ioc closer to 0.065 than the best keyword
		//length found so far.
		if(std::abs(total_ioc / m - 0.065) < closest_ioc){
			closest_ioc = std::abs(total_ioc / m - 0.065);
			best_length = m;
			best_keyword = keyword;
		}
	}

	cout << "Here is the keyword length: " << best_length << endl;
	cout << "Here is the keyword: " <<  best_keyword << endl;

	//Determine the plaintext by shifting every letter in the plaintext back by the correct
	//number of positions.
	std::stringstream plaintext;
	for(unsigned  counter = 0;counter < cipher_text.length();++counter){
		plaintext << static_cast<char>(shift_back(cipher_text[counter] - 'A',best_keyword[counter % best_length] - 'A') + 'A');
	}

	cout << "Here is the plaintext:" << endl << plaintext.str() << endl;

	return 0;
}

//Performs subtraction mod 26, assuming that a and b are between 0 and 25.
int shift_back(int a,int b){
	if(a >= b)
		return a - b;
	else
		return 26 + (a - b);
}
