/*
 * difficult_cipher.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: Arthur Laks
 * The main class used in the solution of the hill cipher.
 */

#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <vector>
#include <random>
#include <unordered_set>

#include "math_lib.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::istream_iterator;
using std::ifstream;
using std::ofstream;
using std::vector;

//Converts a string to a vector of numbers between 0 and 25.  All characters besides uppercase letters are ignored.
vector<int> to_numbers(const string& text)
{
    vector<int> retval;

    //Transform the string to a vector of ints where each uppercase character is a number between 0 and 25.
    std::transform(text.begin(),text.end(),std::back_inserter(retval),
                   [](char c_char) {return static_cast<int>(c_char - 'A');});

    //Remove all characters that are not uppercase letters.
    retval.erase(std::remove_if(retval.begin(),retval.end(),
              [](int c_int) { return c_int > 25 || c_int < 0;}),retval.end());
    return retval;
}

//Takes a matrix and displays it.
void display_matrix(const Matrix& matrix){
    for(auto row : matrix){
        for(int element:row)        {
            //Put a tab between columns and a newline between rows.
            cout << element << "\t";
        }
        cout << endl;
    }
}

//Converts an iterator to a matrix with the specified number of rows and columns.
template<typename iterator>
Matrix to_matrix(iterator begin,unsigned rows,unsigned columns){

    Matrix retval(rows,vector<int>(columns));
    for(unsigned i = 0;i < rows;++i){
        std::copy(begin + i * columns,begin + (i + 1) * columns,retval[i].begin());
    }
    return retval;
}

int main(int argc,char* args[]){
  if(argc < 5){
    cerr << "Usage: hill_cipher known_plaintext_file known_ciphertext_file "
      "unknown_ciphertext_file decryption_file" << endl;
  return 1;
  }
  
    //Open the known plaintext and ciphertext files and read them into strings.
  ifstream known_ciphertext_stream(args[1]);
  ifstream known_plaintext_stream(args[2]);

    if(!known_ciphertext_stream || !known_plaintext_stream){
      cerr << "File does not exist." << endl;
      return 2;
    }
    string known_ciphertext(istream_iterator<char>(known_ciphertext_stream),(istream_iterator<char>()));
    string known_plaintext(istream_iterator<char>(known_plaintext_stream),(istream_iterator<char>()));

    vector<int> ciphertext_numbers = to_numbers(known_ciphertext);
    vector<int> plaintext_numbers = to_numbers(known_plaintext);


    const unsigned int BLOCK_SIZE = 5;

    Matrix plaintext_matrix;

    Matrix ciphertext_matrix;
    //Find the first 5 blocks of plaintext that form a matrix that is invertible mod 26.
    for(int offset = 0;offset < 45;++offset){
        //Convert the block of 25 characters starting with offset to a matrix.
        plaintext_matrix = to_matrix(plaintext_numbers.begin() + offset * BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE);
        ciphertext_matrix = to_matrix(ciphertext_numbers.begin() + offset * BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE);

        //Determine if the matrix is invertible.
        bool invertible = invert(plaintext_matrix,26);
        if(invertible){
            cout << "Success!" << endl;
            break;
        }
    }


    //Recover the key by multiplying the inverse of the plaintext matrix by the corresponding ciphertext matrix.
    Matrix key = multiply(plaintext_matrix,ciphertext_matrix,26);
    cout << "Here is the key used for encryption: " << endl;
    display_matrix(key);

    //Invert the key in order to obtain the matrix used for decryption.
    invert(key,26);
    cout << "Here is the key used for decryption: " << endl;
    display_matrix(key);

    //Open the unknown ciphertext file.
    std::ifstream ciphertext_file(args[3]);
    if(!ciphertext_file){
      cerr << "Unknown ciphertext file does not exist." << endl;
      return 2;
    }
    string unknown_ciphertext_string(istream_iterator<char>(ciphertext_file),(istream_iterator<char>()));
    vector<int> unknown_ciphertext = to_numbers(unknown_ciphertext_string);

    Matrix unknown_ciphertext_matrix
        = to_matrix(unknown_ciphertext.begin(),unknown_ciphertext.size() / BLOCK_SIZE,BLOCK_SIZE);

    //Decrypt the unknown ciphertext matrix by multiplying it by the inverse of the key.
    Matrix unknown_plaintext_matrix = multiply(unknown_ciphertext_matrix,key,26);

    //Write the decrypted plaintext to a file.
    std::ofstream output_file(args[4],std::ios::out);
    std::ostream_iterator<char> output(output_file);

    //Convert each row of the matrix to a string and write it to the file.
    for(auto block:unknown_plaintext_matrix){
        std::transform(block.begin(),block.end(),output,
            [](int n){return static_cast<char>(n + 'a');});
    }


    return 0;
}


