// Workshop 9 - Multi-Threading
// SecureData.cpp
// Name: Mitchell Culligan
// Seneca Student ID: 161293170
// Seneca email: mculligan@myseneca.ca
// Date of completion: Nov 22 2019
//
// I confirm that the content of this file is created by me,
//   with the exception of the parts provided to me by my professor.
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <functional>
#include<vector>
#include "SecureData.h"

using namespace std;

namespace w9 {

	void converter(char* t, char key, int n, const Cryptor& c) {

	    for (int i = 0; i < n; i++)
			t[i] = c(t[i], key);
	}

	SecureData::SecureData(const char* file, char key, ostream* pOfs)
	{
		ofs = pOfs;

		// open text file
		fstream input(file, std::ios::in);
		if (!input)
			throw string("\n***Failed to open file ") +
			string(file) + string(" ***\n");

		// copy from file into memory
		input.seekg(0, std::ios::end);
		nbytes = (int)input.tellg() + 1;

		text = new char[nbytes];

		input.seekg(ios::beg);
		int i = 0;
		input >> noskipws;
		while (input.good())
			input >> text[i++];
		text[nbytes - 1] = '\0';
		*ofs << "\n" << nbytes - 1 << " bytes copied from file "
			<< file << " into memory (null byte added)\n";
		encoded = false;

		// encode using key
		code(key);
		*ofs << "Data encrypted in memory\n";
	}

	SecureData::~SecureData() {
		delete[] text;
	}

	void SecureData::display(std::ostream& os) const {

	    if (text && !encoded)
			os << text << std::endl;
		else if (encoded)
			throw std::string("\n***Data is encoded***\n");
		else
			throw std::string("\n***No data stored***\n");
	}

	void SecureData::code(char key)
	{
		// TODO (at-home): rewrite this function to use at least two threads
		//         to encrypt/decrypt the text.
		std::vector<std::thread> threads;

        const size_t threadNum = std::thread::hardware_concurrency();
        size_t num=0;
        if(threadNum < 2) throw std::string("no threads :(");
      auto convert = std::bind(converter,std::placeholders::_1 ,key,this->nbytes/threadNum,(Cryptor()));

            for(size_t i=1;i<=threadNum;i++) {
                threads.push_back(std::thread(convert, this->text +num));
                num+=this->nbytes /threadNum;
            }
            std::for_each(threads.begin(),threads.end(),[](std::thread& t){
                t.join();
            });

      //  t1.join();
	//	t2.join();



		this->encoded = !this->encoded;
	}

	void SecureData::backup(const char* file) {


        if (!text)
			throw std::string("\n***No data stored***\n");
		else if (!encoded)
			throw std::string("\n***Data is not encoded***\n");
		else
		{
		    // TODO: open a binary file for writing
		    //not checking if file is = nullptr
                std::fstream f(file, std::ios::out|std::ios::binary);

                if(f.is_open()) {
                    f.write(this->text,this->nbytes);
                    f.close();
                    // TODO: write data into the binary file
                    //         and close the file
                }
                else
                    throw std::string("File failed to open");
		}
	}

	void SecureData::restore(const char* file, char key) {
		// TODO: open binary file for reading

        std::fstream f(file,std::ios::in|std::ios::binary);
        if(f.is_open()) {
            delete[] this->text;
            f.seekg(0, std::ios::end);
            this->nbytes = (int)f.tellg();
            f.seekg(0);
            this->text = new char[this->nbytes+1];
            f.read(this->text,this->nbytes);

            f.close();
            // TODO: - allocate memory here for the file content




            // TODO: - read the content of the binary file
        }
        else
            throw std::string("File failed to open");
		*ofs << "\n" << nbytes << " bytes copied from binary file "
			<< file << " into memory.\n";

		encoded = true;

		// decode using key
		code(key);

		*ofs << "Data decrypted in memory\n\n";
	}

	std::ostream& operator<<(std::ostream& os, const SecureData& sd) {
		sd.display(os);
		return os;
	}
}
