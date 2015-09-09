//Author: Arthur Laks
//File: main.cpp

//This file contains my implementation of the Rabin cryptosystem, which uses the gmp library.
//Key generation works, but the decryption operation does not produce the plaintext that encrypted using the encryption
//operation.

#include <iostream>
#include <fstream>
#include <string>
#include <gmp.h>
#include <ctime>
#include <cstring>
#include <gmpxx.h>
#include <cassert>

using std::endl;
using std::cout;
using std::cin;
using std::string;
using std::ifstream;
using std::ofstream;

//The number of bits in n.
const unsigned int NUM_BITS = 1024;
//The number of bytes in one block.
const unsigned int BLOCK_SIZE =  (NUM_BITS >> 5);

//Generates random private and public keys and writes them to the respective files.  The keys are written in plain text in
//base 10 in order to be human readable.
void generate_key(ofstream& pub_key_file,ofstream& priv_key_file){
    //Uses the Mersenne Twister algorithm for random number generation.
    gmp_randclass state(gmp_randinit_mt);
    //Seed the generator to the current time.
    state.seed(std::time(NULL));

    mpz_class p,q;
    //For both q and q, generate random numbers until a prime number is reached such that n = 1 mod 4
    do{
        //p and q should have half the number of bits as n.
        p = state.get_z_bits(NUM_BITS / 2);
    }while(!(p % 4 == 3 && mpz_probab_prime_p(p.get_mpz_t(),25)));
    do{
        q = state.get_z_bits(NUM_BITS / 2);
    }while(!(q % 4 == 3 && mpz_probab_prime_p(q.get_mpz_t(),25)));
    mpz_class n = p * q;
    //Write the numbers to the respective files.
    pub_key_file << "n = " << n << endl;
    priv_key_file << "p = " << p << endl;
    priv_key_file << "q = " << q << endl;
}

//Writes an mpz_class to a file in its binary represenation.
void write_mpz(ofstream& dest,const mpz_class& source){
    static char buffer[BLOCK_SIZE];
    //Clear the buffer
    std::memset(buffer,0,BLOCK_SIZE);
    size_t bytes_written;
    mpz_export(buffer,&bytes_written,1,1,0,0,source.get_mpz_t());
    dest.write(buffer,bytes_written);
}

//Reads an mpz_class from a file
void read_mpz(mpz_class& dest,ifstream& source){
    static char buffer[BLOCK_SIZE];
    std::memset(buffer,0,BLOCK_SIZE);
    source.read(buffer,BLOCK_SIZE);
    mpz_import(dest.get_mpz_t(),source.gcount(),1,1,0,0,buffer);
}

//Takes the public key, and input file, and an output file, reads each block from the input file, encrypts the block,
//and writes it to the output file.
void encrypt(const mpz_class& n,std::ifstream& plaintext_file,std::ofstream& ciphertext_file){
    mpz_class plaintext,ciphertext;
    while(plaintext_file) {
        read_mpz(plaintext,plaintext_file);

        mpz_powm_ui(ciphertext.get_mpz_t(),plaintext.get_mpz_t(),2,n.get_mpz_t());

        write_mpz(ciphertext_file,ciphertext);
    }
}

//Reduces x mod n, handling the case where x is negative properly (not like it is handled by programming languages).
mpz_class mod(mpz_class x,mpz_class n){
    mpz_class retval = x % n;
    if(x < 0){
        retval = n + retval;
    }
    return retval;
}


//Reads each block from the input file, decrypts it to the four candidate decryptions using the private key, and writes
//each candidate decryption to a different output file.
void decrypt(const mpz_class& p,const mpz_class& q,ifstream& ciphertext_file,ofstream (&output_files)[4]){
    mpz_class n = p * q;
    mpz_class ciphertext,exponent,a,b,r,s,x;
    while(ciphertext_file){
        read_mpz(ciphertext,ciphertext_file);

        exponent = (p + 1) / 4;
        mpz_powm(r.get_mpz_t(),ciphertext.get_mpz_t(),exponent.get_mpz_t(),p.get_mpz_t());

        exponent = (q + 1) / 4;
        mpz_powm(s.get_mpz_t(),ciphertext.get_mpz_t(),exponent.get_mpz_t(),q.get_mpz_t());

        //The first argument from mpz_gcdext is the gcd of the two arguments.  It is not needed and cannot be NULL, so
        //I use exponent because it is not needed for the rest of the iteration.
        mpz_gcdext(exponent.get_mpz_t(),a.get_mpz_t(),b.get_mpz_t(),p.get_mpz_t(),q.get_mpz_t());
        x = mod(a * p * s + b * q * r,n);


        write_mpz(output_files[0],x);
        write_mpz(output_files[1],n - x);

        x = mod(a * p * s - b * q * r,n);
        write_mpz(output_files[2],x);
        write_mpz(output_files[3],n - x);

    }
}

int main(){



    cout << "Press \'g\' to generate a key, \'e\' to encrypt a file based on a public key, or \'d\' to decrypt a file "
        "based on a private key: ";
    char choice;
    cin >> choice;
    switch(choice){
        case 'g':
        //Put each case in its own scope in order to be able to reuse variable names.
        {
            cout << "Enter the name of the file to hold the public key: ";
            string pub_key_filename;
            cin >> pub_key_filename;
            cout << "Enter the name of the file to hold the private key: ";
            string priv_key_filename;
            cin >> priv_key_filename;
            ofstream priv_key_file(priv_key_filename.c_str()),pub_key_file(pub_key_filename.c_str());
            generate_key(pub_key_file,priv_key_file);
        break;
        }

    case 'e':
        {
            cout << "Enter the name of the file with the public key: ";
            string key_filename,plaintext_filename,ciphertext_filename;
            cin >> key_filename;


            cout << "Enter the name of the file to encrypt: ";
            cin >> plaintext_filename;
            cout << "Enter the name of the file to store the ciphertext: ";
            cin >> ciphertext_filename;

            ifstream key_file(key_filename.c_str());
            //The way the key is outputed by generate_key, it starts with "n = ".  Ignore those characters.
            key_file.ignore(3);
            string key_str;
            key_file >> key_str;
            key_file.close();

            mpz_class key(key_str,10);
            ifstream plaintext_file(plaintext_filename.c_str(),std::ios::binary | std::ios::in);
            ofstream ciphertext_file(ciphertext_filename.c_str(),std::ios::binary | std::ios::out);

            encrypt(key,plaintext_file,ciphertext_file);
            break;
        }
        case 'd':
        {

        string priv_key_filename,ciphertext_filename;

        cout << "Enter the name of the file with the private key: ";
        cin >> priv_key_filename;

        cout << "Enter the name of the ciphertext file: ";
        cin >> ciphertext_filename;

        string plaintext_filenames[4];
        cout << "Enter the names of the four files with the candidate plaintexts, each on a seperate line: ";
        for(int counter = 0;counter < 4;++counter){
            cin >> plaintext_filenames[counter];
        }

        ifstream priv_key_file(priv_key_filename.c_str());
        string p_string,q_string;
        priv_key_file.ignore(3);
        priv_key_file >> p_string;
        mpz_class p(p_string,10);
        priv_key_file.ignore(4);
        priv_key_file >> q_string;
        priv_key_file.close();
        mpz_class q(q_string,10);


        ifstream ciphertext_file(ciphertext_filename.c_str(),std::ios::binary | std::ios::in);
        ofstream plaintext_files[4];
        for(int counter = 0;counter < 4;++counter){
            plaintext_files[counter].open(plaintext_filenames[counter].c_str(),std::ios::binary | std::ios::out);
        }

        decrypt(p,q,ciphertext_file,plaintext_files);
        break;

    }
    default:
        std::cerr << "Invalid option." << endl;

    }

    return 0;

}

