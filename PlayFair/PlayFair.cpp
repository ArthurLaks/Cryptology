/*
 * File: PlayFair.cpp
 * Author: Arthur Laks

 * This file contains an implentation of the PlayFair cipher.  The user passes the names of
 * the input and output files and specifies whether to encrypt or decrypt.  The program asks
 * the user to enter a phrase as the key, and the program encrypts or decrypts the file based
 * on the key.
 */
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>
#include <cstring>
#include <iterator>
#include <cassert>
#include <unordered_set>
using std::string;
using std::pair;
using std::make_pair;
using std::cout;
using std::cin;
using std::endl;
using std::istream_iterator;

const size_t TABLE_LENGTH = 36;	//This constant is the total length of the key.
//Takes the table and a character and returns the row and column in which the character appears.
pair<int,int> table_lookup(char[][6],char);
//Takes a pair of chars and the key and encrypts them.
pair<char,char> encrypt_chars(char,char,char[][6]);
//Takes a pair of chars and the key and decrypts them.
pair<char,char> decrypt_chars(char,char,char[][6]);

//Takes a string with the keyword, concatenates the remaining letters of the alphabet to it,
//and turns it into table form, which it assigns to the second argument.
void construct_table(string,char[][6]);

int main(int argc,char* args[]){
	//The program expects three arguments: -e or -d to determine whether to encrypt or decrypt,
	//the name of the input file, and the name of the output file.
	if(argc != 4){
		std::cerr << "Usage: ./PlayFair -d|-e input_filename output_filename" << endl;
		return 1;
	}
	//Open input and output files.
	std::ifstream input_stream(args[2]);
	std::ofstream output_stream(args[3]);

	cout << "Enter the key, with '*' instead of space: " << endl;
	string key;
	cin >> key;

	//The key is represented as a 6x6 array.
	char table[6][6];
	construct_table(key,table);

	//If the first argument is -e then encrypt the file.
	if(std::strncmp(args[1],"-e",2) == 0){
		istream_iterator<char> position(input_stream);
		//istream_iterator's default constructor returns a iterator representing the end of the
		//file.
		while(position != istream_iterator<char>()){
			char a = *(position++);
			char b;
			//If a was the last character then pad the message by assigning 'X' to b.
			//If we are not at the end of the file then assign the next character to b.
			if(position != istream_iterator<char>()){
				b = *(position++);
			}else{
				b = 'X';
			}

			//If the two letters are the same, replace one of them by an infrequently used letter.
			if(a == b){
				//If they are already X them set one of them to Z.
				if(b != 'X'){
					b = 'X';
				}else{
					b = 'Z';
				}
			}
			//Encrypt the two characters and write them to the file.
			auto encrypted = encrypt_chars(a,b,table);
			output_stream << encrypted.first << encrypted.second;
		}
		return 0;
	}

	//If the decrypt option was specified.
	if(std::strncmp(args[1],"-d",2) == 0){
		//Assume that the number of characters in the file is even.
		istream_iterator<char> position(input_stream);
		while(position != istream_iterator<char>()){
			//Read two characters from the file, decrypt them, and write them to the output file.
			char a,b;
			a = *(position++);
			b = *(position++);
			auto decrypted = decrypt_chars(a,b,table);
			output_stream << decrypted.first << decrypted.second;
		}
		return 0;
	}
	//If neither -e nor -d was specified, then the user entered an invalid option.
	std::cerr << "Invalid option.  Valid options are -e for encrypt and -d for decrypt." << endl;
	return 1;
}

//This function turns the keyword entered by the user into a table with every letter of the
//cipher alphabet, and returns the result in the second argument.
void construct_table(string key,char table[][6]){
	//The key is a phrase.  Make sure that the key includes the entire alphabet by appending
	//the alphabet to it and eliminating duplicates.
	key = key + "ABCDEFGHIJKLMNOPRSTUVWXYZ*0123456789";

	//For every character in the string, if it already appeared then delete it.  Use a
	//hashset to keep track of which characters appeared so far.
	std::unordered_set<char> appeared;
	auto iter = key.begin();
	while(iter != key.end()){
		//If the character already appeared then erase it.  The count method returns 1 if the
		//element is present and 0 if it is not.
		if(appeared.count(*iter)){
			iter = key.erase(iter);	//erase will advance the iterator to the next character
		}else{
			//If the character did not appear so far then insert it to the hashtable, and advance
			//to the next position.
			appeared.insert(*iter);
			++iter;
		}
	}
	//Copy the key to the array that the user passed as an argument.  Even though it is a
	//two-dimensional array, treat it like a one-dimensional array using reinterpret_cast.
	std::copy(key.begin(),key.end(),reinterpret_cast<char*>(table));
}
pair<char,char> encrypt_chars(char first,char second,char  table[][6]){
	//Find the locations of the two characters within the tables.
	auto first_loc = table_lookup(table,first);
	auto second_loc = table_lookup(table,second);
	//If they are on the same row
	if(first_loc.first == second_loc.first){
		return make_pair(table[first_loc.first][(first_loc.second + 1) % 6],table[first_loc.first][(second_loc.second + 1) % 6]);
	}
	//If they are on the same column
	if(first_loc.second == second_loc.second){
		return make_pair(table[(first_loc.first + 1) % 6][first_loc.second],table[(second_loc.first + 1) % 6][first_loc.second]);
	}
	//If the are on different rows and columns.
	return make_pair(table[first_loc.first][second_loc.second],table[second_loc.first][first_loc.second]);
}

//This function is used when subtracting from n, to make sure that if the difference is negative,
//n will wrap to the other side of the table.
int wrap_back(int n){
	return n >= 0 ? n : 6 + n;
}
//Takes two characters and decrypts them.
pair<char,char> decrypt_chars(char first,char second,char table[][6]){
	auto first_loc = table_lookup(table,first);
	auto second_loc = table_lookup(table,second);
	//If they are on the same row
	if(first_loc.first == second_loc.first){
		//Return the characters with the characters before them on the table, wrapping them
		//back to the end of the table if the result is negative.
		return make_pair(table[first_loc.first][wrap_back(first_loc.second - 1)],table[first_loc.first][wrap_back(second_loc.second - 1)]);
	}
	//If they are on the same column
	if(first_loc.second == second_loc.second){
		return make_pair(table[wrap_back(first_loc.first - 1)][first_loc.second],table[wrap_back(second_loc.first - 1)][first_loc.second]);
	}
	//If they are in different rows and columns
	return make_pair(table[first_loc.first][second_loc.second],table[second_loc.first][first_loc.second]);
}


pair<int,int> table_lookup(char table[][6],char target){
	//Treat the table as a one-dimensional array of chars.
	char* flattened = reinterpret_cast<char*>(table);
	char * location = std::find(flattened,flattened + TABLE_LENGTH,target);
	auto offset = location - flattened;
	//This function relies on the fact an mxn two-dimensional array is laid out in memory as an
	//array of arrays, where each array is a row.  Therefore, if an element is in position x,y,
	//its offset from the beginning of the buffer is y * n + x.
	//offset / 6 is the row that the character is in.  offset % 6 is the column.
	return make_pair(offset / 6,offset % 6);
}

