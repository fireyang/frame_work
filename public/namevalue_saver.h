/*
 * namevalue_saver.h
 *
 *  Created on: 2013-5-28
 *      Author: dukefu
 */

#ifndef NAMEVALUE_SAVER_H_
#define NAMEVALUE_SAVER_H_

#include <string>
#include <map>
using namespace std;

namespace kingnet {
namespace knight_v2 {

class namevalue_saver {
public:
	namevalue_saver(const string& file_name);
	virtual ~namevalue_saver();

	bool load();
	void save(time_t exipration = 0);

	void set(string name, int value);
	int  get(string name);
	void clear();
	//void add(string name, string value);
private:
	string file_name_;
	string get_path();
	bool file_exists(const string& file_name);
	void copy_file(const string& from, const string& to);

	map<string, int> nv_int_;

};

} /* namespace knight_v2 */
} /* namespace kingnet */
#endif /* NAMEVALUE_SAVER_H_ */
