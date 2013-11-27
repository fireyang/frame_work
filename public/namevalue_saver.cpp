/*
 * namevalue_saver.cpp
 *
 *  Created on: 2013-5-28
 *      Author: dukefu
 */

#include "namevalue_saver.h"
#include <fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <fcntl.h>


using namespace std;

namespace kingnet {
namespace knight_v2 {

namevalue_saver::namevalue_saver(const string& file_name)
	: file_name_(file_name)
{
	// TODO Auto-generated constructor stub
	file_name_ = file_name_ + ".data";
	mkdir(get_path().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

}

namevalue_saver::~namevalue_saver() {
	// TODO Auto-generated destructor stub
}


bool namevalue_saver::load()
{
	nv_int_.clear();

	string tmp_file_name = get_path()+"/" + file_name_ + ".tmp";
	string file_name = get_path()+"/" + file_name_;
	string old_file_name = get_path()+"/" + file_name_+ ".old";


	if (file_exists(tmp_file_name)) {
		remove(tmp_file_name.c_str());
	}

	if (file_exists(old_file_name)) {
		//file&tmp is untrusted
		copy_file(old_file_name, file_name);
		remove(old_file_name.c_str());
	}

	if (file_exists(file_name)) {

		ifstream in_file(file_name);
		string name;
		int value;
		int expiration = 0;
		while(in_file >> name) {
			in_file >> value;

			nv_int_[name] = value;
			if (name == "[EXPIRATION]") {
				expiration = value;
				if (expiration != 0) {
					if (time(NULL) >= expiration) {
						nv_int_.clear();

						return false;
					}
				}
			}
		}

		return true;
	}


	return false;
}

void namevalue_saver::save(time_t exipration)
{
	string tmp_file_name = get_path()+"/" + file_name_ + ".tmp";
	string file_name = get_path()+"/" + file_name_;
	string old_file_name = get_path()+"/" + file_name_+ ".old";

	rename(file_name.c_str(), old_file_name.c_str());


	ofstream out_file(tmp_file_name);

	out_file << "[EXPIRATION]" << endl;
	out_file << exipration << endl;

	for (auto nv=nv_int_.begin(); nv!=nv_int_.end();++nv) {
		out_file << nv->first << endl;
		out_file << nv->second << endl;
	}

	rename(tmp_file_name.c_str(), file_name.c_str()); //delete tmp
	remove(old_file_name.c_str()); //delete old


}

void namevalue_saver::clear()
{
	nv_int_.clear();
}

bool namevalue_saver::file_exists(const string& file_name)
{
	if (access(file_name.c_str(), 0) == -1) {
		return false;
	}
	return true;
}

string namevalue_saver::get_path()
{
	return "./file_saver";
}


void namevalue_saver::copy_file(const string& source, const string& destination)
{
	int input, output;

	if( (input = open(source.c_str(), O_RDONLY)) == -1) {
	    return;
	}

	if( (output = open(destination.c_str(), O_WRONLY | O_CREAT)) == -1)
	{
	        close(input);
	        return;
	}

	off_t bytesCopied;
	struct stat sourceStat;

	if (!stat(source.c_str(), &sourceStat)) {
	   sendfile(output, input, &bytesCopied, sourceStat.st_size);
	}

	close(input);
	close(output);

}

void namevalue_saver::set(string name, int value)
{
	nv_int_[name] = value;
}

int  namevalue_saver::get(string name)
{
	int ret  = -1;

	auto p = nv_int_.find(name);
	if (p != nv_int_.end()) {
		ret = p->second;
	}

	return ret;
}

} /* namespace knight_v2 */
} /* namespace kingnet */
